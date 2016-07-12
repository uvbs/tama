rmdir /s /q "../../proj.android/assets"
mkdir "../../proj.android/assets/"
mkdir "../../proj.android/assets/fonts/"
mkdir "../../proj.android/assets/fonts/en"
mkdir "../../proj.android/assets/icon/"
mkdir "../../proj.android/assets/img/"
mkdir "../../proj.android/assets/img/en/"
mkdir "../../proj.android/assets/prop/"
mkdir "../../proj.android/assets/prop/layout"
mkdir "../../proj.android/assets/prop/layout/en"
mkdir "../../proj.android/assets/prop/particles/"
mkdir "../../proj.android/assets/snd/"
mkdir "../../proj.android/assets/snd/mp3"
mkdir "../../proj.android/assets/spr/"
mkdir "../../proj.android/assets/spr/en"
mkdir "../../proj.android/assets/ui/"
mkdir "../../proj.android/assets/ui/en"


copy "fonts\*.ttf" "../../proj.android/assets/fonts"
copy "fonts\en" "../../proj.android/assets/fonts/en"
copy "icon\*.png" "../../proj.android/assets/icon"
copy "img\*.png" "../../proj.android/assets/img"
copy "img\en\*.png" "../../proj.android/assets/img/en"
copy "prop\game.lua" "../../proj.android/assets/prop"
copy "prop\global.xml" "../../proj.android/assets/prop"
copy "prop\propObj.xml" "../../proj.android/assets/prop"
copy "prop\exp_*.txt" "../../proj.android/assets/prop"
copy "prop\global.txt" "../../proj.android/assets/prop"
copy "prop\propUser.txt" "../../proj.android/assets/prop"
copy "prop\sound.txt" "../../proj.android/assets/prop"
copy "prop\text*.txt" "../../proj.android/assets/prop"
copy "prop\hslmap.txt" "../../proj.android/assets/prop"

copy "prop\layout\*.xml" "../../proj.android/assets/prop/layout"
copy "prop\particles\*.xml" "../../proj.android/assets/prop/particles"
copy "snd\*.ogg" "../../proj.android/assets/snd"
copy "snd\mp3\*.mp3" "../../proj.android/assets/snd\mp3"
copy "spr\*.spr" "../../proj.android/assets/spr"
copy "ui\*.png" "../../proj.android/assets/ui"
copy "ui\en\*.png" "../../proj.android/assets/ui/en"
copy lang.txt "../../proj.android/assets"
copy verpacket.h "../../proj.android/assets"
copy "..\LastUpdate.txt" "../../proj.android/assets/"

cd "../../proj.android/assets/img"
del /q /f pf_*.png
del /q /f _*.png
cd "../spr"
del /q /f _*.spr
cd "../prop"
del /q /f _*.*

pause

