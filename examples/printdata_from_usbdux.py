#!/usr/bin/python3

import pyusbdux as dux

class DataCallback(dux.Callback):

    def hasSample(self,s):
        print("s:",s)

cb = DataCallback()
dux.setCallback(cb)

# open default USBDUX
dux.open()

# starts to acquire 8 analogue channels at a rate of 250Hz
# in the background. They will be stored in the ringbuffer
# till the they are read via getSampleFromBuffer().
dux.start(8)

input()

dux.stop()
