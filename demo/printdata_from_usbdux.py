#!/usr/bin/python3

import pyusbdux as dux

dux.open()

dux.start(8)

while True:
    sample = dux.getSampleFromBuffer()
    print(sample)
