#ifndef _INCLUDES_H_
#define _INCLUDES_H_

/* C Lib */
#include <stdio.h>

/* Driver */
#include "driver.h"

/* Selftest */
enum
{
    SELFTEST_ERROR_NULL,            // �޴���
    SELFTEST_ERROR_EEPROM_HW,       // EEPROMӲ������
    SELFTEST_ERROR_EEPROM_ARGS,     // EEPROM��������
};
u8 selftestGetResult(u8 chan);

#endif
