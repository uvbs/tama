:193번서버의 wwwroot/caribe/update에 카피
@set YEAR=%date:~0,4%
@set MONTH=%date:~5,2%
@set DAY=%date:~8,2%
@set HOUR=%time:~0,2%
@set MINUTE=%time:~3,2%
@set SECOND=%time:~6,2%

@set POSTFIX=%YEAR%-%MONTH%-%DAY%_%HOUR%-%MINUTE%-%SECOND%

@echo ==========패치파일 압축중========
@del update.zip
@"c:\Program Files\7-Zip\7z.exe" a update.zip core\
@if not "%ERRORLEVEL%" == "0" goto FAIL
@"c:\Program Files\7-Zip\7z.exe" a update.zip Full\
@if not "%ERRORLEVEL%" == "0" goto FAIL

@echo ==========리눅스 서버로 전송중========
pscp -scp -pw q#E35ktvhQjnt update.zip root@211.125.93.152:~/test/share
@if not "%ERRORLEVEL%" == "0" goto FAIL

:@echo ==========서버앱 리소스 전송중========
:@cd C:\jenkins_work\workspace\caribe\Caribe\App\Resource
:@if not "%ERRORLEVEL%" == "0" goto FAIL
:@call 서버패치용_카피.bat
:@if not "%ERRORLEVEL%" == "0" goto FAIL


: 리눅스 => GMO서버로 전송
:plink root@211.125.93.152 -pw q#E35ktvhQjnt -m cmd.txt
:@if not "%ERRORLEVEL%" == "0" goto FAIL

:OK
@echo 작업성공
@pause
@goto QUIT

:FAIL
@echo 작업실패
@pause
@goto QUIT

:QUIT

