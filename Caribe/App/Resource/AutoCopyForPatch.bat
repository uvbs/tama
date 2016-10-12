@echo
rmdir /s /q "..\..\patch\patch_server"
mkdir "..\..\patch\patch_server\"
mkdir "..\..\patch\patch_server\fonts\"
mkdir "..\..\patch\patch_server\fonts\en"
mkdir "..\..\patch\patch_server\icon\"
mkdir "..\..\patch\patch_server\img\"
mkdir "..\..\patch\patch_server\img\en\"
mkdir "..\..\patch\patch_server\prop\"
mkdir "..\..\patch\patch_server\prop\layout"
mkdir "..\..\patch\patch_server\prop\layout\en"
mkdir "..\..\patch\patch_server\prop\particles\"
mkdir "..\..\patch\patch_server\snd\"
mkdir "..\..\patch\patch_server\snd\mp3"
mkdir "..\..\patch\patch_server\spr\"
mkdir "..\..\patch\patch_server\spr\en"
mkdir "..\..\patch\patch_server\ui\"
mkdir "..\..\patch\patch_server\ui\en"

copy "fonts\*.ttf" "..\..\patch\patch_server\fonts"
copy "fonts\en" "..\..\patch\patch_server\fonts\en"
copy "icon\*.png" "..\..\patch\patch_server\icon"
copy "img\*.png" "..\..\patch\patch_server\img"
copy "img\en\*.png" "..\..\patch\patch_server\img\en"
rem copy "prop\*.xml" "..\..\patch\patch_server\prop"
rem copy "prop\*.txt" "..\..\patch\patch_server\prop"
rem copy "prop\*.lua" "..\..\patch\patch_server\prop"
copy "prop\game.lua" "..\..\patch\patch_server\prop"
copy "prop\global.xml" "..\..\patch\patch_server\prop"
copy "prop\propObj.xml" "..\..\patch\patch_server\prop"
copy "prop\exp_*.txt" "..\..\patch\patch_server\prop"
copy "prop\global.txt" "..\..\patch\patch_server\prop"
copy "prop\propUser.txt" "..\..\patch\patch_server\prop"
copy "prop\sound.txt" "..\..\patch\patch_server\prop"
copy "prop\text*.txt" "..\..\patch\patch_server\prop"
copy "prop\layout\*.xml" "..\..\patch\patch_server\prop\layout"
copy "prop\layout\en\*.xml" "..\..\patch\patch_server\prop\layout\en"
copy "prop\particles\*.xml" "..\..\patch\patch_server\prop\particles"
copy "snd\*.ogg" "..\..\patch\patch_server\snd"
copy "snd\mp3\*.mp3" "..\..\patch\patch_server\snd\mp3"
copy "spr\*.spr" "..\..\patch\patch_server\spr"
copy "ui\*.png" "..\..\patch\patch_server\ui"
copy "ui\en\*.png" "..\..\patch\patch_server\ui\en"
rem copy define*.h "..\..\patch\patch_server"
copy lang.txt "..\..\patch\patch_server"
copy verpacket.h "..\..\patch\patch_server"
copy apk_list.txt "..\..\patch\patch_server"

cd "..\..\patch\patch_server\img"
del /q /f pf_*.png
cd ..\spr"
del /q /f _*.spr
cd "..\prop"
del /q /f _*.*
cd layout
del /q /f _*.*
cd ..
cd ..\ui"
del /q /f _*.png
cd ..

net use * /delete /y
net use k: \\192.168.0.193\caribe_patch_tool Ahqlf099! /user:mtricks_dev1
@if not "%ERRORLEVEL%" == "0" goto FAIL

@echo 아무키나 누르면 카피가 시작됩니다.
pause
xcopy *.* "k:\patchFile" /e /s /y /i
@if not "%ERRORLEVEL%" == "0" goto FAIL

net use k: /del /y
@if not "%ERRORLEVEL%" == "0" goto FAIL

goto OK

:OK
@echo 복사성공! 193서버에서 extract.exe와 patchTool.exe를 실행시켜주십시오.
pause
goto QUIT

:FAIL
echo 에러.
pause
goto QUIT

:QUIT
