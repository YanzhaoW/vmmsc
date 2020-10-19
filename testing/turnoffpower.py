#!/usr/bin/python3 -u etc
import serial
import time
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=2)
ser.write(b'OUT0\n')
exit()