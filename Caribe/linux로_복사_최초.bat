:�񱳿� ������ temp������ ������ ���� �ѹ� core,full�� temp������ ī���Ѵ�. ���ĺ��� ����Ȱ͸�.bat�� �����Ѵ�.
del update.zip
del dir.txt
xcopy core\* temp\core /s /y /d /l >> dir.txt
xcopy full\* temp\full /s /y /d /l >> dir.txt
xcopy core\* temp\core /s /y /d
xcopy full\* temp\full /s /y /d
copy dir.txt temp\
"c:\Program Files\7-Zip\7z.exe" a update.zip @dir.txt -scsDOS
pause