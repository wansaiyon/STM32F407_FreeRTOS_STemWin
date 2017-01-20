#ifndef _INCLUDES_H_
#define _INCLUDES_H_

/* C Lib */
#include <stdio.h>

/* Driver */
#include "driver.h"

/* Selftest */
enum
{
    SELFTEST_ERROR_NULL,            // 无错误
    SELFTEST_ERROR_EEPROM_HW,       // EEPROM硬件错误
    SELFTEST_ERROR_EEPROM_ARGS,     // EEPROM参数错误
};
u8 selftestGetResult(u8 chan);

#endif
