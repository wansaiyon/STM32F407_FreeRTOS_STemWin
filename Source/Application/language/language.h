#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#include "GUI.h"

enum
{
	STR_ALARM_BATT_EMPTY,
	STR_INFO_CAN_NOT_INTO_MAINTAIN,
	STR_INFO_CAN_NOT_INTO_WORK,
	STR_INFO_CAN_NOT_SWITCH_OFF,
	STR_INFO_SWITCHING_OFF,
	STR_LANGUAGE,
	STR_MENU_ABOUT,
	STR_MENU_ADVANCED_SETTING,
	STR_MENU_BRAND_SELECTION,
	STR_MENU_MODE_SELECTION,
	STR_MENU_NAME,
	STR_MENU_SYSTEM_CONFIG,
	STR_TOTAL_NUM
};

const GUI_FONT* LangGetFont(unsigned char lang, unsigned char font_size);
const char* LangGetStr(unsigned char lang, unsigned short str_id);

#endif
