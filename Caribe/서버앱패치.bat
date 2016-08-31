:리눅스에서 저장하는 share폴더에 넣으시오
@echo ===========패치리소스/서버앱/서버앱리소스 업데이트.============
@set YEAR=%date:~0,4%
@set MONTH=%date:~5,2%
@set DAY=%date:~8,2%
@set HOUR=%time:~0,2%
@set MINUTE=%time:~3,2%
@set SECOND=%time:~6,2%

@set POSTFIX=%YEAR%-%MONTH%-%DAY%_%HOUR%-%MINUTE%-%SECOND%
@set APP_PATH=C:\Users\Administrator\Desktop\caribe
@set WWW_PATH=C:\inetpub\wwwroot\caribe\update
@set SHARE_PATH=C:\Users\Administrator\Desktop\share

@cd %APP_PATH%
:@call 현재버전백업.bat
:@if not "%ERRORLEVEL%" == "0" goto FAIL

@echo -------------------extract caribe/Resource 업데이트-------------------
@cd %SHARE_PATH%
@"c:\Program Files\7-Zip\7z.exe" x -y app_res.zip -o%APP_PATH%\Resource >> log.txt
@if not "%ERRORLEVEL%" == "0" goto FAIL
@echo "=>성공"

@echo -------------------copy caribe/win32/*.exe 업데이트-------------------
@copy /Y *.exe %APP_PATH%\win32
@if not "%ERRORLEVEL%" == "0" goto FAIL
@echo "=>성공"

@echo ------------------- wwwroot/update 업데이트-------------------
@"c:\Program Files\7-Zip\7z.exe" x -y update.zip -o%WWW_PATH% >> log.txt
@if not "%ERRORLEVEL%" == "0" goto FAIL
@echo "=>성공"

:OK
@echo ===모든 업데이트 성공.(log.txt참조)===
@pause
@goto QUIT

:FAIL
@echo !!!!!!!!!!!!!업데이트 실패!!!!!!!!!!!!!!!!!!
@pause
@goto QUIT

:QUIT

