#ifndef PY_USBDUX_H
#define PY_USBDUX_H

typedef float* sample_p;

int open(int comediDeviceNumber);
int open();

int start(int n_channels, double fs);
int start(int nChan);
int start(int nChan, double fs);

int hasSampleAvilabale();

sample_p getSampleFromBuffer();

void stop();

void close();

#endif
