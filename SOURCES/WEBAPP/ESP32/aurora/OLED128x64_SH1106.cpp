#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include "hwconfig.h"
#include "OLED128x64_SH1106.h"

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2( U8G2_R0, /* clock=*/ I2C_SCL_PIN, /* data=*/ I2C_SDA_PIN, /* reset=*/ U8X8_PIN_NONE );

void OLED128x64_SH1106::begin( void )
{
  u8g2.setBusClock(100000);
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void OLED128x64_SH1106::clearBuffer( void )
{
  u8g2.clearBuffer();
}

void OLED128x64_SH1106::sendBuffer(void )
{
  u8g2.sendBuffer();
}

void OLED128x64_SH1106::drawBootScreen( void )
{
  u8g2.clearBuffer();

  u8g2.setFont( u8g2_font_helvR14_tf );
  u8g2.drawStr( 23, 10, "AURORA");

  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr( 0, 54, "Booting...");

  u8g2.sendBuffer();
}

void OLED128x64_SH1106::drawUI(const char* ip, const char* preset, const char* input, float vol, int cursor_at)
{
  const int offX    = 16;
  const int zeile0  =  0;
  const int zeile1  = 22;
  const int spalte1 = 55;
  u8g2.clearBuffer();    
  u8g2.setFont(u8g2_font_helvR14_tf);
  
  u8g2.drawStr(offX, zeile0, input);
  
  u8g2.drawStr(offX, zeile1, preset);

  char buf[10];
  dtostrf(vol, 4, 1, buf);
  u8g2.drawStr(spalte1+offX, zeile1, buf);
  
  u8g2.setFont(u8g2_font_open_iconic_arrow_2x_t);
  switch (cursor_at)
  {
    case 0:
      u8g2.drawGlyph(0, zeile0, 87);
      break;
    case 1:
      u8g2.drawGlyph(0, zeile1, 87);
      break;
    case 2:
      u8g2.drawGlyph(spalte1, zeile1, 87);
      break;
  }
  
  u8g2.setFont(u8g2_font_5x7_tf);  
  u8g2.drawStr(0, 56, ip); // this position is fine without descents
  
  if (false)
  {
    u8g2.drawLine(  0,  0, 127,  0);
    u8g2.drawLine(  0, 63, 127, 63);
    u8g2.drawLine(  0,  0,   0, 63);
    u8g2.drawLine(127,  0, 127, 63);
  }
  
  u8g2.sendBuffer();
}

void OLED128x64_SH1106::drawSwitchingPreset( void )
{
  u8g2.clearBuffer();

  u8g2.setFont( u8g2_font_helvR08_tf );
  u8g2.drawStr( 5, 10, "Switching Preset...");

  u8g2.sendBuffer();
}
