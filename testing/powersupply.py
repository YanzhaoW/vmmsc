#!/usr/bin/python3 -u etc
import serial
import serial.tools.list_ports
import sys
from datetime import datetime
import time
import atexit
import signal
import struct
def termination():
    ser.write(b'OUT0\n') #Change this to the corresponding "Turn off power output" command for your powersupply

warn19 = sys.argv[1]
warn29 = sys.argv[2]
ports = serial.tools.list_ports.comports()
i=0
relport = 0
for p in ports:
    if(p.vid == 1027 and p.pid == 24577):#Change here the corresponding product and vendor id for the powersupply
        relport = i
    i +=1
ser = serial.Serial(ports[relport].device, 9600, timeout=2)
#Change this codeblock in a way, that the voltages and currents are set properly for your powersupply
ser.write(b'VSET1:1.9\n') 
ser.write(b'VSET2:2.9\n')
ser.write(b'ISET2:1.0\n')
ser.write(b'ISET1:2.3\n')
ser.write(b'OUT1\n')
#This needs to stay as it is. It opens the current file, where data is written into and later also readout by the testing software
f =open("currents.dat","a")
t_start = datetime.utcnow()
atexit.register(termination)
signal.signal(signal.SIGTERM, termination)
#Until here no change is needed
while(True):
    #Inbetween here readout current of channel 1. Commands may be more complex
    ser.write(b'IOUT1?\n')
    #o here
    i1=ser.readline()
    i1=i1.strip(b'A\r\n')
    if(float(i1)>float(warn19)):
        ser.write(b'OUT0\n')#Needs to be changed to "Turn off power output" command
        exit(42)
    #Here readout current of channel2
    ser.write(b'IOUT2?\n')
    #to here
    i2=ser.readline()
    i2=i2.strip(b'A\r\n')
    if(float(i2)>float(warn29)): 
        ser.write(b'OUT0\n')#Needs to be changed to "Turn off power output" command
        exit(42)
    out = datetime.utcnow().strftime("%Y%m%d%H%M%S.%f")[:-3] + '\t' + str(i1,'utf-8') + '\t' + str(i2,'utf-8') 
    f.write(out)
    f.write("\n")
    print(out)
    time.sleep(0.2)