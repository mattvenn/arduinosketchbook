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
BAD_CMD = 7

#commands
START = 8
STOP = 9
LOAD = 10 
FLUSH = 11 
STATUS = 12 
SET_POS = 13
LOAD_P = 14
LOAD_I = 15
LOAD_D = 16

buflen = 32
freq = 50.0

logging.basicConfig(level=logging.DEBUG)
crc8_func = crcmod.predefined.mkPredefinedCrcFun("crc-8-maxim")

class TestBuffer(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls._serial_port=serial.Serial()
        cls._serial_port.port='/dev/ttyUSB1'
        cls._serial_port.timeout=2
        cls._serial_port.baudrate=115200
        cls._serial_port.open()
        cls._serial_port.setDTR(True)

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

    def send_packet(self, command, lpos=0, rpos=0, can=0, id=0):
        id = id % 256
        bin = struct.pack('<BHHBB', command, lpos, rpos, can, id)
        bin = struct.pack('<BHHBBB',command, lpos, rpos, can, id, crc8_func(bin))

        self.send_rs485_data(bin)

    def send_rs485_data(self, bin):
        self._serial_port.setDTR(False)
        time.sleep(0.001)
        self._serial_port.write(bin)
        time.sleep(0.001)
#        time.sleep(0.001)
        self._serial_port.setDTR(True)

    def send_rs232_data(bin):
        self._serial_port.write(bin)

    def test_send_flush(self):
        self.send_packet(FLUSH)
        status, data = self.get_response()
        self.assertEqual(status, BUFFER_EMPTY)

    def test_send_PID(self):
        """
        float kp = .45;
        float ki = 0.000;
        float kd = .25;
        """
        self.send_packet(LOAD_P, 450)
        status, data = self.get_response()
        self.assertEqual(status, LOAD_P)

        self.send_packet(LOAD_I, 0)
        status, data = self.get_response()
        self.assertEqual(status, LOAD_I)

        self.send_packet(LOAD_D, 250)
        status, data = self.get_response()
        self.assertEqual(status, LOAD_D)

    def test_set_pos(self):
        self.send_packet(SET_POS,1150,1150)
        status, data = self.get_response()
        self.assertEqual(status, SET_POS)


    def test_good_cksum(self):
        self._serial_port.flushInput()
        self.send_packet(STOP)
        status, data = self.get_response()
        self.assertEqual(status, STOP)
        self.send_packet(FLUSH)
        status, data = self.get_response()
        for i in range(1,500):
            logging.debug(i)
            self.send_packet(LOAD, i, i, 0, i)
            status, data = self.get_response()
            self.assertNotEqual(status, BAD_CKSUM)

    def test_bad_cksum(self):
        self._serial_port.flushInput()
        self.send_packet(STOP)
        status, data = self.get_response()
        self.send_packet(FLUSH)
        status, data = self.get_response()
        for i in range(1,100):
            logging.debug(i)
            bin = struct.pack('<BHHBB',START, i,i,i,0xFF)
            self.send_rs485_data(bin)
            status, data = self.get_response()
            self.assertEqual(status, BAD_CKSUM)

    def test_buffer_underrun(self):
        self._serial_port.flushInput()
        self.send_packet(FLUSH)
        status, data = self.get_response()
        self.assertEqual(status, BUFFER_EMPTY)
        self.send_packet(START)
        status, data = self.get_response()
        self.assertEqual(status, START)

        # run at less than frequency
        for i in range(1, buflen / 2):
            logging.debug(i)
            self.send_packet(LOAD, i, i, 0, i)
            status, data = self.get_response()
            time.sleep(20 * (1 / freq))

        self.send_packet(STATUS)
        status, data = self.get_response()
        self.assertEqual(status, BUFFER_EMPTY)

    def test_buffer_overrun(self):
        self._serial_port.flushInput()
        self.send_packet(FLUSH)
        status, data = self.get_response()
        self.assertEqual(status, BUFFER_EMPTY)
        self.send_packet(STOP)
        status, data = self.get_response()
        self.assertEqual(status, STOP)

        for i in range(1, buflen + 1):
            logging.debug(i)
            self.send_packet(LOAD, i, i, 0, i)
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
            logging.debug(i)
            self.send_packet(LOAD, i, i, 0, i)
            status, data = self.get_response()

        self.send_packet(LOAD)
        status, data = self.get_response()

        self.assertEqual(status, MISSING_DATA)
        self.assertEqual(data, buflen / 2 - 1)

    def test_accuracy(self, num=2, amount=500):
        self._serial_port.flushInput()
        self.send_packet(STOP)
        status, data = self.get_response()
        self.assertEqual(status, STOP)
        self.send_packet(FLUSH)
        status, data = self.get_response()
        self.assertEqual(status, BUFFER_EMPTY)

        self.send_packet(SET_POS,0,0)
        status, data = self.get_response()
        self.assertEqual(status, SET_POS)

        self.send_packet(START)
        status, data = self.get_response()
        self.assertEqual(status, START)

        i = 1
        while i < num * 2:
            logging.debug(i)
            self.send_packet(LOAD, amount, amount, 0, i)
            status, data = self.get_response()
            logging.debug(self.status_str(status))
            #self.assertEqual(status, BUFFER_LOW)

            time.sleep(3)
            i += 1

            logging.debug(i)
            self.send_packet(LOAD, 0, 0, 0, i)
            status, data = self.get_response()
            logging.debug(self.status_str(status))
            #self.assertEqual(status, BUFFER_LOW)

            i += 1
            time.sleep(3)

    def test_single_load(self, amount=1000):
        self._serial_port.flushInput()
        self.send_packet(STOP)
        status, data = self.get_response()
        self.assertEqual(status, STOP)
        self.send_packet(FLUSH)
        status, data = self.get_response()
        self.assertEqual(status, BUFFER_EMPTY)

        self.send_packet(START)
        status, data = self.get_response()
        self.assertEqual(status, START)

        self.send_packet(LOAD, amount, amount, 30, 1)
        status, data = self.get_response()
        self.assertEqual(status, BUFFER_LOW)

    def test_keep_buffer_full(self, num=500):
        self._serial_port.flushInput()
        self.send_packet(STOP)
        status, data = self.get_response()
        self.assertEqual(status, STOP)
        self.send_packet(FLUSH)
        status, data = self.get_response()
        self.assertEqual(status, BUFFER_EMPTY)

        for i in range(1, num):
            logging.debug(i)
            if i == buflen / 2:
                logging.debug("starting servo")
                self.send_packet(START)
                status, data = self.get_response()
                self.assertEqual(status, START)

            can = i % 70
            self.send_packet(LOAD, i, i, can, i)
            status, data = self.get_response()

            if status == BUFFER_OK:
                pass
            elif status == BUFFER_LOW:
                pass
            elif status == BUFFER_HIGH:
                logging.debug("buffer high, waiting...")
                time.sleep(buflen / 4 * (1 / freq))
            else:
                self.fail("packet %d unexpected status: %s [%s]" % (i, self.status_str(status), data))


    def test_run_robot(self):
        with open('points.d') as fh:
            points = pickle.load(fh)
        logging.debug("file is %d points long" % len(points['i']))

        self._serial_port.flushInput()
        self.send_packet(STOP)
        status, data = self.get_response()
        self.send_packet(FLUSH)
        status, data = self.get_response()
        self.assertEqual(status, BUFFER_EMPTY)
        
        i = 1
        while i < len(points['i']):
            if i == buflen / 2:
                self.send_packet(START)
                status, data = self.get_response()

            a = points['i'][i]['a']
            b = points['i'][i]['b']
            can = points['i'][i]['can']
            if can == 0:
                can = 30
            if can == 1:
                can = 90
            logging.debug("writing %d (%d,%d can %d)" % (i,a,b,can))
            self.send_packet(LOAD, a, b, can, i)
            status, data = self.get_response()
            logging.debug(self.status_str(status))

            if status == BUFFER_OK:
                pass
            elif status == BUFFER_LOW:
                pass
            elif status == BUFFER_HIGH:
                time.sleep(buflen / 2 * (1 / freq))
            else:
                self.fail("packet %d unexpected status: %s [%s]" % (i, self.status_str(status), data))

            i += 1

    """
    timing info

    * messages sent every 20ms (50Hz).
    * software serial has stop & start bits, plus 8 bits for the data.
    * slave messages are 3 bytes (so 30 bits with software serial)
        * 57600 msg takes 0.5ms
        * 19200 msg takes 1.6ms
        * 9600 msg takes 3.1ms
        * 2400 msg takes 12ms



    """

        

    @unittest.skip("skipping")
    def test_read_slave_nums(self):
        slave_port=serial.Serial()
        slave_port.port='/dev/ttyUSB0'
        slave_port.timeout=1
        slave_port.baudrate=115200
        slave_port.open()

        slave_port.write('a')
        ok = int(slave_port.readline())
        bad_cksum = int(slave_port.readline())
        logging.debug("bad cksum = %d, ok = %d" % (bad_cksum, ok))

    def test_slave_direct(self):
        slave_port=serial.Serial()
        slave_port.port='/dev/ttyUSB0'
        slave_port.timeout=1
        slave_port.baudrate=57600
        slave_port.open()
        
    # @unittest.skip("skipping")
    def test_slave_comms(self):
        slave_port=serial.Serial()
        slave_port.port='/dev/ttyUSB0'
        slave_port.timeout=1
        slave_port.baudrate=115200
        slave_port.open()

        # this doesn't always work, don't know why
        import ipdb; ipdb.set_trace()
        slave_port.write('b') # clear sums
        slave_port.write('a')
        ok = int(slave_port.readline())
        bad_cksum = int(slave_port.readline())
        self.assertEqual(ok, 0)
        self.assertEqual(bad_cksum, 0)

        # run tests on buffer
        num = 1000
        self.test_keep_buffer_full(num)

        # wait for buffer to empty
        time.sleep(1.5 * buflen * (1 / freq))

        self.send_packet(STATUS)
        status, data = self.get_response()
        self.assertEqual(status, BUFFER_EMPTY)

        slave_port.write('a')
        ok = int(slave_port.readline())
        bad_cksum = int(slave_port.readline())
        logging.debug("bad cksum = %d, ok = %d" % (bad_cksum, ok))
        self.assertEqual(ok, num-1)  # starts at 1
        self.assertEqual(bad_cksum, 0)
    
if __name__ == '__main__':
    unittest.main()
    exit(0)

    # just run the one test
    slave = unittest.TestSuite()
    slave.addTest(TestBuffer('test_slave_comms')) 

    log_file = 'log_file.txt'
    with open(log_file, "a") as fh:
        runner = unittest.TextTestRunner(fh)
        runner.run(slave)
