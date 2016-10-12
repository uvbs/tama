:비교용 폴더인 temp폴더가 없으면 최초 한번 core,full을 temp폴더로 카피한다. 이후부턴 변경된것만.bat를 실행한다.
del update.zip
del dir.txt
xcopy core\* temp\core /s /y /d /l >> dir.txt
xcopy full\* temp\full /s /y /d /l >> dir.txt
xcopy core\* temp\core /s /y /d
xcopy full\* temp\full /s /y /d
copy dir.txt temp\
"c:\Program Files\7-Zip\7z.exe" a update.zip @dir.txt -scsDOS
pause