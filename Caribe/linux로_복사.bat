:193�������� wwwroot/caribe/update�� ī��
@set YEAR=%date:~0,4%
@set MONTH=%date:~5,2%
@set DAY=%date:~8,2%
@set HOUR=%time:~0,2%
@set MINUTE=%time:~3,2%
@set SECOND=%time:~6,2%

@set POSTFIX=%YEAR%-%MONTH%-%DAY%_%HOUR%-%MINUTE%-%SECOND%

@echo ==========��ġ���� ������========
@del update.zip
@"c:\Program Files\7-Zip\7z.exe" a update.zip core\
@if not "%ERRORLEVEL%" == "0" goto FAIL
@"c:\Program Files\7-Zip\7z.exe" a update.zip Full\
@if not "%ERRORLEVEL%" == "0" goto FAIL

@echo ==========������ ������ ������========
pscp -scp -pw q#E35ktvhQjnt update.zip root@211.125.93.152:~/test/share
@if not "%ERRORLEVEL%" == "0" goto FAIL

:@echo ==========������ ���ҽ� ������========
:@cd C:\jenkins_work\workspace\caribe\Caribe\App\Resource
:@if not "%ERRORLEVEL%" == "0" goto FAIL
:@call ������ġ��_ī��.bat
:@if not "%ERRORLEVEL%" == "0" goto FAIL


: ������ => GMO������ ����
:plink root@211.125.93.152 -pw q#E35ktvhQjnt -m cmd.txt
:@if not "%ERRORLEVEL%" == "0" goto FAIL

:OK
@echo �۾�����
@pause
@goto QUIT

:FAIL
@echo �۾�����
@pause
@goto QUIT

:QUIT

