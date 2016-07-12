cd "../win32"
del /q /f loginserver*.exe
del /q /f loginserver*.pdb
del /q /f gameserver*.exe
del /q /f gameserver*.pdb
del /q /f dbagentserver*.exe
del /q /f dbagentserver*.pdb
cd "../pc_prj"

copy loginserver*.* ..\win32 /y
copy dbagentserver*.* ..\win32 /y
copy gameserver*.* ..\win32 /y
copy gameserverBot.exe ..\win32\gameserverBot2.exe /y
copy gameserverBot.pdb ..\win32\gameserverBot2.pdb /y
copy gameserverBot.exe ..\win32\gameserverBot3.exe /y
copy gameserverBot.pdb ..\win32\gameserverBot3.pdb /y
copy gameserverBot.exe ..\win32\gameserverBot4.exe /y
copy gameserverBot.pdb ..\win32\gameserverBot4.pdb /y

copy bot.exe ..\win32 /y
copy bot.pdb ..\win32 /y

copy Caribe.exe ..\win32 /y
copy Caribe.pdb ..\win32 /y

copy Caribe_single.exe ..\win32 /y
copy Caribe_single.pdb ..\win32 /y

pause
