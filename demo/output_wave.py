#!/usr/bin/python3

import pyusbdux as dux

result = dux.open()
if not result == 0:
    exit()

# as fast as possible just for testing
while True:
    print(dux.digital_out(0,0))
    dux.analogue_out(0,0)
    dux.digital_out(0,1)
    dux.analogue_out(0,256)
