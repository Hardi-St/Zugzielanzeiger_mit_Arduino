#define START_LINE_CHAR '\247' // paragraph symbol
#define NEXT_ENTRY_CHAR '|'
#define SL              "\247" // Start Line
#define NE              "|"    // Next Entry

// Abhaengig vom dem Zeichensatz des Editors werden die Deutschen Umlaute richtig dardestellt oder nicht ;-(
// Es ist es besser wenn hier immer die folgenden Codes anstelle der Umlaute verwendtet werden.
// Dann kommt jeder Editor damit zurecht.
//
// Umlaute ersetzten laut folgender Chiffre. Dazu koennen Hex Codes (0xE4) oder Octal Codes (\344) verwendet werden
// Die oktalen Codes enthalten immer 3 Ziffern. Dadurch koennen diese besser zusammen mit darauf folgenden
// Ziffern verwendet werden.
//   ae  = \344
//   oe  = \366
//   ue  = \374
//   ss  = \337
//   Ae  = \304
//   Oe  = \326
//   Ue  = \334
//   3/4 = \276
// Wuerzburg ist daher W\374rzburg
// Verspaetung ist daher Versp\344tung

#define G934 "#G9 \276|11:00" // Gleis '9 3/4' at '11:00'

// Attention: The sequence of the entries must match with the sequence in the array Dat[]
//            "lauftext" is an exception to this sequence. This is handled in function
//            Short_Mode_Next_ProcState()

const PROGMEM char Text_Messages[] =
//    uhrzeit      zugnummer     ziel                  zuglauf1                  zuglauf2                  wagenstand   lauftext  1         2         3         4         5         6         7         8         9        10
//    "12345"      "1234567"     "1234567890123456"    "12345678901234567890"    "12345678901234567890"    "1234567"    "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
   SL "22:24"   NE "ICE 153"  NE "Mainz Hbf"        NE "Schlier \374ber"      NE " Karlsruhe nach"      NE "-222F--" NE "+++ Vorsicht: STUMMI-ICE f\344hrt durch +++            +++ Danke an Tobias, Klaus & Fredddy,... +++ "
   SL "09:34"   NE "RB 1521"  NE "Aschaffenburg"    NE "Gro\337auheim - Kahl" NE "- Gro\337krotzenburg" NE ""        NE ""
   SL "10:04"   NE "RB 3237"  NE "Plattling"        NE "Freising - Moosburg"  NE "- Landshut"           NE ""        NE ""
   SL "12:53"   NE "EC 172"   NE "Hamburg - Altona" NE "Berlin Hbf - "        NE "Hamburg Hbf"          NE "-222211" NE "Versp\344tung ca 10 Min"
   SL "15:21"   NE "ICE 592"  NE "Berlin Ostbf"     NE "Fulda - Kassel -"     NE "Braunschweig Hbf"     NE "11111"   NE ""
   SL "17:02"   NE "IC 602"   NE "Puttgarden"       NE "Wuppertal - Dortmund" NE "Bremen - Hamburg"     NE " 22111"  NE ""
   SL "18:30"   NE "RE 7"     NE "Kiel / Flensburg" NE "Elmshorn -"           NE "Neum\374nster"        NE "   2121" NE "Zugteilung in Neum\374nster - Vorderer Zugteil f\344hrt bis Flensburg"
   SL G934      NE "Hg-Exp"   NE "Hogsmeade"        NE "King's Cross"         NE "- nonstop -"          NE "-------" NE "Hogwarts-Express"
   SL "21:45"   NE "ICE 651"  NE "Leipzig Hbf"      NE "Fulda - Eisenach"     NE ""                     NE ""        NE "Achtung: Heute auf Gleis 7"
   SL "#LDiese Zeile verändert nur den Lauftext"

; // End of the Text_Messages string



