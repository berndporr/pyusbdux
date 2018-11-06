==========================
Python API for the USB-DUX
==========================

USB-DUX is a collection of data acquisition boxes designed for Linux

http://www.linux-usb-daq.co.uk


Installation instructions
=========================

For Linux install::

      apt install libcomedi0
      apt install libcomedi-dev
      pip3 install pyusbdux



Quick start guide
=================

Here are the basic steps how to use the API::

      # load the module
      import pyusbdux as dux

      # open comedi
      dux.open()

      # Start data acquisition in the background: one channel, fs=250Hz
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
      dux.close()


API documentation
==================

The commands focus on the asynchronous acquisition::

      # opens the comedi device with comediDeviceNumber
      # returns 0 on success
      open(comediDeviceNumber)
      open() # opens 1st comedi device

      # Starts acquisition of n_channels at the sampling rate of fs.
      # Returns 0 for success and an error code if not successful.
      start(n_channels, fs)       # on the 1st comedi device
      start(n_channels)           # on the 1st comedi device at fs=250

      # Checks if samples are available (=1) or zero if not.
      hasSampleAvilabale();

      # Returns one sample from all channels.
      # returns always 16 values irrespective of how many channels
      # are measured. Blocking call if no samples are available.
      getSampleFromBuffer()

      # stops the background acquisition
      stop()

      # closes the comedi device
      close()
