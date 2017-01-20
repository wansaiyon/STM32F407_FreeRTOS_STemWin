@echo off

:: 删除c文件夹下所有文件
del /Q c\*

:: 生成位图数据文件
for /r bmp %%i in (*.bmp) do (
	BmpCvtST %%i -saveas.\c\%%~ni,1,27 -exit
)

:: 生成resource.c
del /Q resource.c
(
	echo #include "resource.h"

	for /r c %%i in (*.c) do (
		echo #include "c\%%~ni.c"
	)

	echo=
	echo static const GUI_BITMAP* const resTbl[] =
	echo {

	for /r c %%i in (*.c) do (
		echo 	^&bm%%~ni,
	)
	echo };

	echo #define RES_BMP_NUM	(sizeof(resTbl^)/sizeof(resTbl[0]^)^)

	echo=
	echo const GUI_BITMAP* resGetBitmap(unsigned short id^)
	echo {
	echo 	if(id ^>= RES_BMP_NUM^)
	echo 	{
	echo 		return resTbl[0];
	echo 	}
	echo 	return resTbl[id];
	echo }
)>> resource.c

:: 生成resource.h文件
del /Q resource.h
(
	echo #ifndef _RES_H_
	echo #define _RES_H_
	echo=
	echo #include "GUI.h"
	echo=

	echo enum
	echo {
	setlocal enabledelayedexpansion
	for /r c %%i in (*.c) do (
		call :upcase %%~ni name
		echo 	BMP_!name!,
	)
	echo };

	echo=
	echo const GUI_BITMAP* resGetBitmap(unsigned short id^);
	echo=
	echo #endif
)>> resource.h

goto :EOF

:upcase
set $=&set "#=@%~1"
for %%a in (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z)do set #=!#:%%a=%%a!
set "%~2=%#:~1%"&exit/b