#ifndef PY_USBDUX_H
#define PY_USBDUX_H

#define N_CHANS 16

class Callback{
public:
	virtual void hasSample(float s[N_CHANS]) = 0;
	virtual ~Callback() {};
	Callback() {};
};

void open(int comediDeviceNumber);
void open();

void start(Callback* cb, int nChan);
void start(Callback* cb, int nChan, double fs);

int hasSampleAvailable();

float getSamplingRate();

void stop();

void digital_out(int channel, int value);
int digital_in(int channel);
void analogue_out(int channel, int value);
int get_analogue_out_max_raw_value();

const char* get_board_name();

void close();

#endif
