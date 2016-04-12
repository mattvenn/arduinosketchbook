import unittest
import serial
import time
import logging
import math

logging.basicConfig(level=logging.DEBUG)

class TestArduino(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls._serial_port=serial.Serial()
        cls._serial_port.port='/dev/ttyUSB1'
        cls._serial_port.timeout=5
        cls._serial_port.baudrate=9600
        cls._serial_port.open()
        ack = cls._serial_port.readline()
        print ack
        assert ack == "OK\n"

    @classmethod
    def tearDownClass(cls):
        cls._serial_port.close()

    def test_random(self):
        results = []
        for j in range(500):
            logging.info(j)
            for i in range(10):
                self._serial_port.write("album: %s\n" % i)
                time.sleep(0.01)
            self._serial_port.write("OK\n");
            ack = self._serial_port.readline()
            print(ack)
            assert ack == "got OK\n"
            result = self._serial_port.readline()
            assert result.startswith("choice:")
            choice = int(result.split(':')[1])
            logging.info("choice = %d" % choice)
            results.append(choice)

        import pickle
        with open("data.pk", 'w') as fh:
            pickle.dump(results, fh)


if __name__ == '__main__':
    unittest.main()

