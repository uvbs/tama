@set YEAR=%date:~0,4%
@set MONTH=%date:~5,2%
@set DAY=%date:~8,2%
@set HOUR=%time:~0,2%
@set MINUTE=%time:~3,2%
@set SECOND=%time:~6,2%

@set POSTFIX=%YEAR%-%MONTH%-%DAY%_%HOUR%-%MINUTE%-%SECOND%
:@set LOGFILE=log%POSTFIX%.txt
@set LOGFILE=log.txt

xcopy "..\proj.android\obj" d:\caribe\build_obj\obj%POSTFIX% /a /e /y
copy show_line.bat d:\caribe\build_obj\obj%POSTFIX% /y


pause
