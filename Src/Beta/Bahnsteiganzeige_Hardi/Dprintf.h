/*
 Dprintf.h


 Debug printf function
 ~~~~~~~~~~~~~~~~~~~~~
 This module contains functions for debug printing. It is enabled with the
 compiler switch _PRINT_DEBUG_MESSAGES.
 If it's disabled no memory is used.

*/
#ifndef _Dprintf_h_
#define _Dprintf_h_

#if _PRINT_DEBUG_MESSAGES == 1
   // F() makro saves dynamic memory
   #define Dprintf(Format, ...) Dprintf_proc(F(Format), ##__VA_ARGS__)   // see: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
   // Memory based on 01.09.18:
   // Flash: 18426   59%
   // Ram:     740   36%
#else
   #define DPRINTF(Str)
   #define Dprintf(Format, ...)
   // Flash: 16534   53%     save: 1892
   // Ram:     720   35%     save: 20
#endif


#include <stdarg.h>
#include <Wstring.h>

void Dprintf_proc(const __FlashStringHelper *format, ...);


#if _PRINT_DEBUG_MESSAGES == 1
  //-------------------------------------------------------
  void Dprintf_proc(const __FlashStringHelper *format, ...)  // uses 812 byte FLASH and 173 Bytes RAM if enabled ;-(
  //-------------------------------------------------------
  {
    char buf[150];
    va_list ap;
    va_start(ap, format);
    #ifdef __AVR__
       vsnprintf_P(buf, sizeof(buf), (const char *)format, ap); // progmem for AVR
    #else
       vsnprintf  (buf, sizeof(buf), (const char *)format, ap); // for the rest of the world
    #endif

    va_end(ap);
    Serial.print(buf);
  }
#endif

#endif  // _Dprintf_h_





