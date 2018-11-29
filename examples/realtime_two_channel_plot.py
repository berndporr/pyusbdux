#!/usr/bin/python3
"""
Plots channels zero and 7 of the USB-DUX in two different windows. Requires pyqtgraph.

"""

import threading
from time import sleep
import sys

import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtGui

import numpy as np

import pyusbdux as c

# create a global QT application object
app = QtGui.QApplication(sys.argv)

# signals to all threads in endless loops that we'd like to run these
running = True

class QtPanningPlot:

    def __init__(self,title):
        self.win = pg.GraphicsLayoutWidget()
        self.win.setWindowTitle(title)
        self.plt = self.win.addPlot()
        self.plt.setYRange(-1,1)
        self.plt.setXRange(0,500)
        self.curve = self.plt.plot()
        self.data = []
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(100)
        self.layout = QtGui.QGridLayout()
        self.win.setLayout(self.layout)
        self.win.show()
        
    def update(self):
        self.data=self.data[-500:]
        if self.data:
            self.curve.setData(np.hstack(self.data))

    def addData(self,d):
        self.data.append(d)


def getDataThread(qtPanningPlot1,qtPanningPlot2):
    # endless loop with sleeps not for timing but for multitasking
    while running:
        # loop as fast as we can to empty the kernel buffer
        while c.hasSampleAvailable():
            sample = c.getSampleFromBuffer()
            qtPanningPlot1.addData(sample[0])
            qtPanningPlot2.addData(sample[7])
        # let Python do other stuff and sleep a bit
        sleep(0.1)

# open comedi
c.open()

# info about the board
print("ADC board:",c.get_board_name())

# Let's create two instances of plot windows
qtPanningPlot1 = QtPanningPlot("USB-DUX 1st channel")
qtPanningPlot2 = QtPanningPlot("USB-DUX 2nd channel")

# create a thread which gets the data from the USB-DUX
t = threading.Thread(target=getDataThread,args=(qtPanningPlot1,qtPanningPlot2,))

# start data acquisition
c.start(8,250)

# start the thread getting the data
t.start()

# showing all the windows
app.exec_()

# no more data from the USB-DUX
c.stop()

# Signal the Thread to stop
running = False

# Waiting for the thread to stop
t.join()

c.close()

print("finished")
