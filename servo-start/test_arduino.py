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
START = 7

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
        cls._serial_port.port='/dev/ttyACM1'
        cls._serial_port.timeout=1
        cls._serial_port.baudrate=115200
        cls._serial_port.open()

        time.sleep(2);

    @classmethod
    def tearDownClass(cls):
        cls._serial_port.close()

    def get_response(self):
        response = self._serial_port.read(3)
        status, data, cksum = struct.unpack('<BBB', response)
        data = struct.pack('<BB',status,data)
        # check cksum
        self.assertEqual(cksum, crc8_func(data))
        return status, data

    def send_packet(self, command, lpos, rpos, id):
        data = struct.pack('<BHHB', command, lpos, rpos, id)
        data = struct.pack('<BHHBB',command, lpos, rpos, id, crc8_func(data))
        self._serial_port.write(data)

    def test_good_cksum(self):
        self._serial_port.flushInput()
        for i in range(1,100):
            self.send_packet(START, i, i, 0)
            status, data = self.get_response()
            self.assertNotEqual(status, BAD_CKSUM)

    def test_bad_cksum(self):
        self._serial_port.flushInput()
        for i in range(1,100):
            data = struct.pack('<BHHB',START, i,i,i)
            data = struct.pack('<BHHBB',START, i+1,i+1,i,crc8_func(data))
            self._serial_port.write(data)
            status, data = self.get_response()
            self.assertEqual(status, BAD_CKSUM)

    def test_buffer_underrun(self):
        self._serial_port.flushInput()
        self.send_packet(START, 0, 0, 0)
        status, data = self.get_response()
        self.assertEqual(status, BUFFER_OK)

        # run at less than frequency
        for i in range(1, buflen):
            self.send_packet(LOAD, i, i, i)
            status, data = self.get_response()
            time.sleep(10 * (1 / freq))

        self.assertEqual(status, BUFFER_EMPTY)

    def test_buffer_overrun(self):
        self._serial_port.flushInput()
        self.send_packet(FLUSH, 0, 0, 0)
        status, data = self.get_response()
        self.send_packet(STOP, 0, 0, 0)
        status, data = self.get_response()
        self.assertEqual(status, BUFFER_OK)

        for i in range(1, buflen + 1):
            self.send_packet(LOAD, i, i, i)
            status, data = self.get_response()
            time.sleep(0.1 * (1 / freq))

        self.assertEqual(status, BUFFER_FULL)

        
    def test_missing_data(self):
        self._serial_port.flushInput()
        self.send_packet(FLUSH, 0, 0, 0)
        status, data = self.get_response()

        for i in range(1, buflen / 2):
            self.send_packet(LOAD, i, i, i)
            status, data = self.get_response()

        self.send_packet(LOAD, 0, 0, 0)
        status, data = self.get_response()

        self.assertEqual(status, MISSING_DATA)

    def test_keep_buffer_full(self):
#        self._serial_port.timeout=0.001
        self._serial_port.flushInput()
        self.send_packet(FLUSH, 0, 0, 0)
        status, data = self.get_response()

        self.send_packet(START, 0, 0, 0)
        status, data = self.get_response()
        
        i = 1
        while i < 1000:
#            logging.info("writing %d" % i)
            self.send_packet(LOAD, i, i, i % 256)
            status, data = self.get_response()

            if status == BUFFER_OK:
                pass
            # overrun
            elif status == BUFFER_FULL:
                logging.info("overrun last id = %d, sleeping...." % data)
                time.sleep((buflen / 2) * (1 / freq))
                i = data + 1
            # missing data
            elif status == MISSING_DATA:
                logging.info("missing data, last id = %d" % data)
                i = data + 1
            # underrun
            elif status == BUFFER_EMPTY:
                logging.info("underrun")
            # buffer low
            elif status == BUFFER_LOW:
                logging.info("buffer low")
            # buffer high
            elif status == BUFFER_HIGH:
                logging.info("buffer high, sleeping")
                time.sleep(0.1)
            else:
                logging.debug("unexpected status: %s" % status)

            i += 1

    def test_run_robot(self):
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

