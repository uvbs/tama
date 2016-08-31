rmdir /s /q "..\..\patch\server"
mkdir "..\..\patch\server\"
mkdir "..\..\patch\server\prop\"

copy "prop\*.xml" "..\..\patch\server\prop"
copy "prop\*.txt" "..\..\patch\server\prop"
copy "prop\*.lua" "..\..\patch\server\prop"
copy *.h "..\..\patch\server"
copy *.txt "..\..\patch\server"

cd "..\..\patch\server\prop"
del /q /f _*.*

:압축
@cd ..
@"c:\Program Files\7-Zip\7z.exe" a -r app_res.zip *.*
@if not "%ERRORLEVEL%" == "0" goto FAIL

: GMO(windows)서버의 share로 전송
cd ..\..\
pscp -scp -pw q#E35ktvhQjnt patch\server\app_res.zip root@211.125.93.152:~/test/share
@if not "%ERRORLEVEL%" == "0" goto FAIL


:OK
@echo 작업성공
@pause
@goto QUIT

:FAIL
@echo 작업실패
@pause
@goto QUIT

:QUIT

