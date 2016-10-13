rem 최소용량의 패치버전으로 apk만들때 사용
rmdir /s /q "..\..\proj.android\assets"

mkdir "..\..\proj.android\assets\"
copy lang.txt "..\..\proj.android\assets\"
copy "VerPacket.h" "..\..\proj.android\assets\"
copy "..\LastUpdate.txt" "../../proj.android/assets/"

mkdir "..\..\proj.android\assets\fonts\"
copy "fonts\bold.ttf"  "..\..\proj.android\assets\fonts"
copy "fonts\normal.ttf" "..\..\proj.android\assets\fonts\"
copy "fonts\res_num.ttf"  "..\..\proj.android\assets\fonts\"
copy "fonts\damage.ttf"  "..\..\proj.android\assets\fonts\"
copy "fonts\mnls.ttf"  "..\..\proj.android\assets\fonts\"

mkdir "..\..\proj.android\assets\fonts\en\"
copy "fonts\en\bold.ttf"  "..\..\proj.android\assets\fonts\en\"
copy "fonts\en\normal.ttf" "..\..\proj.android\assets\fonts\en\"

mkdir "..\..\proj.android\assets\prop\"
copy "prop\global.xml" "..\..\proj.android\assets\prop\"
copy "prop\text_ko.txt" "..\..\proj.android\assets\prop\"
copy "prop\text_en.txt" "..\..\proj.android\assets\prop\"
copy "prop\global.txt" "..\..\proj.android\assets\prop\"
copy "prop\sound.txt" "..\..\proj.android\assets\prop\"
copy "prop\hslmap.txt" "..\..\proj.android\assets\prop\"

mkdir "..\..\proj.android\assets\ui\"
copy "ui\bg_patch.png" "..\..\proj.android\assets\ui\"
copy "ui\common_butt_mid.png" "..\..\proj.android\assets\ui\"
copy "ui\common_butt_small.png" "..\..\proj.android\assets\ui\"
copy "ui\popup01.png" "..\..\proj.android\assets\ui\"
copy "ui\bg_popup_dark.png" "..\..\proj.android\assets\ui\"
copy "ui\fb_empty.png" "..\..\proj.android\assets\ui\"

mkdir "..\..\proj.android\assets\spr\"
copy "spr\ui_glow_butt.spr" "..\..\proj.android\assets\spr\"
copy "spr\ui_loading.spr" "..\..\proj.android\assets\spr\"
copy "spr\opening.spr" "..\..\proj.android\assets\spr\"

pause
