@ECHO OFF
SET Elf_File="C:\Users\Hardi\AppData\Local\Temp\arduino_build_371159\Bahnsteiganzeige_Hardi.ino.elf"
IF NOT x%1x == xx Set Elf_File=%1
"C:\Program Files (x86)\Arduino\hardware\tools\avr\bin\avr-nm" -Crtd --size-sort "%Elf_File%" > MemUsage.txt
ECHO.
ECHO Bei einem Fehler muss der Pfad zur Datei Bahnsteiganzeige_V3.ino.elf
ECHO in dieser BAT Datei angepasst werden
ECHO.
ECHO MemUsage.txt written
ECHO.
PAUSE
