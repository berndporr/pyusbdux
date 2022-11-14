#ifndef PY_USBDUX_H
#define PY_USBDUX_H

void open(int comediDeviceNumber);
void open();

void start(int nChan);
void start(int nChan, double fs);

int hasSampleAvailable();

float getSamplingRate();

void stop();

void digital_out(int channel, int value);
int digital_in(int channel);
void analogue_out(int channel, int value);
int get_analogue_out_max_raw_value();

const char* get_board_name();

void close();

class Callback{
    public:
    virtual void hasSample(float sample[16]) = 0;                             
    virtual ~Callback() {}; 
};

void setCallback(Callback * cb);

#endif
