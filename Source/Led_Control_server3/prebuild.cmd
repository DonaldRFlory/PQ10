@echo OFF
rem main prebuild file for basic link test setup--------------------------------------------------------------------------
rem calling four different sub-cmd files to ease debugging
rem  Created by:   Don Flory
rem --------------------------------------------------------------------------

echo Start of Building Link list files
echo.

copy /y ..\common\link.h .
copy /y ..\common\slink.h .
copy /y ..\common\mlink.h .
rem copy /y ..\common\mainll.h .
rem copy /y ..\common\basicll.h .
copy /y ..\common\slavparm.h .
copy /y ..\common\linkctrl.h .
copy /y ..\common\hostopsys.h .
copy /y ..\common\type.h .
copy /y ..\common\linkerror.h .
copy /y ..\common\linkerrdef.h .
if not exist ..\..\Output md ..\..\Output
if not exist ..\..\Output\LCS_arduino md ..\..\output\LCS_arduino


rem mode is don't care below. We are generating the slave side cpp file with
rem LinkDef structure:

copy /Y ..\common\basicll.h  ..\..\Output\LCS_arduino\basicll.h
copy /Y ..\common\mainll.h  ..\..\Output\LCS_arduino\slvmainll.h
rem so, the input file below though named slvmainll.h is in fact a copy of mainll.h
rem This is because ldfgen generates an include of its input header file at top of output file slavelink.cpp
..\tools\ldfutil  -x0 -i..\..\Output\LCS_arduino\slvmainll.h   -s..\..\Output\LCS_arduino\slavelink.cpp

rem mode is don't care below. We are generating the slave side header file with
rem pointer arguments removed for block up or down functions
..\tools\ldfutil -x10 -i..\common\mainll.h -q..\..\Output\LCS_arduino\slvmainll.h

copy /y ..\..\output\LCS_arduino\slvmainll.h .
copy /y ..\..\output\LCS_arduino\slavelink.cpp .
rem copy /Y mainll.h  ..\..\output\LCS_arduino\slvmainll.h
echo.
exit /B 0
