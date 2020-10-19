#!/usr/bin/python
import pyvisa as visa
import time
resources = visa.ResourceManager('@py')
print(resources.list_resources())
siggen = resources.open_resource('USB0::1689::835::C021191::0::INSTR')
#print(siggen.query('*IDN?'))
#print(siggen.write('*RST'))
siggen.read_termination = '\n'
siggen.write_termination = '\n'
print(siggen.write('TRIG:SEQ:SOUR EXT'))
time.sleep(0.05)
print(siggen.write('SOUR2:FUNC:SHAPE SQUARE'))
time.sleep(0.05)
print(siggen.write('SOUR2:BURS:STATE ON'))
time.sleep(0.05)
print(siggen.write('SOUR2:BURS:NCYC 1'))
time.sleep(0.05)
print(siggen.write('SOUR2:VOLT:LEV:IMM:AMPL 1.8'))
time.sleep(0.05)
print(siggen.write('SOUR2:VOLT:IMM:OFFS 0.9'))
time.sleep(0.05)
print(siggen.write('OUTP2:STAT ON'))
time.sleep(0.05)
siggen.write('TRIG:IMM')
print(siggen.write('OUTP2:STAT OFF'))
#print(siggen.write('SYST:BEEP:IMM'))
