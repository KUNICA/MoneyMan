rem Anton Pirogov's windows dist script (requires installed MinGW+MSYS+Allegro)
mkdir dist
mkdir dist\win32
mkdir dist\win32\gfx
copy alleg42.dll dist\win32
xcopy gfx dist\win32\gfx
C:\MinGW\bin\gcc.exe -o dist\win32\MoneyMan.exe *.c -lalleg -lm
