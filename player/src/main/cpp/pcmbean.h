//
// Created by yangw on 2018-4-1.
//


#include <SoundTouch.h>

using namespace soundtouch;

class PcmBean {

public:
    char *buffer;
    int buffsize;

public:
    PcmBean(SAMPLETYPE *buffer, int size);
    ~PcmBean();


};

