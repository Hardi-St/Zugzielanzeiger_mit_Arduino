#include <U8glib.h>


U8GLIB_SSD1306_128X32_2X u8g(U8G_I2C_OPT_NONE);

String gleis = "GGG";
char lauftext[121];
char uhrzeit[6];
char zugnummer[8];
char ziel[17];
char zuglauf1[21];
char zuglauf2[21];
String wagenstand = "-------";

const int lauftextlength = 21;                                                                                // 27.01.19:  Removed "unsigned" to remove compiler warnings
int offset = 0;
int subset = 0;
char ausgabe[lauftextlength];

char incomingByte = 0;
unsigned int incomingBytePosition = 0;

enum ReceiveState
{
  ReceiveState_Idle,
  ReceiveState_Hash,
  ReceiveState_Lauftext,
  ReceiveState_Gleis,
  ReceiveState_Wagenstand,
  ReceiveState_Zuglauf1,
  ReceiveState_Zuglauf2,
  ReceiveState_Ziel,
  ReceiveState_Uhrzeit,
  ReceiveState_Zugnummer
};

ReceiveState currentState = ReceiveState_Idle;

enum GleisSeite
{
  GleisSeite_Links,
  GleisSeite_Rechts
};

GleisSeite gleisSeite = GleisSeite_Links;

void draw()
{
  u8g.setColorIndex(1);
  static int zugdatenSaplte = 0;
  if(gleisSeite == GleisSeite_Links)
  {
    zugdatenSaplte = 100;
  }
  else if(gleisSeite == GleisSeite_Rechts)
  {
    zugdatenSaplte = 0;
  }

  u8g.setFont(u8g_font_5x8); // 5x8
  u8g.drawStr(zugdatenSaplte, u8g.getFontAscent(), uhrzeit);

  u8g.setFont(u8g_font_4x6); // 4x6
  u8g.drawStr(zugdatenSaplte, 7 + u8g.getFontAscent(), zugnummer);

  if(wagenstand.length() > 0)
  {
    u8g.setColorIndex(1);
    u8g.setFont(u8g_font_4x6); // 4x6
    u8g.drawStr(zugdatenSaplte, 15 + u8g.getFontAscent(), "ABCDEFG");
    u8g.drawBox(zugdatenSaplte, 21 + u8g.getFontAscent() - 5, wagenstand.length() * 4, 6);
    u8g.setColorIndex(0);
    u8g.drawStr(zugdatenSaplte, 21 + u8g.getFontAscent(), wagenstand.c_str());
    u8g.setColorIndex(1);
  }

  static int zuglaufSpalte = 30;
  if(gleisSeite == GleisSeite_Links)
  {
    zuglaufSpalte = 20;
  }
  else if(gleisSeite == GleisSeite_Rechts)
  {
    zuglaufSpalte = 30;
  }

  //VerspÃƒÂ¤tungsbox
  if(lauftext[20] != '\0')
  {
    u8g.setColorIndex(1);
    u8g.drawBox(zuglaufSpalte, 0, 78, 6);
    u8g.setColorIndex(0);
    u8g.setFont(u8g_font_4x6); // 4x6
    int remaining = strlen(lauftext) - offset;
    if(remaining > (lauftextlength - 1))
    {
      remaining = lauftextlength - 1;
    }
    memcpy(ausgabe, &lauftext[offset], remaining);
    ausgabe[remaining] = '\0';
    //Serial.println(lauftext);
    //Serial.println(ausgabe);
    u8g.drawStr(zuglaufSpalte-subset, 6, ausgabe);
    u8g.setColorIndex(1);
  }

  u8g.drawStr(zuglaufSpalte, 12, zuglauf1);

  u8g.drawStr(zuglaufSpalte, 19, zuglauf2);

  u8g.setFont(u8g_font_tpss);
  u8g.drawStr(zuglaufSpalte, 34 + u8g.getFontDescent(), ziel);

  u8g.setFont(u8g_font_6x13B);
  if(gleisSeite == GleisSeite_Links)
  {
    u8g.drawStr(0, 13, gleis.c_str());
  }
  else if(gleisSeite == GleisSeite_Rechts)
  {
    u8g.drawStr(128 - (6 * gleis.length()), 13, gleis.c_str());
  }

  //u8g.setColorIndex(1);
  // u8g.drawFrame(0, 0, 128, 32); // Test
}

void initializeStrings()
{
  String("Zuglauf 1  Zuglauf 1").toCharArray(zuglauf1, 21);
  String("Zuglauf 2  Zuglauf 2").toCharArray(zuglauf2, 21);
  String("Ziel   Ziel").toCharArray(ziel, 21);
  String("Abfah").toCharArray(uhrzeit, 6);
  String("Zugnumm").toCharArray(zugnummer, 8);
  String("                    +++ Lauftext +++ Lauftext +++ Lauftext +++ Lauftext +++ Lauftext +++ Lauftext ++").toCharArray(lauftext, 101);;
}

void setup()
{
  initializeStrings();

  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  digitalWrite(7, HIGH);

  Serial.begin(115200);

  u8g.begin();

  u8g.setContrast(150);     // Kontrast eingeben                                                              // 23.01.19:  Copied from Zugzielanzeiger_71.ino
  u8g.setRot180();          // anzeige 180 grad rotieren weil dann der breitere,
                            // nicht beschreibbare Rand des Displays oben ist (Unter den Bahnhofsdach)
}

int contrast = 0;

void loop() {

  u8g.firstPage();
  do {
    draw();
  } while( u8g.nextPage() );

  //delay(250);

  subset += 1;
  if(subset > 3)
  {
    offset += 1;
    subset = 0;
  }

  if(offset > (int)strlen(lauftext))                                                                          // 27.01.19:  Added: (int) to remove compiler warnings
  {
    offset = 0;
  }


  while (Serial.available() > 0)
  {
    // read the incoming byte:
    incomingByte = Serial.read();
    if(currentState == ReceiveState_Idle)
    {
      if(incomingByte == '#')
      {
        currentState = ReceiveState_Hash;
      }
    }
    else if(currentState == ReceiveState_Hash)
    {
      if(incomingByte == 'L')
      {
        Serial.println("Empfange Lauftext");
        currentState = ReceiveState_Lauftext;
        lauftext[20] = '\0';
        incomingBytePosition = 20;
      }
      else if(incomingByte == 'G')
      {
        Serial.println("Empfange Gleisnummer");
        currentState = ReceiveState_Gleis;
        gleis = "";
      }
      else if(incomingByte == 'W')
      {
        Serial.println("Empfange Wagenstand");
        currentState = ReceiveState_Wagenstand;
        wagenstand = "";
      }
      else if(incomingByte == '1')
      {
        Serial.println("Empfange Zuglauf 1");
        currentState = ReceiveState_Zuglauf1;
        zuglauf1[0] = '\0';
        incomingBytePosition = 0;
      }
      else if(incomingByte == '2')
      {
        Serial.println("Empfange Zuglauf 2");
        currentState = ReceiveState_Zuglauf2;
        zuglauf2[0] = '\0';
        incomingBytePosition = 0;
      }
      else if(incomingByte == 'Z')
      {
        Serial.println("Empfange Ziel");
        currentState = ReceiveState_Ziel;
        ziel[0] = '\0';
        incomingBytePosition = 0;
      }
      else if(incomingByte == 'U')
      {
        Serial.println("Empfange Abfahrtszeit");
        currentState = ReceiveState_Uhrzeit;
        uhrzeit[0] = '\0';
        incomingBytePosition = 0;
      }
      else if(incomingByte == 'N')
      {
        Serial.println("Empfange Zugnummer");
        currentState = ReceiveState_Zugnummer;
        zugnummer[0] = '\0';
        incomingBytePosition = 0;
      }
      else if(incomingByte == 'X')
      {
        Serial.println("LÃƒÂ¶sche Zug");
        zugnummer[0] = '\0';
        uhrzeit[0] = '\0';
        ziel[0] = '\0';
        zuglauf1[0] = '\0';
        zuglauf2[0] = '\0';
        wagenstand = "";
        currentState = ReceiveState_Idle;
      }
      else if(incomingByte == '<')
      {
        Serial.println("Gleis Links");
        gleisSeite = GleisSeite_Links;
        currentState = ReceiveState_Idle;
      }
      else if(incomingByte == '>')
      {
        Serial.println("Gleis Rechts");
        gleisSeite = GleisSeite_Rechts;
        currentState = ReceiveState_Idle;
      }
    }
    else if(currentState == ReceiveState_Lauftext)
    {
      if((incomingByte == '#') || (incomingBytePosition == 120))
      {
        lauftext[incomingBytePosition] = '\0';
        Serial.print("Speichere Lauftext: ");
        Serial.println(lauftext);
        offset = 0;
        subset = 0;
        currentState = ReceiveState_Idle;
      }
      else
      {
        lauftext[incomingBytePosition] = incomingByte;
        incomingBytePosition++;
      }
    }
    else if(currentState == ReceiveState_Gleis)
    {
      if(incomingByte == '#')
      {
        Serial.println("Speichere Gleisnummer: " + gleis);
        currentState = ReceiveState_Idle;
      }
      else
      {
        gleis += incomingByte;
      }
    }
    else if(currentState == ReceiveState_Wagenstand)
    {
      if(incomingByte == '#')
      {
        Serial.println("Speichere Wagenstand: " + wagenstand);
        currentState = ReceiveState_Idle;
      }
      else
      {
        wagenstand += incomingByte;
      }
    }
    else if(currentState == ReceiveState_Ziel)
    {
      if((incomingByte == '#') || (incomingBytePosition == 16))
      {
        ziel[incomingBytePosition] = '\0';
        Serial.print("Speichere Ziel: ");
        Serial.println(ziel);
        currentState = ReceiveState_Idle;
      }
      else
      {
        ziel[incomingBytePosition] = incomingByte;
        incomingBytePosition++;
      }
    }
    else if(currentState == ReceiveState_Zuglauf1)
    {
      if((incomingByte == '#') || (incomingBytePosition == 20))
      {
        zuglauf1[incomingBytePosition] = '\0';
        Serial.print("Speichere Zuglauf 1: ");
        Serial.println(zuglauf1);
        currentState = ReceiveState_Idle;
      }
      else
      {
        zuglauf1[incomingBytePosition] = incomingByte;
        incomingBytePosition++;
      }
    }
    else if(currentState == ReceiveState_Zuglauf2)
    {
      if((incomingByte == '#') || (incomingBytePosition == 20))
      {
        zuglauf2[incomingBytePosition] = '\0';
        Serial.print("Speichere Zuglauf 2: ");
        Serial.println(zuglauf2);
        currentState = ReceiveState_Idle;
      }
      else
      {
        zuglauf2[incomingBytePosition] = incomingByte;
        incomingBytePosition++;
      }
    }
    else if(currentState == ReceiveState_Zugnummer)
    {
      if((incomingByte == '#') || (incomingBytePosition == 7))
      {
        zugnummer[incomingBytePosition] = '\0';
        Serial.print("Speichere Zugnummer: ");
        Serial.println(zugnummer);
        currentState = ReceiveState_Idle;
      }
      else
      {
        zugnummer[incomingBytePosition] = incomingByte;
        incomingBytePosition++;
      }
    }
    else if(currentState == ReceiveState_Uhrzeit)
    {
      if((incomingByte == '#') || (incomingBytePosition == 6))
      {
        uhrzeit[incomingBytePosition] = '\0';
        Serial.print("Speichere Abfhartszeit: ");
        Serial.println(uhrzeit);
        currentState = ReceiveState_Idle;
      }
      else
      {
        uhrzeit[incomingBytePosition] = incomingByte;
        incomingBytePosition++;
      }
    }
  }
}


