xcopy "c:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src" "d:\_caribe_today\Caribe\App\game_src"/e /k /i
xcopy "c:\xuzhu_work\Project\iPhone_zero\Caribe\App\pc_prj" "d:\_caribe_today\Caribe\App\pc_prj"/e /k /i /exclude:exclude.txt
xcopy "c:\xuzhu_work\Project\iPhone_zero\Caribe\App\Resource" "d:\_caribe_today\Caribe\App\Resource"/e /k /i /exclude:exclude_zip.txt
copy "c:\xuzhu_work\Project\iPhone_zero\Caribe\*.txt" "d:\_caribe_today\Caribe\"
xcopy "c:\xuzhu_work\Project\iPhone_zero\XE\Common" "d:\_caribe_today\XE\Common"/e /k /i /exclude:exclude.txt
xcopy "c:\xuzhu_work\Project\iPhone_zero\XE\XFramework" "d:\_caribe_today\XE\XFramework"/e /k /i /exclude:exclude.txt
xcopy "c:\xuzhu_work\Project\iPhone_zero\XE\prj\android" "d:\_caribe_today\XE\prj\android"/e /k /i /exclude:exclude.txt
xcopy "c:\xuzhu_work\Project\iPhone_zero\XE\prj\WinFTGL" "d:\_caribe_today\XE\prj\WinFTGL"/e /k /i /exclude:exclude.txt
xcopy "c:\xuzhu_work\Project\iPhone_zero\XE\prj\WinOolong" "d:\_caribe_today\XE\prj\WinOolong"/e /k /i /exclude:exclude.txt
xcopy "c:\xuzhu_work\Project\iPhone_zero\XE\prj\xe_client" "d:\_caribe_today\XE\prj\xe_client"/e /k /i /exclude:exclude.txt
xcopy "c:\xuzhu_work\Project\iPhone_zero\XE\prj\xe_server" "d:\_caribe_today\XE\prj\xe_server"/e /k /i /exclude:exclude.txt
xcopy "c:\xuzhu_work\Project\iPhone_zero\XE\prj\xe_console" "d:\_caribe_today\XE\prj\xe_console"/e /k /i /exclude:exclude.txt
xcopy "c:\xuzhu_work\Project\iPhone_zero\XE\prj\zlib" "d:\_caribe_today\XE\prj\zlib"/e /k /i /exclude:exclude.txt
pause
