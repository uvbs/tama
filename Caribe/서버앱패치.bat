:���������� �����ϴ� share������ �����ÿ�
@echo ===========��ġ���ҽ�/������/�����۸��ҽ� ������Ʈ.============
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
:@call ����������.bat
:@if not "%ERRORLEVEL%" == "0" goto FAIL

@echo -------------------extract caribe/Resource ������Ʈ-------------------
@cd %SHARE_PATH%
@"c:\Program Files\7-Zip\7z.exe" x -y app_res.zip -o%APP_PATH%\Resource >> log.txt
@if not "%ERRORLEVEL%" == "0" goto FAIL
@echo "=>����"

@echo -------------------copy caribe/win32/*.exe ������Ʈ-------------------
@copy /Y *.exe %APP_PATH%\win32
@if not "%ERRORLEVEL%" == "0" goto FAIL
@echo "=>����"

@echo ------------------- wwwroot/update ������Ʈ-------------------
@"c:\Program Files\7-Zip\7z.exe" x -y update.zip -o%WWW_PATH% >> log.txt
@if not "%ERRORLEVEL%" == "0" goto FAIL
@echo "=>����"

:OK
@echo ===��� ������Ʈ ����.(log.txt����)===
@pause
@goto QUIT

:FAIL
@echo !!!!!!!!!!!!!������Ʈ ����!!!!!!!!!!!!!!!!!!
@pause
@goto QUIT

:QUIT

