@ECHO OFF
REM Based on: C:\Dat\Märklin\Arduino\Zugzielanzeiger\Bahnsteiganzeige_Hardi\Fonts\aus neuer Lib u8g2\u8g2-master\tools\font\bdfconv\create_mingw_bdfconv_exe.bat


REM Old Line ./bdfconv.exe -v -f 1 -m '32-127' ../bdf/helvB18.bdf -o helvb18_tf.c  -n u8g2_font_helvB18_tf -d ../bdf/helvB18.bdf

:: German Umlauts (ANSI Nr)
:: 3/4    190
:: Ae     196
:: Oe     214
:: Ue     220
:: ss     223
:: ae     228
:: oe     246
:: ue     252


Call :Convert_One tpss   "32-127,196,214,220,223,228,246,252"
Call :Convert_One 4x6    "32-127,196,214,220,223,228,246,252"
Call :Convert_One 5x8    "32-127,196,214,220,223,228,246,252"
Call :Convert_One 6x13B  "32-127,190,196,214,220,223,228,246,252"
Call :Convert_One tpssb  "32-127,196,214,220,223,228,246,252"

Goto End


::-----------
:Convert_One
::-----------
:: Parameter: BDF-font name without extention
::
  bdfconv.exe -v -f 1 -m "32-255" %1.bdf -o %1_tf.c  -n u8g2_font_%1_tf -d %1.bdf
  DEL %1.tga
  REN bdf.tga %1.tga
  DEL %1_tf.h
  REN %1_tf.c %1_tf.h
  COPY %1_tf.h ..
  ::type %1_tf.c


  bdfconv.exe -v -f 1 -m %2 %1.bdf -o %1_t_german.c  -n u8g2_font_%1_t_german -d %1.bdf
  DEL %1_german.tga
  REN bdf.tga %1_german.tga
  DEL %1_t_german.h
  REN %1_t_german.c %1_t_german.h
  COPY %1_t_german.h ..
  ::type %1_t_german.c
EXIT /B


:End
ECHO.
ECHO.
dir *.h
