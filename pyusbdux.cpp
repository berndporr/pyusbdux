/*
 * pyusbdux
 *
 * Copyright (c) 1999,2000,2001 David A. Schleef <ds@schleef.org>
 *               2019-2020 Bernd Porr <mail@berndporr.me.uk>
 *
 * This file may be freely modified, distributed, and combined with
 * other software, as long as proper attribution is given in the
 * source code.
 */

#include <stdio.h>
#include <comedilib.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "pyusbdux.h"


#define N_CHANS 16
#define BUFSZ N_CHANS*sizeof(long int)


comedi_t *dev = NULL;
int range = 0;
int subdevice = 0;
int aref  = AREF_GROUND;
int n_chan = 0;
double freq = 0;
int bytes_per_sample = 0;
int subdev_flags = 0;

char buffer[BUFSZ];
float samples[N_CHANS];

unsigned int chanlist[N_CHANS];
comedi_range* range_info[N_CHANS];
lsampl_t maxdata[N_CHANS];
comedi_cmd cmd;

static const char errorDevNotOpen[] = "Comedi device not open. Use open() first.";
static const char errorDisconnect[] = "Device error. Possible disconnect.";

void open(int comediDeviceNumber) {
	char filename[256];
	sprintf(filename,"/dev/comedi%d",comediDeviceNumber);

	/* open the device */
	dev = comedi_open(filename);
	if(!dev){
		throw "Could not open comedi device.";
	}
	if (strstr(comedi_get_board_name(dev),"usbdux") == NULL) {
		throw "Not a USBDUX board.";
	}
	memset(&cmd,0,sizeof(comedi_cmd));
}


// autodetect
void open() {
	for(int i = 0;i<16;i++) {
		try {
			open(i);
			return;
		} catch (const char*) {
		}
	}
	throw "No USBDUX board found.";
}


void start(int n_channels, double fs) {
	if (dev == NULL) throw errorDevNotOpen;

	n_chan = n_channels;
	freq = fs;

	for(int i=0;i<N_CHANS;i++) {
		samples[i] = 0;
	}

	subdevice = comedi_find_subdevice_by_type(dev,COMEDI_SUBD_AI,0);

	// Print numbers for clipped inputs
	comedi_set_global_oor_behavior(COMEDI_OOR_NUMBER);

	/* Set up channel list */
	for(int i = 0; i < n_chan; i++){
		chanlist[i] = CR_PACK(i, range, aref);
		range_info[i] = comedi_get_range(dev, subdevice, 0, range);
		maxdata[i] = comedi_get_maxdata(dev, subdevice, 0);
	}

	subdev_flags = comedi_get_subdevice_flags(dev,subdevice);
	if(subdev_flags & SDF_LSAMPL) {
		bytes_per_sample = sizeof(lsampl_t);
	} else {
		bytes_per_sample = sizeof(sampl_t);
	}

	memset(&cmd,0,sizeof(comedi_cmd));
        int ret = comedi_get_cmd_generic_timed(dev, subdevice, &cmd, n_chan, 1e9 / freq);
        if(ret<0){
                throw "Failed to generate async timing information"
			"based on sampling rate and number of channels.";
        }

	cmd.chanlist = chanlist;
        cmd.chanlist_len = n_chan;
	cmd.stop_src = TRIG_NONE;
	cmd.stop_arg = 0;
	cmd.scan_end_arg = n_chan;

	// twice!
	ret = comedi_command_test(dev, &cmd);
	ret = comedi_command_test(dev, &cmd);
	if(ret!=0){
		throw "Error preparing async analogue in. Check sampling rate / number of channels.";
	}

	/* start the command */
	ret = comedi_command(dev, &cmd);
	if(ret < 0){
		throw "Async data acquisition could not be started.";
	}
}


void start(int nChan) {
	start(nChan,250);
}


sample_p getSampleFromBuffer() {
	if (dev == NULL) throw errorDevNotOpen;
	while (comedi_get_buffer_contents(dev,subdevice) < (bytes_per_sample * n_chan) ) {
		usleep(100);
	};
	int ret = read(comedi_fileno(dev),buffer,bytes_per_sample * n_chan);
	if (ret == 0) {
		throw "BUG: No data returned by read() but there is data in the kernel buffer!";
	}
	if(ret < 0){
		throw errorDisconnect;
	} else if (ret > 0) {
		int subdev_flags = comedi_get_subdevice_flags(dev,subdevice);
		for(int i = 0; i < n_chan; i++) {
			int raw;
			if(subdev_flags & SDF_LSAMPL) {
				raw = ((lsampl_t *)buffer)[i];
			} else {
				raw = ((sampl_t *)buffer)[i];
			}
			samples[i] = comedi_to_phys(raw,
						    range_info[i],
						    maxdata[i]);
		}
	}
	return samples;
}


int hasSampleAvailable() {
	if (dev == NULL) throw errorDevNotOpen;
	int ret = comedi_get_buffer_contents(dev,subdevice);
	if (ret < 0) throw errorDisconnect;
	return ret > 0;
}

float getSamplingRate() {
	float sampling_rate = 0;
        // the timing is done channel by channel
        // this means that the actual sampling rate is divided by
        // number of channels
        if ((cmd.convert_src ==  TRIG_TIMER)&&(cmd.convert_arg)) {
                sampling_rate=((1E9 / cmd.convert_arg)/n_chan);
        }
        
        // the timing is done scan by scan (all channels at once)
        // the sampling rate is equivalent of the scan_begin_arg
        if ((cmd.scan_begin_src ==  TRIG_TIMER)&&(cmd.scan_begin_arg)) {
                sampling_rate=1E9 / cmd.scan_begin_arg;
        }
	return sampling_rate;
}

void stop() {
	if (dev == NULL) throw errorDevNotOpen;
	comedi_cancel(dev,subdevice);
	memset(&cmd,0,sizeof(comedi_cmd));
}



void digital_out(int channel, int value) {
	if (dev == NULL) throw errorDevNotOpen;
	int subdevice = 2;
        int ret = comedi_dio_config(dev,subdevice,channel,COMEDI_OUTPUT);
        if(ret < 0){
		throw errorDisconnect;
        }
        ret = comedi_dio_write(dev,subdevice,channel,value);
	if (ret < 0) {
		throw errorDisconnect;
	}
}



int digital_in(int channel) {
	if (dev == NULL) throw errorDevNotOpen;
	int subdevice = 2;
        int ret = comedi_dio_config(dev,subdevice,channel,COMEDI_INPUT);
        if (ret < 0) throw errorDisconnect;
	unsigned int value;
        ret = comedi_dio_read(dev,subdevice,channel,&value);
	if (ret < 0) throw errorDisconnect;
        return (int)value;
}



void analogue_out(int channel, int value) {
	if (dev == NULL) throw errorDevNotOpen;
	int subdevice = comedi_find_subdevice_by_type(dev,COMEDI_SUBD_AO,0);
	int ret = comedi_data_write(dev,subdevice,channel,0,0, value);
	if (ret<0) throw errorDisconnect;
}


int get_analogue_out_max_raw_value() {
	if (dev == NULL) throw errorDevNotOpen;
	int subdevice = comedi_find_subdevice_by_type(dev,COMEDI_SUBD_AO,0);
	int ret = comedi_get_maxdata(dev, subdevice, 0);
	if (ret < 0) throw errorDisconnect;
	return ret;
}


const char* get_board_name() {
	if (dev == NULL) throw errorDevNotOpen;
	return comedi_get_board_name(dev);
}


void close() {
	if (dev == NULL) throw errorDevNotOpen;
	comedi_close(dev);
	dev = NULL;
	memset(&cmd,0,sizeof(comedi_cmd));
}
