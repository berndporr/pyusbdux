/*
 * Example of using commands - asynchronous input
 * Part of Comedilib
 *
 * Copyright (c) 1999,2000,2001 David A. Schleef <ds@schleef.org>
 *               2018 Bernd Porr <mail@berndporr.me.uk>
 *
 * This file may be freely modified, distributed, and combined with
 * other software, as long as proper attribution is given in the
 * source code.
 */

/*
 * The program is used to test the usbdux sigma board
 */

#include <stdio.h>
#include <comedilib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "pyusbdux.h"


#define N_CHANS 16
#define BUFSZ N_CHANS*sizeof(long int)

const char filename[] = "/dev/comedi0";

comedi_t *dev;
int range = 0;
int subdevice = 0;
int aref;
int verbose;
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

int start(int n_channels, double fs)
{
	comedi_cmd cmd;
	int ret;

	range = 0;
	aref = AREF_GROUND;
	n_chan = n_channels;
	freq = fs;

	/* open the device */
	dev = comedi_open(filename);
	if(!dev){
		comedi_perror(filename);
		return errno;
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

        ret = comedi_get_cmd_generic_timed(dev, subdevice, &cmd, n_chan, 1e9 / freq);
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


sample_p getSampleFromBuffer() {
	while (!comedi_get_buffer_contents(dev,subdevice)) {};
	int ret = read(comedi_fileno(dev),buffer,bytes_per_sample * n_chan);
	if(ret < 0){
		perror("read");
	}else if(ret == 0){
		fprintf(stderr,"No data has arrived!");
	}else{
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
	return comedi_get_buffer_contents(dev,subdevice) > 0;
}


void stop() {
	comedi_cancel(dev,subdevice);
}
