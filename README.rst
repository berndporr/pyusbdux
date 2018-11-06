==========================
Python API for the USB-DUX
==========================

USB-DUX are Linux DAQ boxes

http://www.linux-usb-daq


Installation instructions
=========================

For Ubuntu Linux install  attyscomm-dev::

      sudo apt install libcomedi0
      sudo apt install libcomedi-dev
      pip3 install pyusbdux



Quick start guide
=================

Here are the basic steps how to use the API::

      # load the module
      import pyusbdux as dux

      # Start data acquisition in the background
      dux.start()

      # Now we just read data at our convenience in a loop or timer or thread
      # if data is available you can read it from the ringBuffer

      while (not dux.hasSampleAvilabale()):
      	    # do something else or nothing

      # getting a sample (array of all Attys channels)
      sample = dux.getSampleFromBuffer()

      # do something with the sample
      print(sample)

      # rinse and repeat!


API documentation
==================

There are only four commands::

      # Starts acquisition of n_channels at the sampling rate of fs
      # Returns 0 for success and an error code if not successful
      start(int n_channels, double fs);

      # Checks if samples are available. Returns 1 if there are samples
      # Otherwise zero.
      hasSampleAvilabale();

      # Returns one sample from all channels.
      # returns always 16 values irrespective of how many channels
      # are measured.
      getSampleFromBuffer()

      # stops the background acquisition
      stop()
