==========================
Python API for the USB-DUX
==========================

USB-DUX is a collection of data acquisition boxes designed for Linux

http://www.linux-usb-daq.co.uk


Installation instructions
=========================

For Linux install::

      sudo apt install libcomedi0
      sudo apt install libcomedi-dev
      pip3 install numpy
      pip3 install pyusbdux



Quick start guide
=================

Here are the basic steps how to use the API::

      # load the module
      import pyusbdux as dux

      # Start data acquisition in the background
      dux.start(1,250)

      # Now we just read data at our convenience in a loop or timer or thread
      # if data is available you can read it from the ringBuffer

      while (not dux.hasSampleAvilabale()):
      	    # do something else or nothing

      # getting a sample (array of all USB-DUX channels)
      sample = dux.getSampleFromBuffer()

      # do something with the sample
      print(sample)

      # rinse and repeat!

      dux.stop()


API documentation
==================

There are only four commands::

      # Starts acquisition of n_channels at the sampling rate of fs
      # on device with comediDeviceNumber.
      # Returns 0 for success and an error code if not successful.
      start(n_channels, fs, comediDeviceNumber)
      start(n_channels, fs)       # on the 1st comedi device
      start(n_channels)           # on the 1st comedi device at fs=250

      # Checks if samples are available. Returns the number of
      samples. Otherwise zero.
      hasSampleAvilabale();

      # Returns one sample from all channels.
      # returns always 16 values irrespective of how many channels
      # are measured.
      getSampleFromBuffer()

      # stops the background acquisition
      stop()
