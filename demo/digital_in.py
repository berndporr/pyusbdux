#!/usr/bin/python3

import pyusbdux as dux

result = dux.open()
if not result == 0:
    exit()

# as fast as possible just for testing
while True:
    print(dux.digital_in(0))
