/*
 -------------------------------------------------------------------------------
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 -------------------------------------------------------------------------------


 ********************************************************
 * Arduino program to generate railway station displays *
 ********************************************************

 This program is based the ideas and programs from Tobias, Klaus and Fredddy:
 - https://stummiforum.de/viewtopic.php?f=21&t=131472
 - https://stummiforum.de/viewtopic.php?f=21&t=131472&start=50#p1849272
 - https://stummiforum.de/viewtopic.php?f=21&t=131472&start=50#p1895237


 List of changes compared to the basic program:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 - Several OLED displays could be connected to one Arduino
 - DCC commands could be used to control due displays
 - More than 100 displays could be stored in the FLASH
 - Buttons or other external signals could be used to change the display
 - Flexible adaptable display layout
 - Uses the new u8g2lib to access the OLED panel
 - Faster screen update
 - Saving memory by
   - using special fonts with only german "Umlauts"
   - using PROGMEM
   - optimizing the program
 - Serial port could be used to send the displays (Compatible to Fredddy's program)

 Several OLED panels
 ~~~~~~~~~~~~~~~~~~~~
 The program could display different text messages on several OLED panels.
 For each panel a MOS-Fet (BS170) is used to switch the SDA line.  It's tested
 with 3 panels, but it shouldn't be a problem to connect more than 8 panels.
 The limiting factor will be the current to drive the SCL line and the number of
 pins to switch the enable signal for the MOS-Fets (can be bypassed with an
 additional Demux: 74HCT238 or CD4514).
 Limitations: Only the active OLED could show a rolling text.

 DCC commands:
 ~~~~~~~~~~~~~
 The displays could be controlled by DCC accessory commands. There are commands to:
 - show the previous/next text message on the actual display
 - change the actual display
 - show the next text message on a special display
 - select a special display as actual display
 - show a special text message
 The number of available DCC commands depends on the number of OLED panels an the number of
 text messages.
 The first used DCC command is defined with the constant FIRST_DCC_ADDR in "Configuration.h"
 It's also possible to limit the last used DCC with LAST_DCC_ADDR .
 For more details see below (search for "Used DCC addressees:")

 More than 100 displays
 ~~~~~~~~~~~~~~~~~~~~~~
 Due to several memory optimizations over 100 displays could be stored in the FLASH of the
 Arduino (Nano/Uno). => There is no need to add an external SD-card to store the messages.
 If more displays are needed an Arduino Mega2560 could be used. This module has 256 KB flash
 instead of 32 KB => Over 2000 displays are possible.


 Serial commands:
 ~~~~~~~~~~~~~~~~
 The displayed texts could be read out of the FLASH or send to the Arduino via the
 serial port (Over USB)
 The program accepts the same serial commands as introduced in the program from Fredddy.
 Some additional commands have been added and a "short" mode has been added.

 Beispiel:
   #LDas ist ein neuer Lauftext#
 definiert einen neuen Lauftext
 Die Felder haben folgende Kuerzel:
    L: Lauftext    (Maximal 100 Zeichen)
    G: Gleisnummer
    W: Wagenstand
    1: Zuglauf 1
    2: Zuglauf 2
    Z: Ziel
    U: Abfahrtszeit
    N: Zugnummer
    X: Loesche Zug Daten (zugnummer, uhrzeit, ziel, zuglauf1, zuglauf2, wagenstand)
    <: Gleis Links
    >: Gleis Rechts
 New commands:
    T: display text message with the given number: #T7 Also possible: #T+  and #T-
    a-z: Select OLED display: #b switch to the second OLED display. The next commands will control this panel.

 Short mode:
 The short mode uses one single character '|' to separate the entries. In this case the sequence
 of the fields must be fix. This mode is also used to store the text messages in the FLASH.
 The short mode could be mixed with the original mode.



 Test fuer serielle Monitor (Short Mode):
 12:53|EC 172|Hamburg - Altona|Berlin Hbf - |Hamburg Hbf|-222211|Verspaetung ca 10 Min

 Normal Mode:
 #U12:53##NEC 172##ZHamburg - Altona##1Berlin Hbf - ##2Hamburg Hbf##W-222211##LVerspaetung ca 10 Min#

 Mixed:
 #1Zielzeile 1|Zielzeile 2

 Mixed Mode mit Gleis:
 #G17#U12:53|EC 172|Hamburg - Altona|Berlin Hbf - |Hamburg Hbf|-222211|Verspaetung ca 10 Min


 Aufbau des Displays:
     123456789012345678901234567
     [<Lauftext>       ] <Abfahrt>
 <G> <zuglauf 1         ><zugNummer>
     <zuglauf 2         >ABCDEFG
     <Ziel              ><Wagenstand>


 Hardware:
 ~~~~~~~~~
 The OLED Display is connected to the following pins: (0.91 Inch 128x32 OLED Display SPI Series SSD1306)
  A4 = SDA
  A5 = SCL
 If multiple OLED displays are used a FET (BS170) is used to switch the SDA line to the OLED displays
 See schematics for more details

 Ansteuerung mehrerer OLEDs
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 - Nur ein OLED kann Lauftext anzeigen
 - Active_OLED bestimmt das OLED welches mit Tastern, DCC Kommandos
   oder serieller Schnittstelle angesprochen wird.
 - Nur das Active_OLED kann einen Lauftext anzeigen dadurch braucht
   man keinen Speicher fuer die anderen OLEDs.
 - Beim umschalten von einem auf das naechste OLED wird der Lauftext
   im alten abgeschaltet
 - Das aktive OLED wird nur bei einer Aenderung neu gezeichnet oder
   wenn der Lauftext aktiviert ist.


 Attention:
 ~~~~~~~~~~
 If the number of used OLED panels is changed during tests sometimes the program doesn't start.
 This may be caused by a hanging OLED display.
 In this case the power supply has to be removed to reset the OLEDs.


 Low memory warning:
 ~~~~~~~~~~~~~~~~~~~
 The warning "Low memory available, stability problems may occur." is generated if less than 512 bytes
 RAM are available. This is a safety warning because the compiler can't calculate the usage of dynamically
 allocated memory and the maximal used memory for the stack.

 German: "Wenig Arbeitsspeicher verfuegbar, es koennen Stabilitaetsprobleme auftreten."

Alte Lib:
 Der Sketch verwendet 20824 Bytes (67%) des Programmspeicherplatzes. Das Maximum sind 30720 Bytes.
 Globale Variablen verwenden 1500 Bytes (73%) des dynamischen Speichers, 548 Bytes fuer lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.


 ToDo:
 ~~~~~
 - Einstellungen fuer Upload in Forum:
   - _PRINT_DEBUG_MESSAGES 0
   - Nur ein OLED
   - Pruefen wie viele Text Messages moeglich sind  (03.02.19:  >130)
   - Deutsche Umlaute ersetzen
   - Schnellen Grafik Treiber aktivieren
   - ProcState_Outside_Disp() deaktivieren wenn alles getestet ist

 - RAM sparen:
   - 20 Leerzeichen am Anfang beim Lauftexts. Das wuerde auch die Display Updatezeit veringern
 - Abfahrtszeit aus Moba Uhr bestimmen
 - Ausgabe wird schneller wenn Leerzeichen uebersprungen werden
   das sieht man mit dem Ossi bei der Laufschift. Es koennten vermutlich
   6 ms von 60 ms gespart werden
 - Programm aufraeumen (Verstreute Konstanten, Variablen, ...)
 - Gleis Nummer Zentriert darstellen
 - Weitere DCC Befehle
   - Uhrzeit relativ zur Modelbahnzeit
 - CAN Bus

 Notizen:
 ~~~~~~~~
 - Das Display gibt es auch ohne Platine: https://www.aliexpress.com/item/0-91-Inch-128x32-OLED-LCD-White-Display-Module-SPI-Series-SSD1306/32859420903.html?spm=2114.search0104.3.124.9ae71fd8F6M06v&ws_ab_test=searchweb0_0,searchweb201602_3_10320_10065_10068_10547_319_317_10548_10696_453_10084_454_10083_433_10618_431_10304_10307_10820_10821_537_10302_536_10902_10843_10059_10884_10887_100031_10319_321_322_10103,searchweb201603_6,ppcSwitch_0&algo_expid=d3e03323-63e4-4399-984e-81f3e7e31fa0-17&algo_pvid=d3e03323-63e4-4399-984e-81f3e7e31fa0&transAbTest=ae803_3
   2W64284020BC 8
   - Pin Namen und Abmessungen: https://www.aliexpress.com/item/0-91-0-91inch-White-OLED-Display-Module-OLED-Screen-Board-128x32-SSD1306/32787123531.html?spm=2114.10010108.1000014.4.33661312rWuNvP&gps-id=pcDetailBottomMoreOtherSeller&scm=1007.13338.80878.000000000000000&scm_id=1007.13338.80878.000000000000000&scm-url=1007.13338.80878.000000000000000&pvid=989eede7-35d8-466c-890d-cebdab413e79
   - Chip: SSD1306
   - Datenblatt (S.63 Anchluss): "..\SSD1306.pdf"
 - Bei der Original Version muss ein Befehl zum lesen von Texten immer mit einem '#' beginnen
   und mit einem # beendet werden.
      #LLauftext#
   Zeichen ohne einleitenden '#' werden ignoriert.
   - Bei den Kommandos '<', '>' und 'X' darf aber kein # kommen
   - Das '#' Zeichen kann nicht im Text verwendet werden


 Revision History:
 ~~~~~~~~~~~~~~~~~
 24.01.19:  - Started
 28.01.19:  - Adapted to the new library u8g2lib
 29.01.19:  - Added font u8g2_font_tpss_tf from the old library
 30.01.19:  - Solved starnge crash which was caused by missing u8g.setDrawColor(1) call
              Unfortunately the drawBox() of the old lib function still generates crash.
              There is no setDrawColor() in the old Lib
              08.02.19: Crash was caused by the wrong ProcState accessing Disp[ProcState]
            - DCC is working, but loosing messages ;-(
 01.02.19:  - Displays are only updated if necessary
            - Support for several OLED panels added
 02.02.19:  - Changed the DCC receive function. Now every signal triggers the action and
              not only a 0->1 edge. Now lost DCC signals are not a big problem.
            - Rail number and side is stored in individual arrays.
            - Commands to switch between the OLED panels by serial port and DCC added.
 03.02.19:  - Using special german fonts to save memory
 05.02.19:  - Saving RAM by not using an own buffer in Process_Character()
            - Found strange error which overwrites Active_OLED !!!
              Wrong value of ProcState in the following line:
                memset(ProcChrPtr, 0, PB(&Disp[ProcState].size));
              in function Short_Mode_Next_ProcState() caused memory corruption ;-(
 06.02.19:  - Moving Rail_Cfg[] to PROGMEM to save RAM
 07.02.19:  - Moving Disp[] to PROGMEM to save RAM
 09.02.19:  - New commands to change the OLED and the displayed text
            - Random changing displays if RAND_CHANGE_MINTIME is enabled
*/


#if 0  // Change to 1 to compile the old program for tests
 #include "Old_Bahnsteiganzeige_73.h"
/*
   Der Sketch verwendet 18146 Bytes (59%) des Programmspeicherplatzes. Das Maximum sind 30720 Bytes.
   Globale Variablen verwenden 1365 Bytes (66%) des dynamischen Speichers, 683 Bytes fuer lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.

 Zugzielanzeiger_68.ino: (7 Anzeigen)
   Der Sketch verwendet 16200 Bytes (52%) des Programmspeicherplatzes. Das Maximum sind 30720 Bytes.
   Globale Variablen verwenden 1604 Bytes (78%) des dynamischen Speichers, 444 Bytes fuer lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.
   Wenig Arbeitsspeicher verfuegbar, es koennen Stabilitaetsprobleme auftreten.

*/
#else // New program
/* 09.02.19:
   Der Sketch verwendet 20782 Bytes (67%) des Programmspeicherplatzes. Das Maximum sind 30720 Bytes.
   Globale Variablen verwenden 1500 Bytes (73%) des dynamischen Speichers, 548 Bytes fuer lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.

  Big memory consumers: (Get_MemUsage.bat (must be adapted to the location of Bahnsteiganzeige_V3.ino.elf))
    1156 t u8g2_font_6x13B_t_german
    1122 t u8g2_font_tpss_t_german
    0952 t u8g2_font_5x8_t_german
    0792 t u8g2_font_4x6_t_german
   =====
    4022   Total = 13 %
*/

#define OLED_COUNT (sizeof(Rail_Cfg)/sizeof(Rail_Cfg_T))

typedef struct
  {
  uint8_t OLED_Enable_Pin;
  char    RailNr[4];
  uint8_t RailSide;
  } Rail_Cfg_T;


#include "Configuration.h" // This file contains the configuration of the program (Pin numbers, Options, ...)

#include "Text_Messages.h" // This file contains the messages for the displays


#if USE_U8G2LIB         // Porting to new lib: https://github.com/olikraus/u8g2/wiki/u8gvsu8g2
  #include <U8g2lib.h>
  #include "4x6_t_german.h"   // Special fonts which have only the german "umlauts" instead of all
  #include "5x8_t_german.h"   // extendet characters (>127). Use "4x6_tf.h" if you need all ANSI characters >127
  #include "6x13B_t_german.h"
  #include "tpss_t_german.h"
  #define FONT_4x6      u8g2_font_4x6_t_german
  #define FONT_5x8      u8g2_font_5x8_t_german
  #define FONT_6x13B    u8g2_font_6x13B_t_german
  #define FONT_PS_11X17 u8g2_font_tpss_t_german
#else
  #include <U8glib.h>    // Alte Library
  #define FONT_4x6      u8g_font_4x6
  #define FONT_5x8      u8g_font_5x8
  #define FONT_6x13B    u8g_font_6x13B
  #define FONT_PS_11X17 u8g_font_tpss        // 11x17  A = 9
#endif

#include "Dprintf.h"   // Is enabled with _PRINT_DEBUG_MESSAGES  Memory usage: FLASH:1844   27.01.19:
                       // Attention: If to much debug messages are used the receiving of serial characters
                       //            may lose characters because of the limmited size of the input and
                       //            output buffer of the serial comunication
                       //            It's also a problem if the baud rate is to high. In this case the
                       //            received characters can't be processed fast enough.

#ifdef LED_HEARTBEAT_PIN
  #include "LED_HeartBeat.h"
  LED_Heartbeat_C LED_HeartBeat(LED_HEARTBEAT_PIN); // Initialize the heartbeat LED which is flashing if the program runs. (FLASH usage: 116 Byte 02.02.19:  )
#endif

#if USE_DCC
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wunused-variable"     // Disable the warning: warning: 'EEPROM' defined but not used [-Wunused-variable]
  #include <NmraDcc.h>
  NmraDcc  Dcc;        // Instance of the NmraDcc class
  // #pragma GCC diagnostic pop                          // Unfortunately I can't turn on the warning again for the following code for some reasons ;-(
  // #pragma GCC diagnostic warning "-Wunused-variable"  // Is also enabling the EEPROM warning
#endif


#if USE_U8G2LIB
  // Three possible display drivers. The differ in the used RAM. The first one uses a full frame buffer
  // => All data for the display fit into the RAM => The content for the display has to be drawn only once
  // The other drivers use less RAM => The display has to be drawn several times and written partly to the OLED
  // If several parts are used the OLED update time is longer ;-(

  // Attention: Problems with the arduino avr core library version 1.8.3                                      // 03.09.20:
  //            There are some time out functions added to the wire part which generates compiler errors:
  //              error: no 'void TwoWire::setWireTimeout(uint32_t, bool)
  //              error: no 'bool TwoWire::getWireTimeoutFlag()
  //              error: no 'void TwoWire::clearWireTimeoutFlag()
  //            It's working with 1.8.2

  #if 1 // 0.87" Display (New U8g2 library required 2.27.6)                                                   // 03.09.20:
      U8G2_SSD1316_128X32_F_HW_I2C u8g(U8G2_R2, U8X8_PIN_NONE); // Für 0.87" Display full frame buffer
    //U8G2_SSD1316_128X32_F_HW_I2C u8g(U8G2_R0, U8X8_PIN_NONE); // Für 0.87" Display full frame buffer Rotate by 180 Deg
    //U8G2_SSD1316_128X32_1_HW_I2C u8g(U8G2_R2, U8X8_PIN_NONE); // Für 0.87" Display
  #else // 0.91" Display
      // Rotate by 180 Deg: U8G2_R2 (Normal: U8G2_R0)    See: https://github.com/olikraus/u8g2/wiki/u8g2setupc
      U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g(U8G2_R2, U8X8_PIN_NONE); // 55-60 ms update time, full frame buffer:       512 bytes RAM
    //U8G2_SSD1306_128X32_UNIVISION_2_HW_I2C u8g(U8G2_R2, U8X8_PIN_NONE); // 65-74 ms update time, 2 pages => 2* nextPage() 256 bytes RAM
    //U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g(U8G2_R2, U8X8_PIN_NONE); // 88-94 ms update time, 4 pages => 4* nextPage() 128 bytes RAM
  #endif
#else // Old library
  U8GLIB_SSD1306_128X32_2X u8g(U8G_I2C_OPT_NONE); // 2 pages  256 bytes RAM
#endif

#include "Push_Button.h"

Push_Button Button0(BUTTON0_PIN);
Push_Button Button1(BUTTON1_PIN);
Push_Button Button2(BUTTON2_PIN);
Push_Button Button3(BUTTON3_PIN);
Push_Button Button4(BUTTON4_PIN);

#define GleisSeite_Links    0      // An enum uses 2 byte RAM and this uses more byte in the code also ;-(
#define GleisSeite_Rechts   1      // Without enum we save 1 byte RAM and 18 bytes FLASH !

bool    gleisSeite = GleisSeite_Links; // defines the position of the rail number. Links = Rail number on the left side
uint8_t offset     = 0;  // Rolling text
uint8_t subset     = 0;

typedef struct
    {
    uint8_t ColorIndex    :1; // 1= Normal, 0 = Invers
    uint8_t Rightaligned  :1; // Text is right alligned if gleisSeite = Rechts
    uint8_t DynInvLen     :1; // Invers box has only the length of the text
    uint8_t NoFontDescent :1; // Pixel below baseline (in 'g') not shown in invers mode. Text shifted down
    uint8_t DelMark       :1; // Delete this entry with the 'X' command
    uint8_t DispIfPrivious:1; // Display only if the privious line is also shown
    } Flags_T;

typedef struct
    {
    uint8_t        xR, xL, y; // x position if gleisSeite is R or L, y position
    uint8_t        PixWidth;  // output length in pixels
    uint8_t        RolTextLen;// Visible characters of the rolling text
    Flags_T        Flags;     // single bits (see above)
    const uint8_t *font;      // Pointer to the used font
    char           ActivChar; // Char to activate the field when entered by the serial port or const. text
    uint8_t        size;      // Length of the Txt
    } Disp_T;

// Numbers of the Disp[] entries
#define LAUFTEXT     0
#define GLEIS        1
#define UHRZEIT      2
#define ZUGNUMMER    3
#define ZIEL         4
#define ZUGLAUF1     5
#define ZUGLAUF2     6
#define WAGENSTAND   7
#define ABSCHNITT    8

char lauftext[121];
char gleis[4]     ;
char uhrzeit[6]   ;
char zugnummer[8] ;
char ziel[17]     ;
char zuglauf1[21] ;
char zuglauf2[21] ;
char wagenstand[8];
char abschnitt[8] = "ABCDEFG"; // This default value doesn't need additional RAM


#define RT_SHIFT 0  // In the original code the "lauftext" is shifted down => Letters like 'g' are cut of
                    // If this is a wanted "feature" set the the constant to 1 otherwise use 0
                    // This feature uses 30 additional bytes of flash. It does't matter if the NoFontDescent
                    // variable is removed from the code or not if RT_SHIFT is set to 0. The compiler
                    // is optimizing this

#define DEFVAR(var) sizeof(var)

// Attention: At the moment only one rolling text is possible ! Problem: offset must be individuel for each string

// Changed layout ("ziel" is left alligned if "GleisSeite_Rechts" is active. Letters like 'g' are shown with descent pixel)
const PROGMEM Disp_T Disp[] =
  {// xR   xL  y   Pix  RolT. Color Right DynInv NoFont  Del  DispIf font            Activ Define variable length
   //             Width Len   Index alig. Len    Descent Mark Privi.                 Char
    { 30,  20,  5,  78, 20, { 0,    0,    0,   RT_SHIFT, 0,   0 },   FONT_4x6,       'L',  DEFVAR(lauftext  ) }, // lauftext  Should be the first entry because it overwrites the areas on the left and right side with Black pixels
    {128,   0, 13,  18,  0, { 1,    1,    0,     0,      0,   0 },   FONT_6x13B,     'G',  DEFVAR(gleis     ) }, // gleis
    {  0, 100,  6,  25,  0, { 1,    0,    0,     0,      1,   0 },   FONT_5x8,       'U',  DEFVAR(uhrzeit   ) }, // uhrzeit
    {  0, 100, 12,  20,  0, { 1,    0,    0,     0,      1,   0 },   FONT_4x6,       'N',  DEFVAR(zugnummer ) }, // zugnummer
    { 30,   0, 28, 176,  0, { 1,    0,    0,     0,      1,   0 },   FONT_PS_11X17,  'Z',  DEFVAR(ziel      ) }, // ziel      proportional font 11x17 ? https://github.com/olikraus/u8glib/wiki/fontgrouporgdot
    { 30,  20, 11,  80,  0, { 1,    0,    0,     0,      1,   0 },   FONT_4x6,       '1',  DEFVAR(zuglauf1  ) }, // zuglauf1
    { 30,  20, 17,  80,  0, { 1,    0,    0,     0,      1,   0 },   FONT_4x6,       '2',  DEFVAR(zuglauf2  ) }, // zuglauf2
    {  0, 100, 26,  28,  0, { 0,    0,    1,     0,      1,   0 },   FONT_4x6,       'W',  DEFVAR(wagenstand) }, // wagenstand
    {  0, 100, 20,  28,  0, { 1,    0,    0,     0,      0,   1 },   FONT_4x6,       'A',  DEFVAR(abschnitt ) }, // haltepositionen ABCDEFG
  };

char *Disp_Txt[sizeof(Disp)/sizeof(Disp_T)] =  // Attention the sequence must be equal with the definition above !
    {
    lauftext,
    gleis,
    uhrzeit,
    zugnummer,
    ziel,
    zuglauf1,
    zuglauf2,
    wagenstand,
    abschnitt
    };

#define PB(x) pgm_read_byte_near(x)
#define PW(x) pgm_read_word_near(x)

// Constants for the UpdateDisplay variable:
#define DONT_UPD_DISP  0
#define UPD_DISP_ONCE  1  // Update the display once. It has a rolling text UpdateDisplay is set to UPD_DISP_ROLL oterwise its set to DONT_UPD_DISP
#define UPD_DISP_ROLL  2  // Display is updated preiodic because a rolling display is active
#define UPD_DISP_STOP  3  // Stop the rolling display


// Constants for DispNr_Req:
#define DNR_PRIV       (255 - 4 - (2 * OLED_COUNT))
#define DNR_NEXT       (DNR_PRIV      +1)
#define DNR_PRIV_OLED  (DNR_NEXT      +1)
#define DNR_NEXT_OLED  (DNR_PRIV_OLED +1)
#define DNR_OLED0_NEXT (DNR_NEXT_OLED +1)
#define DNR_SEL_OLED0  (DNR_OLED0_NEXT+OLED_COUNT)
#define DNR_NOTHING    255

volatile uint8_t DispNr_Req   = DNR_NOTHING;
volatile uint8_t Active_OLED  = 0;  // Number of the active OLED panel
uint8_t UpdateDisplay         = UPD_DISP_ONCE;
uint8_t DispNr                = 255;
uint8_t Last_DispNr           = 255;  // If Find_MsgNr() id called with an not existing number this variable contains the last available DispNr


//----------------------------------------------------------
void Draw_Element(const Disp_T &Disp, Flags_T &F, char *Txt)
//----------------------------------------------------------
// This function draws one element.
// It's called in a loop to draw all
// elements.
{
  //Dprintf("'%s'\n", Txt); // Debug
  u8g.setFont(PW(&Disp.font));

  uint8_t x;
  if (gleisSeite == GleisSeite_Rechts)
       {
       x = PB(&Disp.xR);
       if (F.Rightaligned) x -= u8g.getStrWidth(Txt);
       }
  else x = PB(&Disp.xL);

  uint8_t y = PB(&Disp.y);

  if (F.ColorIndex == 0) // Invers => draw a box
     {
     u8g.setColorIndex(1);

     if (*Txt != '\0' && (PB(&Disp.RolTextLen) == 0 || UpdateDisplay != UPD_DISP_STOP)) // Don't draw box if text is empty or RolTextLen enabled and UPD_DISP_STOP
        {
        uint8_t PixWidth;
        if (F.DynInvLen)
             PixWidth = u8g.getStrWidth(Txt);
        else PixWidth = PB(&Disp.PixWidth);
        u8g.drawBox(x, y - u8g.getFontAscent(), PixWidth, u8g.getFontAscent()+u8g.getFontDescent()+2);
        }

     #if RT_SHIFT
       if (F.NoFontDescent) y+= (u8g.getFontDescent()+2); // Was this a wanted "feature" in the original code ?
     #endif
     }

  u8g.setColorIndex(F.ColorIndex);


  if (PB(&Disp.RolTextLen) && *Txt && UpdateDisplay != UPD_DISP_STOP) // rolling text used ?
       {
       uint8_t Len = strlen(Txt);
       if (offset > Len) offset = 0;
       int remaining = Len - offset;
       if (remaining > PB(&Disp.RolTextLen)) remaining = PB(&Disp.RolTextLen);
       if (offset < Len)
          {
          char Old;
          uint8_t End = offset+remaining;
          if (End < Len)
             {
             Old = Txt[End];
             Txt[End] = '\0';
             }
          u8g.drawStr(x - subset, y, Txt+offset);
          if (End < Len) Txt[End] = Old;
          }
       }
  else u8g.drawStr(x, y, Txt); // Normal text
}

//----------------------
void Draw_All_Elements()
//----------------------
{
  for (uint8_t i = 0; i < sizeof(Disp)/sizeof(Disp_T); i++)
    {
    union
        {
        Flags_T F;
        uint8_t F_byte;
        };
    F_byte = PB(&Disp[i].Flags);
    if (i == 0 || F.DispIfPrivious == 0 || *Disp_Txt[i-1] != '\0') // Some lines are only shown if the privious line is also shown
       Draw_Element(Disp[i], F, Disp_Txt[i]);
    }
}

//-------------------------------
uint8_t Display_has_Roling_Text()
//-------------------------------
{
  for (uint8_t i = 0; i < sizeof(Disp)/sizeof(Disp_T); i++)
      if (PB(&Disp[i].RolTextLen) && *Disp_Txt[i]) return i+1;
  return 0;
}

//--------------------------
void Write_Display_to_OLED()
//--------------------------
{
  u8g.firstPage();
  do {
     Draw_All_Elements();
     } while( u8g.nextPage() );

  if (Display_has_Roling_Text())
       UpdateDisplay = UPD_DISP_ONCE;
  else UpdateDisplay = DONT_UPD_DISP; // Don't update the display any more
}

#define PC_READ_TM_NR   250           // Read the index number of the text message to be displayed
#define PC_WAIT_START   251
#define PC_READ_TYP     252
#define PC_WAIT_END     253
#define PC_UNUSED       254
#define PC_ERROR        255
#define PC_UNKNOWN_CHAR PC_READ_TYP   // Used only temporary in Process_Character()

#define START_CHAR      '#'
#define END_CHAR        '#'
#define EXT_END_CHARS   "#\r\n" NE  // const char[] benoetigt genau so viel Speicher

uint8_t ProcState = PC_WAIT_START;

char *ProcChrPtr;
char ShortBuf[4];

//---------------------------------
void Debug_Print_State_if_Changed() // Debug
//---------------------------------
// Print the states for debugging
{
#if _PRINT_DEBUG_MESSAGES && 0   // use "&& 0" to disable, "&& 1" to enable the debug display
  static uint8_t Old_State = PC_UNUSED;
  if (Old_State != ProcState)
     {
     Old_State = ProcState;
     Dprintf("S: ");
     switch (ProcState)
        {
        case PC_WAIT_START: Dprintf("PC_WAIT_START\n"); break;
        case PC_READ_TYP:   Dprintf("PC_READ_TYP\n");   break;
        case PC_WAIT_END:   Dprintf("PC_WAIT_END\n");   break;
        case PC_READ_TM_NR: Dprintf("PC_READ_TM_NR\n"); break;
        default:            Dprintf("Read Disp %i\n", ProcState);
        }
     }
#endif
}

//--------------------------------------------------------
uint8_t Find_Activation_Character_in_Display_Array(char c)
//--------------------------------------------------------
{
  for (uint8_t i = 0; i < sizeof(Disp)/sizeof(Disp_T); i++)
    {
    if (PB(&Disp[i].ActivChar) == c) return i;
    }
  return PC_UNKNOWN_CHAR;
}

//-----------------
void DeleteMarked()
//-----------------
// Delete the strings marked with an DelMark in the Disp[] array
{
  for (uint8_t i = 0; i < sizeof(Disp)/sizeof(Disp_T); i++)
    {
    union
        {
        Flags_T F;
        uint8_t F_byte;
        };
    F_byte = PB(&Disp[i].Flags);
    if (F.DelMark) *Disp_Txt[i] = '\0';
    }
}

//--------------------------------
uint8_t Check_Special_Char(char c)
//--------------------------------
{
  switch (c)
    {
    case '<'       : gleisSeite = GleisSeite_Links;  break;
    case '>'       : gleisSeite = GleisSeite_Rechts; break;
    case 'X'       : DeleteMarked();                 break;       // Clear all lines in Disp[] which are marked with the DelMark flag
    case START_CHAR: return PC_READ_TYP;             break;       // to allow multible start characters
    case 'T'       : // Read the number of the text message to be displayed
                     ProcChrPtr = ShortBuf;
                     memset(ShortBuf,  0, sizeof(ShortBuf));
                     return PC_READ_TM_NR;
    default        : if ( c>= 'a' and c < (char)('a'+OLED_COUNT)) // a..z switch to an other OLED
                          {
                          Activate_OLED(c-'a');
                          return PC_WAIT_START; // return without updating the display
                          }
                     else Dprintf("Unknown command:'%c' 0x%02X\n", c, c); // Debug
    }
  UpdateDisplay = UPD_DISP_ONCE;
  return PC_WAIT_START;
}

#define PCM_NORMAL  0
#define PCM_SHORT   1
uint8_t PC_Mode = PCM_NORMAL;

#define START_DISP_NUMMER UHRZEIT  // The first Entry in Text_Messages[] is the time


#if 1 // Set to 1 for debugging
//------------------------------------------------
uint8_t ProcState_Outside_Disp(uint8_t CheckPoint)  // 294 Byte FLASH
//------------------------------------------------
// Debug function to detect errors of the variable ProcState
{
  if (ProcState >= sizeof(Disp)/sizeof(Disp_T))
     {
     Serial.print(F("ERROR Wrong ProcState:"));
     Serial.print(ProcState);
     Serial.print(" CP:");
     Serial.println(CheckPoint);
     return 1;
     }
  return 0;
}
#else
  #define ProcState_Outside_Disp(Nr) 0
#endif

//------------------------------
void Short_Mode_Next_ProcState()
//------------------------------
{
  //Dprintf("Next_ProcState %i\n", ProcState); // Debug
  PC_Mode = PCM_SHORT;
  switch (ProcState)
    {
    case PC_WAIT_START: ProcState = START_DISP_NUMMER; break;
    case WAGENSTAND:    ProcState = LAUFTEXT;          break;
    case LAUFTEXT:      ProcState = PC_WAIT_START;     return; // 06.02.19:  Old: break;   FEHLER! Hier muss rausgesprungen werden werden sonst wird unten eine Falsche Groesse verwendet Disp[252] !!!
    default:            ProcState++;
                        if (ProcState >= sizeof(Disp)/sizeof(Disp_T)) // Overflow ?
                           {
                           ProcState = PC_WAIT_START;
                           return ;
                           }
    }

  if (ProcState_Outside_Disp(1)) return; // Savety check

  ProcChrPtr = Disp_Txt[ProcState];
  memset(ProcChrPtr, 0, PB(&Disp[ProcState].size));
}

//------------------------------------------
void Change_Disp_from_Str(const char *NrStr)
//------------------------------------------
{
  //Dprintf("Change_Disp_from_Str %s\n", NrStr); // Debug
  switch (*NrStr)
    {
    case '+': DispNr_Req = DNR_NEXT; return;
    case '-': DispNr_Req = DNR_PRIV; return;
    }
  int Nr = atoi(NrStr);  // sscanf() uses a lot of memory! if atoi() is used we save 1778 bytes !
  if (Nr >= 0) DispNr_Req = (uint8_t)Nr;
}

//----------------------------
void Process_Character(char c)
//----------------------------
{
  Debug_Print_State_if_Changed();  // Debug
  switch (ProcState)
    {
    case PC_WAIT_START: if (c == START_CHAR)      { PC_Mode = PCM_NORMAL; ProcState = PC_READ_TYP;}
                        if (c == START_LINE_CHAR) Short_Mode_Next_ProcState();
                        break;
    case PC_READ_TYP:   ProcState = Find_Activation_Character_in_Display_Array(c);
                        if (ProcState != PC_UNKNOWN_CHAR)
                             {
                             if (ProcState_Outside_Disp(2)) { ProcState = PC_ERROR; return;} // Savety check
                             ProcChrPtr = Disp_Txt[ProcState];
                             memset(ProcChrPtr, 0, PB(&Disp[ProcState].size));
                             }
                        else ProcState = Check_Special_Char(c);
                        break;
    case PC_WAIT_END:   if (strchr(EXT_END_CHARS, c))
                             {
                             if (c == END_CHAR)
                                  ProcState = PC_READ_TYP;   // The # could also be the next start character
                             else ProcState = PC_WAIT_START;
                             }
                        break;
    case PC_ERROR:      break;
    default:            // Reading the characters into a local temporary variable
                        uint8_t MaxLen;
                        if (ProcState == PC_READ_TM_NR)
                             MaxLen = sizeof(ShortBuf)-1;
                        else {
                             if (ProcState_Outside_Disp(3)) { ProcState = PC_ERROR; return;} // Savety check
                             MaxLen = PB(&Disp[ProcState].size) - 1 - PB(&Disp[ProcState].RolTextLen);
                             }
                        uint8_t ActLen = strlen(ProcChrPtr);
                        if (ActLen >= MaxLen || strchr(EXT_END_CHARS, c))
                             { // End of the text detected
                             //Dprintf("Disp[%i]='%s'\n", ProcState, ProcChrPtr); // Debug
                             if (ProcState == PC_READ_TM_NR)
                                  {
                                  Change_Disp_from_Str(ShortBuf);
                                  ProcState = PC_WAIT_START;
                                  }
                             else {
                                  if (ProcState_Outside_Disp(4)) { ProcState = PC_ERROR; return;}; // Savety check
                                  uint8_t RolTextLen = PB(&Disp[ProcState].RolTextLen);
                                  if (RolTextLen > 0 && ActLen > 0)    // Special treatement for rolling text
                                       {
                                       memmove(ProcChrPtr+RolTextLen, ProcChrPtr, ActLen);
                                       memset(ProcChrPtr, ' ', RolTextLen); // add leading space characters
                                       offset = subset = 0; // new start of the rolling text
                                       }
                                  switch (c)
                                     {
                                     case END_CHAR:        ProcState = PC_READ_TYP; break; // The end could be the next start character
                                     case NEXT_ENTRY_CHAR: Short_Mode_Next_ProcState();
                                                           break;
                                     case '\r': case '\n': ProcState = PC_WAIT_START; break;
                                     default:              Dprintf("End read Char c=%c (%02X)\n", c, c); // Debug
                                                           ProcState = PC_WAIT_END;
                                     }
                                  }
                             UpdateDisplay = UPD_DISP_ONCE;
                             }
                        else ProcChrPtr[ActLen] = c;
                        //Dprintf("ProcChrPtr = '%s' ActLen %i\n", ProcChrPtr, ActLen);  // Debug
    }
  Debug_Print_State_if_Changed();  // Debug
}

//--------------------------------
void Set_Rail_Defaults(uint8_t Nr)
//--------------------------------
{
  strcpy_P(Disp_Txt[GLEIS], Rail_Cfg[Nr].RailNr);
  gleisSeite = (PB(&Rail_Cfg[Nr].RailSide) == 'L') ? GleisSeite_Links : GleisSeite_Rechts;
}

//---------------------------------------------------
void Activate_Display_from_Flash_Sub(const char *Msg)
//---------------------------------------------------
// Activate the display by a text message located in the flash.
// The message contains special characters which define the
// destination in Disp[].
{
  Set_Rail_Defaults(Active_OLED);    // The rail number is defined by the array Rail_Cfg[Active_OLED].RailNr, but this could be replaced in the text
  char c = pgm_read_byte_near(Msg);  //  message with the #G<Nr> command. RailSide is replaced with #< or #>.
  do
    {
    Process_Character(c);
    c = pgm_read_byte_near(++Msg);
    } while (c != START_LINE_CHAR && c != '\0');
  Process_Character(NEXT_ENTRY_CHAR);
}


//--------------------------------
const char *Find_MsgNr(uint8_t Nr)
//--------------------------------
{
  //Dprintf("Find_MsgNr %i\n", Nr); // Debug to check the search duration
  //uint32_t Start = millis();      // Debug
  const char *Msg = Text_Messages;
  char c = pgm_read_byte_near(Msg);
  Last_DispNr = 255;
  do
    {
    if (c == START_LINE_CHAR)
       {
       if (Nr-- == 0) return Msg;
       Last_DispNr++;
       }
    c = pgm_read_byte_near(++Msg);
    } while (c != '\0');
  // Dprintf("Search time %lims\n", millis() - Start); // Searching the entry 150 took 8 ms :-)  // Debug
  return NULL;
}

//---------------------------------------------------
uint8_t Activate_Display_from_Flash_by_Nr(uint8_t Nr)
//---------------------------------------------------
{
  //Dprintf("Activate_Display %i\n", Nr);   // Debug
  const char *Msg = Find_MsgNr(Nr);
  if (Msg) Activate_Display_from_Flash_Sub(Msg);
  return Msg != NULL;
}


//-------------------------------------
void Activate_Next_Display_from_Flash()
//-------------------------------------
{
  if (!Activate_Display_from_Flash_by_Nr(++DispNr))
      Activate_Display_from_Flash_by_Nr((DispNr = 0)); // Not found => activate display 0
}

//-------------------------------------
void Activate_Priv_Display_from_Flash()
//-------------------------------------
{
  if (!Activate_Display_from_Flash_by_Nr(--DispNr))
      Activate_Display_from_Flash_by_Nr((DispNr = Last_DispNr)); // Not found => activate the last display
}

//------------------------------
void Enable_OLED_Pin(uint8_t Nr)
//------------------------------
// Activates the enable pin for one OLED.
// The enable pin drives a FET (BS170) which
// switches the SDA line to the OLED display.
{
  for (uint8_t i = 0; i < OLED_COUNT; i++)
      digitalWrite(PB(&Rail_Cfg[i].OLED_Enable_Pin), 0);
  digitalWrite(PB(&Rail_Cfg[Nr].OLED_Enable_Pin), 1);
  Active_OLED = Nr;
}

//----------------------------
void Activate_OLED(uint8_t Nr)
//----------------------------
{
  uint8_t res;
  if ((res = Display_has_Roling_Text()))
     {
     *Disp_Txt[res-1] = '\0';       // Clear the rolling text because otherwise the next display is overwritten when switched trough the OLEDs
     Write_Display_to_OLED();       // Update the display
     }
  UpdateDisplay = DONT_UPD_DISP; // Don't draw to the new display until we have a new content
  if (Nr >= OLED_COUNT) Nr = 0;
  Enable_OLED_Pin(Nr);
  Dprintf("Act OLED %i\n", Nr);
}

//--------------
void Next_OLED()
//--------------
{
  Active_OLED++;
  if (Active_OLED >= OLED_COUNT) Active_OLED = 0;
  Activate_OLED(Active_OLED);
}

//--------------
void Priv_OLED()
//--------------
{
  Active_OLED--;
  if (Active_OLED >= OLED_COUNT) Active_OLED = OLED_COUNT-1;
  Activate_OLED(Active_OLED);
}

//---------------------------------------
void Next_Display_for_OLED_Nr(uint8_t Nr)
//---------------------------------------
{
  Activate_OLED(Nr);
  Activate_Next_Display_from_Flash();
}

//----------------
void Setup_OLEDs()
//----------------
{
  pinMode(RESET_DISP_PIN, OUTPUT);                                                                            // 02.10.20:
  delay(10);
  digitalWrite(RESET_DISP_PIN, 1);

  for (uint8_t i = 0; i < OLED_COUNT; i++)  // Configure all pins as output
    pinMode(PB(&Rail_Cfg[i].OLED_Enable_Pin), OUTPUT);
  for (uint8_t i = OLED_COUNT-1; i != 255; i--) // Initialize the OLEDs in reverse order to end with the first display
      {
      Enable_OLED_Pin(i);
      u8g.begin();
      if (i == OLED_COUNT-1)
         {
         u8g.setContrast(150);     // Kontrast Anpassen                                                       // 23.01.19:  Copied from Zugzielanzeiger_71.ino

         #if USE_U8G2LIB
            u8g.setFontMode(1);    // Fonts in the new library draw also the background pixels => The invers background is visible on the left side
         #else                     // The display rotation is defined in the constructor in the new version
           u8g.setRot180();        // Anzeige 180 grad rotieren weil dann der breitere,
                                   // nicht beschreibbare Rand des Displays oben ist (Unter den Bahnhofsdach)
         #endif
         }
      if (i > 0) UpdateDisplay = UPD_DISP_STOP; // don't draw the rolling text

      Activate_Display_from_Flash_by_Nr(i);
      Set_Rail_Defaults(i);
      Write_Display_to_OLED();
      }
  DispNr = OLED_COUNT-1;
}


//************************************************** DCC *****************************************************


#if USE_DCC
  // Used DCC adresses:
  // ~~~~~~~~~~~~~~~~~~
  //                     Address if FIRST_DCC_ADDR = 5 and three OLED panels are used:
  //                     ~~~~~~
  // First DCC address:   (5)   Change the displayed text of the active OLED
  //                              Red   = previous text message
  //                              Green = next   "
  //
  // Second DCC address:  (6)   Change the active OLED panel (Only if more than one OLEDs are used)
  //                              Red   = previous OLED panel
  //                              Green = next   "
  //
  // Next n DCC addresses:      Show the next text message on a certain OLED panel (Only if more than one OLEDs are used)
  //                            n = Number of OLED displays / 2
  //                      (7)     Red   1 = show next text message on OLED 1
  //                      (7)     Green 1 =  "          "           "      2
  //                      (8)     Red   2 =  "          "           "      3
  //                      (-)     Green 2 =  "          "           "      4   (Invalid if 3 OLEDs are used => Selects OLED panel 0)
  //                      (-)      :         :          :           :      :
  //                      (-)     Green n =  "          "           "      n/2
  //
  // Next n DCC addresses:      Select a certain OLED panel (Only if more than one OLEDs are used)
  //                      (9)     Red   1 = select OLED panel 1
  //                      (9)     Green 1 =   "     "     "    2
  //                      (10)    Red   2 =   "     "     "    3
  //                      (-)     Green 2 =   "     "     "    4               (Invalid if 3 OLEDs are used => Does nothing)
  //                      (-)      :          :     :     :    :
  //                      (-)     Green n =   "     "     "    n/2
  //
  // Next m DCC addresses:      Show a certain text message on the actual OLED panel
  //                      (11)    Red   1 = show text message 1
  //                      (11)    Green 1 =  "     "    "     2
  //                      (12)    Red   2 =  "     "    "     3
  //                      (12)    Green 2 =  "     "    "     4
  //                       :       :         :     :    :     :
  #define CHANGE_OLED_CMD     (FIRST_DCC_ADDR +     (OLED_COUNT > 1 ? 1 : 0))              // = 6 / 5
  #define FIRST_SEL_NEXT_TXT  (CHANGE_OLED_CMD + 1)                                        // = 7 / 6

  #define FIRST_SEL_OLED_CMD  (FIRST_SEL_NEXT_TXT + (OLED_COUNT > 1 ? (OLED_COUNT+1)/2 : 0))
  //                            7                                     (3         +1)/2        = 9 / 6
  #define FIRST_SEL_TXT_CMD   (FIRST_SEL_OLED_CMD + (OLED_COUNT > 1 ?     (OLED_COUNT+1)/2 : 0))
  //                            9+2 = 11

  //-------------------------------------------------------------------------------------
  void notifyDccAccTurnoutOutput( uint16_t Addr, uint8_t Direction, uint8_t OutputPower )
  //-------------------------------------------------------------------------------------
  // This function is called whenever a normal DCC Turnout Packet is received
  //
  // Unfortunately not all DCC commands are received if the I2C is used to update the OLED display.
  // => We don't allways get the "Button pressed" and "Button released" messages.
  // As a work arround a timeout is used.
  // The first message sets the timeout and triggers the action independant from the "Button" state.
  // The following messages within the timeout are ignored.
  // This methode is used because it's possible that we miss the "Button pressed" message but
  // receive the "Button released" signal. In this case we assume thet the button must be pressed before.
  // The OutputPower parameter is ignored.
  // 6-7 DCC messages from a MS2 are received if a button is pressed and released again. This takes
  // about 300 ms (Depending on the time the button is pressed). Therefor the timeout is set to 500 ms.
  {
    const int DCC_BUTTON_TIMEOUT = 500;
    static uint32_t Timeout = 0;

    if (Addr >= FIRST_DCC_ADDR && Addr <= LAST_DCC_ADDR)
       {
       if (millis() > Timeout)
          {
          if (Direction>0) Direction = 1; // In case some controller sends an other value
          Timeout = millis() + DCC_BUTTON_TIMEOUT;
           // Commands to change the active OLED panel
          if      (Addr == FIRST_DCC_ADDR)     DispNr_Req=DNR_PRIV                                   +Direction; // Change the displayed text of the active OLED (-/+)
          else if (Addr == CHANGE_OLED_CMD)    DispNr_Req=DNR_PRIV_OLED                              +Direction; // Change the active OLED (-/+)
          else if (Addr <  FIRST_SEL_OLED_CMD) DispNr_Req=DNR_OLED0_NEXT+(Addr-FIRST_SEL_NEXT_TXT)*2 +Direction; // Next text on a certain OLED
          else if (Addr <  FIRST_SEL_TXT_CMD)  DispNr_Req=DNR_SEL_OLED0 +(Addr-FIRST_SEL_OLED_CMD)*2 +Direction; // Select a sertain OLED
          else                                 DispNr_Req=               (Addr-FIRST_SEL_TXT_CMD) *2 +Direction;
          Dprintf("notifyDccAccTurnoutOutput: %4i, %i, %i\n", Addr, Direction, DispNr_Req); // Debug
          }
       //Dprintf("*"); // Display received messages. If no rolling text is active 6-7 * are shown otherwise messages get lost => 1-4 * are shown
       }
  }

  #if 0 // Not used at the moment
    //---------------------------------------------------------
    void notifyDccSigOutputState( uint16_t Addr, uint8_t State)
    //---------------------------------------------------------
    // notifyDccSigOutputState() Callback for a signal aspect accessory decoder.
    //                      Defined in S-9.2.1 as the Extended Accessory Decoder Control Packet.
    {
      Dprintf("notifyDccSigState: %4i, %02X\n", Addr, State) ;
    }
  #endif
#endif // USE_DCC


#if defined(RAND_CHANGE_MINTIME) && defined(RAND_CHANGE_MAXTIME)
  //----------------------------
  void Change_Display_ramdomly()
  //----------------------------
  {
    static uint32_t Next_Rand_Change = 0;
    if (Next_Rand_Change == 0)
       randomSeed(analogRead(UNUSED_AIN_PIN));

    if (Next_Rand_Change <= 1) Next_Rand_Change = millis() + random(RAND_CHANGE_MINTIME*1000L, RAND_CHANGE_MAXTIME*1000L);

    if (millis() >= Next_Rand_Change)
       {
       Next_Rand_Change = 1;
       if (OLED_COUNT > 1) Activate_OLED(random(OLED_COUNT));
       Activate_Next_Display_from_Flash();
       }
  }
#endif


//---------------------------------
void Check_Display_Change_Request()
//---------------------------------
// Check the variable DispNr_Req and activate the display
{
  if (ProcState == PC_WAIT_START || ProcState == PC_WAIT_END) // Wait until all characters form the serial input (or a privious command) are processed
     {
     switch (DispNr_Req)
        {
        case DNR_NOTHING: break;
        case DNR_PRIV     : Activate_Priv_Display_from_Flash(); Dprintf("Priv txt %i\n", DispNr); break;
        case DNR_NEXT     : Activate_Next_Display_from_Flash(); Dprintf("Next txt %i\n", DispNr); break;
        case DNR_PRIV_OLED: Priv_OLED();                        Dprintf("Priv_OLED\n");           break;
        case DNR_NEXT_OLED: Next_OLED();                        Dprintf("Next_OLED\n");           break;
        default           : if (DispNr_Req < DNR_OLED0_NEXT)
                               {
                               Dprintf("Txt %i\n", DispNr_Req);
                               if (Activate_Display_from_Flash_by_Nr(DispNr_Req) == 0)
                                  Dprintf("Error DispNr_Req %i doesn't exist\n", DispNr_Req);
                               }
                            else if (DispNr_Req < DNR_SEL_OLED0) Next_Display_for_OLED_Nr(DispNr_Req - DNR_OLED0_NEXT);
                            else Activate_OLED(DispNr_Req - DNR_SEL_OLED0);
        }
     DispNr_Req = DNR_NOTHING;
     }
}


//************************************************************************************************************
void setup()
//************************************************************************************************************
{
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println(F("Bahnsteigsanzeige by Hardi"));

  Setup_OLEDs();            // Calls u8g.begin() for all displays and displays the first screens

  #if USE_U8G2LIB           // According to the documentation this command must be called before begin(), But then it has no effect ;-(
    u8g.setBusClock(400000);// Update time for one page with 128 byte: (Measured with scope).  Driver: U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C
  #endif                    //   400 kHz 5.4 ms = default frequency if line is disabled
                            //   600 kHz 3.9 ms
                            //   700 kHz 3.8 ms
                            //   800 kHz 3.7 ms
                            //   900 kHz      => Display is not working (I2C Signals look o.k. on the skope)
                            //
                            //  Old lib 100 kHz 26 ms (page size 256 byte)

  #if USE_DCC
    Dcc.pin(0, DCC_SIGNAL_PIN, 1); // Setup which External Interrupt, the Pin it's associated with that we're using and enable the Pull-Up

    // Call the main DCC Init function to enable the DCC Receiver
    Dcc.init( MAN_ID_DIY, 10, CV29_ACCESSORY_DECODER | CV29_OUTPUT_ADDRESS_MODE, 0 );  // ToDo: Was bedeuten die Konstanten ?

   // Dprintf("FIRST_SEL_NEXT_TXT:%i\n", FIRST_SEL_NEXT_TXT); // Debug
   // Dprintf("FIRST_SEL_OLED_CMD:%i\n", FIRST_SEL_OLED_CMD);
   // Dprintf("FIRST_SEL_TXT_CMD :%i\n", FIRST_SEL_TXT_CMD );
   // Dprintf("DNR_PRIV          :%i\n", DNR_PRIV          ); // The numbers could be used with the #t<Nr> command
   // Dprintf("DNR_SEL_OLED0     :%i\n", DNR_SEL_OLED0     );
  #endif
}


//************************************************************************************************************
void loop()
//************************************************************************************************************
{
  uint32_t EndWait = millis() + 80; // Screen update period [ms]. Important for the rolling text. (Minimum 55 ms)
                                    // The original program (#73) had an update rate of 95 ms without delay
                                    // If the update rate is too high, the DCC commands are recognized worse

  if (UpdateDisplay != DONT_UPD_DISP)
     {
     Write_Display_to_OLED(); // Update the display and set UpdateDisplay

     // Calculate position for rolling text
     subset++;
     if (subset > 3)
        {
        subset = 0;
        offset++;
        }
     }

  // Use a button to change the displayed message
  if (Button0.Pressed()) Activate_Next_Display_from_Flash();
  if (OLED_COUNT > 1)
      { // No #if possible, but the compiler will optimize this and add only the if or else part demending on OLED_COUNT
      if (Button1.Pressed()) Next_OLED();
      if (Button2.Pressed()) Next_Display_for_OLED_Nr(0);
      if (Button3.Pressed()) Next_Display_for_OLED_Nr(1);
      if (Button4.Pressed()) Next_Display_for_OLED_Nr(2);
      }
  else // Only one OLED panel used
      {
      if (Button1.Pressed()) Activate_Display_from_Flash_by_Nr(0);
      if (Button2.Pressed()) Activate_Display_from_Flash_by_Nr(1);
      if (Button3.Pressed()) Activate_Display_from_Flash_by_Nr(2);
      if (Button4.Pressed()) Activate_Display_from_Flash_by_Nr(3);
      }

  do {
     #if USE_DCC
       Dcc.process(); // You MUST call the NmraDcc.process() method frequently from the Arduino loop() function for correct library operation
     #endif

     // Process serial characters
     while (Serial.available() > 0)
       {
       Process_Character(Serial.read());
       }
     } while (millis() < EndWait);

  Check_Display_Change_Request();

  #if defined(RAND_CHANGE_MINTIME) && defined(RAND_CHANGE_MAXTIME)
    Change_Display_ramdomly();
  #endif

  #ifdef LED_HEARTBEAT_PIN
    LED_HeartBeat.Update();
  #endif
}


/*
 Arduino Nano:        +-----+
         +------------| USB |------------+
         |            +-----+            |
 LED HB  | [ ]D13/SCK        MISO/D12[ ] |
         | [ ]3.3V           MOSI/D11[ ]~|
         | [ ]V.ref     ___    SS/D10[ ]~|   Enable OLED panel 2
         | [ ]A0       / N \       D9[ ]~|   Enable OLED panel 1
         | [ ]A1      /  A  \      D8[ ] |   Enable OLED panel 0
         | [ ]A2      \  N  /      D7[ ] |   Push button4: Next text message for OLED 2
 RandNr  | [ ]A3       \_0_/       D6[ ]~|   Push button3: Next text message for OLED 1
 OLED    | [ ]A4/SDA               D5[ ]~|   Push button2: Next text message for OLED 0
 OLED    | [ ]A5/SCL               D4[ ] |   Push button1: Switch to the next OLED
         | [ ]A6              INT1/D3[ ]~|   Push button0: Activate the next text message from Flash fo the current OLED
         | [ ]A7              INT0/D2[ ] |   DCC Optocopler
         | [ ]5V                  GND[ ] |
         | [ ]RST                 RST[ ] |
         | [ ]GND   5V MOSI GND   TX1[ ] |
         | [ ]Vin   [ ] [ ] [ ]   RX1[ ] |
         |          [ ] [ ] [ ]          |
         |          MISO SCK RST         |
         | NANO-V3                       |
         +-------------------------------+
*/
#endif


