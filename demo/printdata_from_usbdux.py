#!/usr/bin/python3

import pyusbdux as dux

result = dux.start(2)
if result > 0:
    exit()

while True:
    while (not dux.hasSampleAvilabale()):
        a = 1
    sample = dux.getSampleFromBuffer()
    print(sample)