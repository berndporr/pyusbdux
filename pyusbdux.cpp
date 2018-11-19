/*
 * pyusbdux
 *
 * Copyright (c) 1999,2000,2001 David A. Schleef <ds@schleef.org>
 *               2018 Bernd Porr <mail@berndporr.me.uk>
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
int n_chan;
double freq;
int bytes_per_sample;
int subdev_flags;

char buffer[BUFSZ];
float samples[N_CHANS];

unsigned int chanlist[N_CHANS];
comedi_range * range_info[N_CHANS];
lsampl_t maxdata[N_CHANS];


const char *cmdtest_messages[]={
	"success",
	"invalid source",
	"source conflict",
	"invalid argument",
	"argument conflict",
	"invalid chanlist",
};


int open(int comediDeviceNumber) {
	char filename[256];
	sprintf(filename,"/dev/comedi%d",comediDeviceNumber);

	/* open the device */
	dev = comedi_open(filename);
	if(!dev){
		comedi_perror(filename);
		return errno;
	}
	return 0;
}


int open() {
	return open(0);
}


int start(int n_channels, double fs, int comediDeviceNumber) {
	n_chan = n_channels;
	freq = fs;

	if (dev == NULL) return -ENODEV;

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

	comedi_cmd cmd;
	memset(&cmd,0,sizeof(comedi_cmd));
        int ret = comedi_get_cmd_generic_timed(dev, subdevice, &cmd, n_chan, 1e9 / freq);
        if(ret<0){
                printf("comedi_get_cmd_generic_timed failed\n");
                return ret;
        }

	cmd.chanlist = chanlist;
        cmd.chanlist_len = n_chan;
	cmd.stop_src = TRIG_NONE;
	cmd.stop_arg = 0;
	cmd.scan_end_arg = n_chan;

	/* comedi_command_test() tests a command to see if the
	 * trigger sources and arguments are valid for the subdevice.
	 * If a trigger source is invalid, it will be logically ANDed
	 * with valid values (trigger sources are actually bitmasks),
	 * which may or may not result in a valid trigger source.
	 * If an argument is invalid, it will be adjusted to the
	 * nearest valid value.  In this way, for many commands, you
	 * can test it multiple times until it passes.  Typically,
	 * if you can't get a valid command in two tests, the original
	 * command wasn't specified very well. */
	ret = comedi_command_test(dev, &cmd);
	if(ret < 0){
		comedi_perror("comedi_command_test");
		if(errno == EIO){
			fprintf(stderr,"Ummm... this subdevice doesn't support commands\n");
		}
		return errno;
	}
	ret = comedi_command_test(dev, &cmd);
	if(ret < 0){
		comedi_perror("comedi_command_test");
		return ret;
	}
	fprintf(stderr,"second test returned %d (%s)\n", ret,
			cmdtest_messages[ret]);
	if(ret!=0){
		fprintf(stderr, "Error preparing command\n");
		return ret;
	}

	/* start the command */
	ret = comedi_command(dev, &cmd);
	if(ret < 0){
		comedi_perror("comedi_command");
		return ret;
	}
	return 0;
}


int start(int nChan) {
	return start(nChan,250,0);
}


int start(int nChan, double fs) {
	return start(nChan,fs,0);
}


sample_p getSampleFromBuffer() {
	while (!comedi_get_buffer_contents(dev,subdevice)) {
		usleep(1000);
	};
	int ret = read(comedi_fileno(dev),buffer,bytes_per_sample * n_chan);
	if(ret < 0){
		perror("reading sample");
	}else if (ret > 0){
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


int hasSampleAvilabale() {
	if (dev == NULL) return 0;
	return comedi_get_buffer_contents(dev,subdevice) > 0;
}


void stop() {
	if (dev == NULL) return;
	comedi_cancel(dev,subdevice);
}



int digital_out(int channel, int value) {
	int subdevice = 2;
        int ret = comedi_dio_config(dev,subdevice,channel,COMEDI_OUTPUT);
        if(ret < 0){
                return ret;
        }
        ret = comedi_dio_write(dev,subdevice,channel,value);
        return ret;
}



int digital_in(int channel) {
	int subdevice = 2;
        int ret = comedi_dio_config(dev,subdevice,channel,COMEDI_INPUT);
        if(ret < 0){
                return ret;
        }
	unsigned int value;
        ret = comedi_dio_read(dev,subdevice,channel,&value);
	if (ret<0) return ret;
        return value;
}



int analogue_out(int channel, int value) {
	int subdevice = comedi_find_subdevice_by_type(dev,COMEDI_SUBD_AO,0);
	int ret = comedi_data_write(dev,subdevice,channel,0,0, value);
        return ret;
}




void close() {
	comedi_close(dev);
	dev = NULL;
}
