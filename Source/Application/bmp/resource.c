#include "resource.h"
#include "c\battery_1_24px.c"
#include "c\battery_2_24px.c"
#include "c\battery_3_24px.c"
#include "c\battery_empty_24px.c"
#include "c\bed_24px.c"
#include "c\logo.c"
#include "c\wifi_no_signal_24px.c"
#include "c\wifi_signal_24px.c"

static const GUI_BITMAP* const resTbl[] =
{
	&bmbattery_1_24px,
	&bmbattery_2_24px,
	&bmbattery_3_24px,
	&bmbattery_empty_24px,
	&bmbed_24px,
	&bmlogo,
	&bmwifi_no_signal_24px,
	&bmwifi_signal_24px,
};
#define RES_BMP_NUM	(sizeof(resTbl)/sizeof(resTbl[0]))

const GUI_BITMAP* resGetBitmap(unsigned short id)
{
	if(id >= RES_BMP_NUM)
	{
		return resTbl[0];
	}
	return resTbl[id];
}
