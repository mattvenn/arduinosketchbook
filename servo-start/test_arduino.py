import unittest
import serial
import time
import logging
import math
import struct

buflen = 16
freq = 50.0

logging.basicConfig(level=logging.DEBUG)

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

    def xtest_buffer_underrun(self):
        self._serial_port.flushInput()

        # run at less than frequency
        for i in range(1, buflen * 2):
            data = struct.pack('<HHBB',10000,20000,i,0)
            self._serial_port.write(data)
            time.sleep(10 * (1 / freq))

        response = self._serial_port.read(1)
        status, = struct.unpack('<B', response)
        self.assertEqual(status, 1)

    def xtest_buffer_overrun(self):
        self._serial_port.flushInput()

        for i in range(1, buflen * 2):
            data = struct.pack('<HHBB',10000,20000,i,0)
            self._serial_port.write(data)
            time.sleep(0.1 * (1 / freq))

        response = self._serial_port.read(2)
        status, id = struct.unpack('<BB', response)
        self.assertEqual(id, 15)
        self.assertEqual(status, 2)

    def xtest_struct_read(self):
        """
        unsigned int lpos;
        unsigned int rpos;
        uint8_t id;
        uint8_t cksum;
        """
        data = struct.pack('<HHBB',10000,20000,10,20)

        self._serial_port.flushInput()
        self._serial_port.write(data)
        response = self._serial_port.readline()
        logging.info(response)
        response = self._serial_port.readline()
        logging.info(response)
        response = self._serial_port.readline()
        logging.info(response)
        response = self._serial_port.readline()
        logging.info(response)
        response = self._serial_port.readline()
        logging.info(response)
        
    def xtest_missing_data(self):
        data = struct.pack('<HHBB',10000,20000,1,0)
        self._serial_port.flushInput()
        self._serial_port.write(data)

        # no response because no error
        response = self._serial_port.read(2)
        self.assertEqual(response, "")

        # send out of sequence data
        data = struct.pack('<HHBB',10000,20000,4,0)
        self._serial_port.write(data)
        response = self._serial_port.read(2)
        status, id = struct.unpack('<BB', response)
        self.assertEqual(status, 4)
        self.assertEqual(id, 1)

    def test_keep_buffer_full(self):
        self._serial_port.timeout=0.001
        self._serial_port.flushInput()
        
        i = 1
        while i < 1000:
            logging.info("writing %d" % i)
            data = struct.pack('<HHBB',i*10,i*10,i % 256,0)
            self._serial_port.write(data)
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

