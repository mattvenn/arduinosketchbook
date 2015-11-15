import unittest
import serial
import time
import logging
import math
import struct
import pickle
import crcmod

#status
BUFFER_OK = 0
BUFFER_EMPTY = 1
BUFFER_FULL = 2
BAD_CKSUM = 3
MISSING_DATA = 4
BUFFER_LOW = 5
BUFFER_HIGH = 6

#commands
START = 0
STOP = 1
LOAD = 2
FLUSH = 3

buflen = 32
freq = 50.0

logging.basicConfig(level=logging.DEBUG)
crc8_func = crcmod.predefined.mkPredefinedCrcFun("crc-8-maxim")

class TestBuffer(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls._serial_port=serial.Serial()
        cls._serial_port.port='/dev/ttyACM0'
        cls._serial_port.timeout=1
        cls._serial_port.baudrate=115200
        cls._serial_port.open()

        time.sleep(2);

    @classmethod
    def tearDownClass(cls):
        cls._serial_port.close()

    def status_str(self, status):
        if status == BUFFER_OK:
            return 'BUFFER_OK'
        if status == BUFFER_EMPTY:
            return 'BUFFER_EMPTY'
        if status == BUFFER_FULL:
            return 'BUFFER_FULL'
        if status == BAD_CKSUM:
            return 'BAD_CKSUM'
        if status == MISSING_DATA:
            return 'MISSING_DATA'
        if status == BUFFER_LOW:
            return 'BUFFER_LOW'
        if status == BUFFER_HIGH:
            return 'BUFFER_HIGH'
        if status == START:
            return 'START'
        
    def get_response(self):
        response = self._serial_port.read(3)
        if response:
            status, data, cksum = struct.unpack('<BBB', response)
            bin = struct.pack('<BB',status,data)
            # check cksum
            self.assertEqual(cksum, crc8_func(bin))
            return status, data
        else:
            logging.error("response time out")

    def send_packet(self, command, lpos=0, rpos=0, id=0):
        bin = struct.pack('<BHHB', command, lpos, rpos, id)
        bin = struct.pack('<BHHBB',command, lpos, rpos, id, crc8_func(bin))
        self._serial_port.write(bin)

    def test_good_cksum(self):
        self._serial_port.flushInput()
        for i in range(1,100):
            self.send_packet(START, i, i, 0)
            status, data = self.get_response()
            self.assertNotEqual(status, BAD_CKSUM)

    def test_bad_cksum(self):
        self._serial_port.flushInput()
        for i in range(1,100):
            bin = struct.pack('<BHHB',START, i,i,i)
            bin = struct.pack('<BHHBB',START, i+1,i+1,i,crc8_func(bin))
            self._serial_port.write(bin)
            status, data = self.get_response()
            self.assertEqual(status, BAD_CKSUM)

    def test_buffer_underrun(self):
        self._serial_port.flushInput()
        self.send_packet(FLUSH)
        status, data = self.get_response()
        self.send_packet(START)
        status, data = self.get_response()
        self.assertEqual(status, BUFFER_EMPTY)

        # run at less than frequency
        for i in range(1, buflen / 2):
            self.send_packet(LOAD, i, i, i)
            status, data = self.get_response()
            time.sleep(20 * (1 / freq))

        self.send_packet(STOP)
        status, data = self.get_response()
        self.assertEqual(status, BUFFER_EMPTY)

    def test_buffer_overrun(self):
        self._serial_port.flushInput()
        self.send_packet(FLUSH)
        status, data = self.get_response()
        self.send_packet(STOP)
        status, data = self.get_response()
        self.assertEqual(status, BUFFER_EMPTY)

        for i in range(1, buflen + 1):
            self.send_packet(LOAD, i, i, i)
            status, data = self.get_response()
            time.sleep(0.1 * (1 / freq))

        self.assertEqual(status, BUFFER_FULL)

        
    def test_missing_data(self):
        self._serial_port.flushInput()
        self.send_packet(STOP)
        status, data = self.get_response()
        self.send_packet(FLUSH)
        status, data = self.get_response()
        self.assertEqual(status, BUFFER_EMPTY)

        for i in range(1, buflen / 2):
            self.send_packet(LOAD, i, i, i)
            status, data = self.get_response()

        self.send_packet(LOAD)
        status, data = self.get_response()

        self.assertEqual(status, MISSING_DATA)
        self.assertEqual(data, buflen / 2 - 1)

    def test_keep_buffer_full(self):
        self._serial_port.flushInput()
        self.send_packet(STOP)
        status, data = self.get_response()
        self.send_packet(FLUSH)
        status, data = self.get_response()
        self.assertEqual(status, BUFFER_EMPTY)

        i = 1
        while i < 1000:
            if i == buflen / 2:
                self.send_packet(START)
                status, data = self.get_response()
            self.send_packet(LOAD, i, i, i % 256)
            status, data = self.get_response()

            if status == BUFFER_OK:
                pass
            elif status == BUFFER_HIGH:
                time.sleep(buflen / 2 * (1 / freq))
            else:
                self.fail("packet %d unexpected status: %s [%s]" % (i, self.status_str(status), data))

            i += 1

    def xtest_run_robot(self):
        self._serial_port.timeout=0.001
        self._serial_port.flushInput()

        with open('points.d') as fh:
            data = pickle.load(fh)

        logging.info("file is %d points long" % len(data['i']))
        
        i = 1
        while i < len(data['i']):
            logging.info("writing %d" % i)
            a = data['i'][i]['a']
            b = data['i'][i]['b']
            bindata = struct.pack('<HHBB',int(a*10),int(b*10),i % 256,0)
            self._serial_port.write(bindata)
            response = self._serial_port.read(1);
            if response:
                status, = struct.unpack('<B', response)

                # overrun
                if status == 2:
                    response = self._serial_port.read(1);
                    id, = struct.unpack('<B', response)
                    logging.info("overrun last id = %d, sleeping...." % id)
                    time.sleep((buflen / 2) * (1 / freq))
                    i = id + 1
                # missing data
                elif status == 4:
                    response = self._serial_port.read(1);
                    id, = struct.unpack('<B', response)
                    logging.info("missing data, last id = %d" % id)
                    i = id + 1
                # underrun
                elif status == 1:
                    logging.info("underrun")
                # buffer low
                elif status == 5:
                    logging.info("buffer low")
                # buffer high
                elif status == 6:
                    logging.info("buffer high, sleeping")
                    time.sleep(0.1)
                else:
                    logging.debug("unexpected status: %s" % status)

            i += 1

    
if __name__ == '__main__':
    unittest.main()

