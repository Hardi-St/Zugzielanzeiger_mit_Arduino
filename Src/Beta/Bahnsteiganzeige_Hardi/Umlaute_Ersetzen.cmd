@ECHO OFF
REM Replaxe the German umlauts in the given file
REM
REM Parameters:
REM  FileName
REM

SET File=%1

IF Not "%1"=="" Goto Start
  SET File=Umlaute_Test.txt
  Echo Datei: %File% wird erzeugt
  Copy Umlaute_Test_Org.txt %File% > NUL
:Start

REM For some reasons a backup must be created if --backup is used
Copy %File% %File%.bak > NUL

Set myPath=%~dp0
Set ErrFile=%myPath%Umlaute_Ersetzen_Err.txt
:echo ErrFile %ErrFile%
ECHO Processing %File%  >> %ErrFile%

ECHO %~n0 %1 %2

Set Opt=--backup --binary --c-style --quiet
Fart.exe %Opt% %File% "\xE4" "ae"      2>> %ErrFile%
Fart.exe %Opt% %File% "\xF6" "oe"      2>> %ErrFile%
Fart.exe %Opt% %File% "\xFC" "ue"      2>> %ErrFile%
Fart.exe %Opt% %File% "\xC4" "Ae"      2>> %ErrFile%
Fart.exe %Opt% %File% "\xD6" "Oe"      2>> %ErrFile%
Fart.exe %Opt% %File% "\xDC" "Ue"      2>> %ErrFile%
Fart.exe %Opt% %File% "\xDF" "ss"      2>> %ErrFile%
Del %File%.bak

IF "%File%"=="Umlaute_Test.txt" more %File%

:End
