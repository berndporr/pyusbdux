#!/usr/bin/python3

import pyusbdux as dux

dux.open()

# as fast as possible just for testing
while True:
    dux.digital_out(0,0)
    dux.analogue_out(0,0)
    dux.digital_out(0,1)
    dux.analogue_out(0,255)
