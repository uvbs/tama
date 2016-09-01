:app폴더에 넣으시오
@echo ===========현재 버전의 앱/앱리소스/패치리소스를 백업합니다.============
@set YEAR=%date:~0,4%
@set MONTH=%date:~5,2%
@set DAY=%date:~8,2%
@set HOUR=%time:~0,2%
@set MINUTE=%time:~3,2%
@set SECOND=%time:~6,2%

@set POSTFIX=%YEAR%-%MONTH%-%DAY%_%HOUR%-%MINUTE%-%SECOND%
:@set LOGFILE=log%POSTFIX%.txt
@set LOGFILE=log.txt

@echo ----서버 앱과 리소스 백업----
@cd C:\Users\Administrator\Desktop\caribe
@if not "%ERRORLEVEL%" == "0" goto FAIL
@"c:\Program Files\7-Zip\7z.exe" a caribe%POSTFIX%.zip win32
@if not "%ERRORLEVEL%" == "0" goto FAIL
@"c:\Program Files\7-Zip\7z.exe" a caribe%POSTFIX%.zip Resource
@if not "%ERRORLEVEL%" == "0" goto FAIL
@move caribe%POSTFIX%.zip backup >> %LOGFILE%
@if not "%ERRORLEVEL%" == "0" goto FAIL

@echo -----패치서버 리소스 백업 중-----
@cd C:\inetpub\wwwroot\caribe\update
@if not "%ERRORLEVEL%" == "0" goto FAIL
@"c:\Program Files\7-Zip\7z.exe" a update_prev%POSTFIX%.zip core >> %LOGFILE%
@if not "%ERRORLEVEL%" == "0" goto FAIL
@"c:\Program Files\7-Zip\7z.exe" a update_prev%POSTFIX%.zip Full >> %LOGFILE%
@if not "%ERRORLEVEL%" == "0" goto FAIL
@mkdir backup >> %LOGFILE%
@move update_prev%POSTFIX%.zip backup >> %LOGFILE%
@if not "%ERRORLEVEL%" == "0" goto FAIL

:OK
@echo =====백업성공=====
@pause
@goto QUIT

:FAIL
@echo !!!!!!!!!!!!백업실패!!!!!!!!!!!
@pause
@goto QUIT

:QUIT

