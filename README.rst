==========================
Python API for the USB-DUX
==========================

USB-DUX are data acquisition boxes designed for Linux

http://www.linux-usb-daq.co.uk


Installation instructions
=========================

Install first the comedi development libraries (if not already on your system)::

      apt install libcomedi0
      apt install libcomedi-dev

and then install pyusbdux (only available for python3)::
  
      pip3 install pyusbdux



Quick start guide
=================

Here are the basic steps how to use the API::

      # load the module
      import pyusbdux as dux

      # open comedi
      dux.open()

      # Start asynchronous data acquisition in the background: one channel, fs=250Hz
      dux.start(1,250)

      # Now we read data at our convenience in a loop or timer or thread
      # The following lines need to be repeated

      # Let's check if samples are available
      if (dux.hasSampleAvilabale() == 0):
      	    # nope! Do something else or nothing

      # Let's get a sample (array of all USB-DUX channels)
      sample = dux.getSampleFromBuffer()

      # do something with the sample, for example print it
      print(sample)

      # rinse and repeat!
      # end of loop

      # shutdown
      dux.stop()
      dux.close()


API documentation
==================

These are the commands which allow you to access the analogue inputs asynchronously
and the analogue outputs, digital input and outputs synchronously::

      # opens the comedi device with comediDeviceNumber
      # returns 0 on success
      open(comediDeviceNumber)
      open()                      # opens 1st comedi device

      # Starts acquisition of n_channels at the sampling rate of fs.
      # Returns 0 for success and an error code if not successful.
      start(n_channels, fs)
      start(n_channels)           # at fs=250

      # Checks if samples are available (=1) or zero if not.
      hasSampleAvilabale();

      # Returns one sample from all channels.
      # returns always 16 values irrespective of how many channels
      # are measured. Blocking call if no samples are available.
      getSampleFromBuffer()

      # stops the background acquisition
      stop()

      # writes to a digital pin the value 0 or 1
      digital_out(channel, value)

      # reads from a digital pin
      digital_in(channel)

      # writes to an analogue output pin (raw ADC values)
      analoge_out(channel, value)

      # gets the max raw value of the analogue output
      get_analogue_out_max_raw_value()

      # closes the comedi device
      close()

      # returns the name of the board connected
      get_board_name()

Any error in comedi throws an exception in python.

Example / demo programs
=======================

In the folder https://github.com/berndporr/pyusbdux/tree/master/demo are example
scripts which show you how to sample data from the analogue and digital ports.
