#!/usr/bin/python3

import pyusbdux as dux

result = dux.open()
if not result == 0:
    exit()

result = dux.start(8)
if not result == 0:
    exit()

while True:
    while (not dux.hasSampleAvilabale()):
        pass
    sample = dux.getSampleFromBuffer()
    print(sample)
