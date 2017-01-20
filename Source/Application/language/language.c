#include "language.h"

// SimplifiedChinese
#include "SimplifiedChinese.c"
#include "MyFont_SimplifiedChinese_24.c"
#include "MyFont_SimplifiedChinese_32.c"

// English
#include "English.c"
#include "MyFont_English_24.c"
#include "MyFont_English_32.c"

// French
#include "French.c"
#include "MyFont_French_24.c"
#include "MyFont_French_32.c"

static const struct
{
	const char** language;
	const GUI_FONT* myfont24;
	const GUI_FONT* myfont32;
}langInfo[3] =
{
	{
		LangSimplifiedChinese,
		&GUI_FontMyFont_SimplifiedChinese_24,
		&GUI_FontMyFont_SimplifiedChinese_32,
	},
	{
		LangEnglish,
		&GUI_FontMyFont_English_24,
		&GUI_FontMyFont_English_32,
	},
	{
		LangFrench,
		&GUI_FontMyFont_French_24,
		&GUI_FontMyFont_French_32,
	},
};
#define LANG_NUM (sizeof(langInfo)/sizeof(langInfo[0]))

const GUI_FONT* LangGetFont(unsigned char lang, unsigned char font_size)
{
	if(lang >= LANG_NUM)
	{
		return 0;
	}
	switch(font_size)
	{
	case 24: return langInfo[lang].myfont24;
	case 32: return langInfo[lang].myfont32;
	default: return 0;
	}
};

const char* LangGetStr(unsigned char lang, unsigned short str_id)
{
	if((lang>=LANG_NUM) || (str_id>=STR_TOTAL_NUM))
	{
		return 0;
	}
	return langInfo[lang].language[str_id];
};
