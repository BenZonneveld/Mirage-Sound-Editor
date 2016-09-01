@echo off
vcredist_x86.exe
mkdir "C:\Program Files\Mirage Editor"
copy "Mirage Editor.exe" "C:\Program Files\Mirage Editor"
copy *.dll "C:\Program Files\Mirage Editor"
copy "Mirage Editor.chm" "C:\Program Files\Mirage Editor"