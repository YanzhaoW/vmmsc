#!/usr/bin/python
import os
import subprocess
import re
import sys
import socket

#please set the present IP of the assister
UDP_IP = "192.168.50.100"
#DO NOT CHANGE THE PORT
UDP_PORT = 6023
#UDP_PORT = 9999


###############################
#Values to be written to EEPROM
###############################
macID = bytearray([0x00, 0x05, 0x04, 0x0a])
assisterIP = bytearray([0xc0, 0xa8, 0x32, 0x64])
daqIP = bytearray([0xc0, 0xa8, 0x32, 0x01])
###############################

   
#Vendor ID for XILINX devices  
macVendor = bytearray([0x00, 0x00, 0x0a, 0x35])
#DO NOT CHANGE THESE VALUES  
counter = bytearray([0x80, 0x00, 0x00, 0x00])
subaddress = bytearray([0xff, 0xff, 0xff, 0xff])
command = bytearray([0xaa, 0xaa, 0xff, 0xff])
filler = bytearray([0x00, 0x00, 0x00, 0x00])

addressMacVendor = bytearray([0xa8, 0xf2, 0x00, 0x7a])
addressMacID = bytearray([0xa8, 0xf2, 0x00, 0x7d])
addressAssisterIP = bytearray([0xa8, 0xf3, 0x00, 0x00])
addressDaqIP = bytearray([0xa8, 0xf3, 0x00, 0x12])

data = counter + subaddress + command + filler + addressMacVendor + macVendor + addressMacID + macID + addressAssisterIP + assisterIP + addressDaqIP + daqIP

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(('', 6007))
s.sendto(data, (UDP_IP, UDP_PORT))
