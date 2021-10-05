// Copyright (C) 2018 Archimedes Exhibitions GmbH,
// Saarbrücker Str. 24, Berlin, Germany
//
// This file contains proprietary source code and confidential
// information. Its contents may not be disclosed or distributed to
// third parties unless prior specific permission by Archimedes
// Exhibitions GmbH, Berlin, Germany is obtained in writing. This applies
// to copies made in any form and using any medium. It applies to
// partial as well as complete copies.

// Support for MCP4822 12bit dual channel DAC for AIO extensions (eg: custom01)
// Original repo: https://github.com/MajenkoLibraries/MCPDAC

#ifndef DAC_H
#define DAC_H

#include <stdint.h>

namespace AIO {

class DAC {
public:
    typedef enum Channel {
        CHANNEL_A,
        CHANNEL_B
    } Channel;

    typedef enum Gain {
        GAIN_1X,
        GAIN_2X
    } Gain;

    DAC();

    void begin();
    void begin(uint8_t cspin_);
    void begin(uint8_t cspin_, uint8_t ldacpin_);
    void set_value(Channel channel, uint16_t dac_value, Gain gain=GAIN_1X);
    void sync_ldac();

private:
    bool use_ldac;

    uint8_t cspin;
    uint8_t ldacpin;
};

} // namespace AIO

#endif
