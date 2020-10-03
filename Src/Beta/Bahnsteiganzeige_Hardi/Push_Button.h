#ifndef __PUSH_BUTTON_H__
#define __PUSH_BUTTON_H__

//:::::::::::::::
class Push_Button
//:::::::::::::::
{
public:
  //------------------------
  Push_Button(uint8_t PinNr) :
  //------------------------
  Old(0),
  Pin(PinNr)
  {
    pinMode(Pin, INPUT_PULLUP);
  }

  //------------
  bool Pressed()
  //------------
  {
    //if (Old || digitalRead(Pin) == 0) delay(30); // simple Debounce
    uint8_t Act = digitalRead(Pin) == 0; // Low active button
    uint8_t ret = (Old == 0 && Act);
    Old = Act;
    return ret;
  }

private:
  uint8_t Old:1;
  uint8_t Pin:5; // 0..31
};
#endif // __PUSH_BUTTON_H__


