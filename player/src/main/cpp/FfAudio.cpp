//
// Created by Administrator on 2019/2/18.
//

#include "FfAudio.h"

FfAudio::FfAudio() {
    this->ffQueue = new FfQueue();
    this->ffPlayStatus = 0;
}
