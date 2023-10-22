@echo OFF
rem   Prebuild file for CSTestApp
echo %CD%
rem ..\tools\internal\ldfutil -HLWPart1.cs -FLINK  -D.\PQ10API.dll -EPPHC -x6 -i..\Common\mainll.h  -m..\..\output\TestAPP\LinkWrap.cs
rem ..\Tools\ldfutil -HLWPart1.cs -FLINK  -D.\PQ10API.dll  -x6 -i..\Common\mainll.h  -m..\..\output\TestAPP\LinkWrap.cs >> 1.out
..\Tools\ldfutil -HLWPart1.cs -FLINK  -D.\PQ10API.dll  -x11 -i..\Common\mainll.h  -m..\..\output\TestAPP\LinkWrap.cs >> 1.out
..\Tools\ldfutil -HMDPart1.cs -x7 -i..\Common\menufunctions.h  -m..\..\output\TestAPP\MenuDelegates.cs
copy /y TestMenu.txt ..\..\output\TestApp\Debug\
copy /y TestMenu.txt ..\..\output\TestApp\Release\
copy /y ..\..\output\TestApp\LinkWrap.cs .
copy /y ..\..\output\TestApp\MenuDelegates.cs .
EXIT  0
