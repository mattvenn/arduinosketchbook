import unittest
import serial
import struct
import time
import crcmod
crc8_func = crcmod.predefined.mkPredefinedCrcFun("crc-8-maxim")


class TestCan(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls._serial_port=serial.Serial()
        cls._serial_port.port='/dev/ttyUSB0'
        cls._serial_port.timeout=2
        cls._serial_port.baudrate=19200
        cls._serial_port.open()

    @classmethod
    def tearDownClass(cls):
        cls._serial_port.close()

    def send_packet(self, amount):
        bin = struct.pack('<B', amount)
        bin = struct.pack('<BB',amount, crc8_func(bin))
        self._serial_port.write(bin)

    def test_send_amounts(self):
        for i in range(100):
            self.send_packet(i)
            time.sleep(0.05)

    def test_send_30(self):
        for i in range(100):
            for i in range(10):
                self.send_packet(30)
                time.sleep(0.05)
            for i in range(10):
                self.send_packet(90)
                time.sleep(0.05)

if __name__ == '__main__':
    unittest.main()
