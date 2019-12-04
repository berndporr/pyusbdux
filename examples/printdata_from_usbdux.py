#!/usr/bin/python3

import pyusbdux as dux

# open default USBDUX
dux.open()

# starts to acquire 8 analogue channels at a rate of 250Hz
# in the background. They will be stored in the ringbuffer
# till the they are read via getSampleFromBuffer().
dux.start(8)

while True:
    # Note that if called without checking if samples
    # are actually available it will block until samples
    # are available. For realtime tasks first check if
    # samples are available.
    sample = dux.getSampleFromBuffer()
    print(sample)
