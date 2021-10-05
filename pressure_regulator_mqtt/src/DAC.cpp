// Copyright (C) 2018 Archimedes Exhibitions GmbH,
// Saarbrücker Str. 24, Berlin, Germany
//
// This file contains proprietary source code and confidential
// information. Its contents may not be disclosed or distributed to
// third parties unless prior specific permission by Archimedes
// Exhibitions GmbH, Berlin, Germany is obtained in writing. This applies
// to copies made in any form and using any medium. It applies to
// partial as well as complete copies.

#include <Arduino.h>
#include <SPI.h>

#include "DAC.h"

namespace AIO {

namespace {
    const uint8_t DEFAULT_CS = 10;

    const uint16_t DACA_SELECT = 0;
    const uint16_t DACB_SELECT = 1 << 15;
    const uint16_t GAIN_1X_SELECT = 1 << 13;
    const uint16_t GAIN_2X_SELECT = 0;
    const uint16_t SHDN_DISABLE = 1 << 12;
    const uint16_t SHDN_ENABLE = 0;

    const SPISettings spi_settings(8000000, MSBFIRST, SPI_MODE0);
}


DAC::DAC() :
    use_ldac(false), cspin(0), ldacpin(0)
{
}

void DAC::begin()
{
    begin(DEFAULT_CS);
}

void DAC::begin(uint8_t cspin_)
{
    cspin = cspin_;
    use_ldac = false;

    digitalWrite(cspin, HIGH);
    pinMode(cspin, OUTPUT);

    SPI.begin();
}

void DAC::begin(uint8_t cspin_, uint8_t ldacpin_)
{
    begin(cspin_);

    use_ldac = true;
    ldacpin = ldacpin_;

    digitalWrite(ldacpin, HIGH);
    pinMode(ldacpin, OUTPUT);
}

void DAC::set_value(Channel channel, uint16_t dac_value, Gain gain)
{
    uint16_t data;

    data = (channel == CHANNEL_A) ? DACA_SELECT : DACB_SELECT;
    data |= SHDN_DISABLE;
    data |= (gain == GAIN_1X) ? GAIN_1X_SELECT : GAIN_2X_SELECT;
    data |= (dac_value & 0x0FFF);

    SPI.beginTransaction(spi_settings);
    digitalWrite(cspin, LOW);
    SPI.transfer(data >> 8);
    SPI.transfer(data & 0xFF);
    digitalWrite(cspin, HIGH);
    SPI.endTransaction();
}

void DAC::sync_ldac()
{
    if (use_ldac) {
        digitalWrite(ldacpin, LOW);
        digitalWrite(ldacpin, HIGH);
    }
}

} // namespace AIO
