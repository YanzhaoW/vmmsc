#!/usr/bin/python3 -u etc
import serial
import serial.tools.list_ports
import sys
from datetime import datetime
import time
import atexit
import signal
import struct
#Function that is called when program is terminated (Turns off the power supply)
def termination():
    ser.write(b'OUTP:GEN 0\n') #Change to corresponding turn off command for your power supply

warn19 = sys.argv[1]
warn29 = sys.argv[2]
ports = serial.tools.list_ports.comports()
i=0
relport = 0
for p in ports:
	if(p.vid == 1027 and p.pid == 24577):#Change numbers to corresponding vendor and product IDs
		relport = i
	i+=1
ser = serial.Serial(ports[relport], 9600, timeout=2)
#Get the identification
ser.write(b'*IDN?\n')
time.sleep(0.1)
idn = ser.readline()
print(str(idn,'utf-8'))
#Code Block to setup the outputs of the powersupply
time.sleep(0.1)
ser.write(b'INST:SEL OUTP1\n')
time.sleep(0.1)
ser.write(b'VOLT 1.9\n')
time.sleep(0.1)
ser.write(b'SOUR:CURR:LEV:IMM:AMPL 2.3\n')
time.sleep(0.1)
ser.write(b'OUTP:SEL 1\n')
time.sleep(0.1)
ser.write(b'INST:SEL OUTP2\n')
time.sleep(0.1)
ser.write(b'VOLT 2.9\n')
time.sleep(0.1)
ser.write(b'SOUR:CURR:LEV:IMM:AMPL 0.3\n')
time.sleep(0.1)
ser.write(b'OUTP:SEL 1\n')
time.sleep(0.1)
ser.write(b'INST:SEL OUTP3\n')
time.sleep(0.1)
ser.write(b'VOLT 5\n')
time.sleep(0.1)
ser.write(b'SOUR:CURR:LEV:IMM:AMPL 2\n')
time.sleep(0.1)
ser.write(b'OUTP:SEL 1\n')
time.sleep(0.1)
ser.write(b'OUTP:GEN 1\n')
time.sleep(0.1)
resp = ser.readline()
print(str(resp,'utf-8'))
#Setup the file in which currents will be written, as well as the termination signalling. DO NOT CHANGE.
f =open("currents.dat","a")
t_start = datetime.utcnow()
atexit.register(termination)
signal.signal(signal.SIGTERM, termination)
while(True):
	#Codeblock to readout currents of channel 1
	ser.write(b'INST OUT1\n')
	time.sleep(0.01)
	ser.write(b'MEAS:CURR?\n')
	time.sleep(0.01)
	i1=ser.readline()
	i1=i1.strip(b'A\r\n')
	#i1 is the measured current.
	if(float(i1)>float(warn19)):#Checks the warning currents (for detecting short-circuits)
		ser.write(b'OUTP:GEN 0\n')
		exit(42)
	#Similar Code to read the currents of channel2
	ser.write(b'INST OUT2\n')
	time.sleep(0.01)
	ser.write(b'MEAS:CURR?\n')
	time.sleep(0.01)
	i2=ser.readline()
	i2=i2.strip(b'A\r\n')
	#i2 is the second measured current
	if(float(i2)>float(warn29)):
		ser.write(b'OUTP:GEN 0\n')
		exit(42)
	#Data is written to the file
	out = datetime.utcnow().strftime("%Y%m%d%H%M%S.%f")[:-3] + '\t' + str(i1,'utf-8') + '\t' + str(i2,'utf-8') 
	f.write(out)
	f.write("\n")
	print(out)
