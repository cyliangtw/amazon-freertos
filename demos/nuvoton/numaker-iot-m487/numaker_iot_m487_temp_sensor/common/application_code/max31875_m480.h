#ifndef	__MAX31875_M480_H__
#define __MAX31875_M480_H__

#include "NuMicro.h"
#include "max31875.h"
#include "max31875_c.h"

int8_t m480_max31875_init(uint8_t slave_id);
void m480_delay_ms(uint32_t period);

#endif
