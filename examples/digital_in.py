#!/usr/bin/python3

import pyusbdux as dux

dux.open()

# as fast as possible just for testing
while True:
    print(dux.digital_in(0))
