:app������ �����ÿ�
@echo ===========���� ������ ��/�۸��ҽ�/��ġ���ҽ��� ����մϴ�.============
@set YEAR=%date:~0,4%
@set MONTH=%date:~5,2%
@set DAY=%date:~8,2%
@set HOUR=%time:~0,2%
@set MINUTE=%time:~3,2%
@set SECOND=%time:~6,2%

@set POSTFIX=%YEAR%-%MONTH%-%DAY%_%HOUR%-%MINUTE%-%SECOND%
:@set LOGFILE=log%POSTFIX%.txt
@set LOGFILE=log.txt

@echo ----���� �۰� ���ҽ� ���----
@cd C:\Users\Administrator\Desktop\caribe
@if not "%ERRORLEVEL%" == "0" goto FAIL
@"c:\Program Files\7-Zip\7z.exe" a caribe%POSTFIX%.zip win32
@if not "%ERRORLEVEL%" == "0" goto FAIL
@"c:\Program Files\7-Zip\7z.exe" a caribe%POSTFIX%.zip Resource
@if not "%ERRORLEVEL%" == "0" goto FAIL
@move caribe%POSTFIX%.zip backup >> %LOGFILE%
@if not "%ERRORLEVEL%" == "0" goto FAIL

@echo -----��ġ���� ���ҽ� ��� ��-----
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
@echo =====�������=====
@pause
@goto QUIT

:FAIL
@echo !!!!!!!!!!!!�������!!!!!!!!!!!
@pause
@goto QUIT

:QUIT

