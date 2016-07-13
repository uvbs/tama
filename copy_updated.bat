rem xcopy c:\p4v\iPhone_zero d:\_backup\today /a /e /y /exclude:exclude.txt /l  >> dir_copy.txt
xcopy d:\_backup\today d:\_backup\today_updated /d:07-12-2016 /a /e /y /s /exclude:exclude_updated.txt
pause