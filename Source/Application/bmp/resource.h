#ifndef _RES_H_
#define _RES_H_

#include "GUI.h"

enum
{
	BMP_BATTERY_1_24PX,
	BMP_BATTERY_2_24PX,
	BMP_BATTERY_3_24PX,
	BMP_BATTERY_EMPTY_24PX,
	BMP_BED_24PX,
	BMP_LOGO,
	BMP_WIFI_NO_SIGNAL_24PX,
	BMP_WIFI_SIGNAL_24PX,
};

const GUI_BITMAP* resGetBitmap(unsigned short id);

#endif
