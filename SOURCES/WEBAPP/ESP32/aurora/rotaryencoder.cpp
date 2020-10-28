#include <Arduino.h>

#include "hwconfig.h"
#include "rotaryencoder.h"

portMUX_TYPE gpioMux = portMUX_INITIALIZER_UNLOCKED;

RotaryEncoder rotaryEncoder;

// A 00110011
// B 01100110
// 
// left to right (+1):
// 0001  1
// 0111  7
// 1110 14
// 1000  8
// 
// right to left (-1):
// 1011 11
// 1101 13
// 0100  4
// 0010  2

static  int     encoder_value;
static  uint8_t encoder_state;
static  uint8_t button_state;

DRAM_ATTR const int8_t encoder_stepTable[16] = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};

// See
// https://github.com/espressif/arduino-esp32/issues/3634
//   "This error typically will only occur if you have an interrupt routine that runs while accessing flash (ie:spiffs)"
//   https://github.com/espressif/arduino-esp32/issues/855
//   "Beware, not only the ISR (Interrupt) has to be in IRAM! Every function called from the ISR also needs to be declared as IRAM_ATTR"
//   https://github.com/espressif/arduino-esp32/issues/489
//   https://github.com/espressif/arduino-esp32/issues/3697
//   "If they use SPIFFS and a webserver on the ESP32 then there is a very high chance of the ESP32 crashing while serving up web content IF your ISR is triggered/running at the time of flash access."
// and also use DRAM_ATTR on the const data above, see:
//   https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/general-notes.html#dram-data-ram
// or else you'll get
// +--------------------------------------------------------
// | Uploading user parameters.............[OK]
// | Uploading user parameters from /usrparam.003Guru Meditation Error: Core  1 panic'ed (Cache disabled but cached memory region accessed)
// | Core 1 register dump:
// | PC      : 0x40080fac  PS      : 0x00060034  A0      : 0x80081000  A1      : 0x3ffbe790  
// | A2      : 0x00000001  A3      : 0x3ffd1ea0  A4      : 0x3ffbec44  A5      : 0x3ffb1af0  
// | A6      : 0x00000000  A7      : 0x3ffb1b2c  A8      : 0x000000d0  A9      : 0x3ffbe770
// | A10     : 0x00000001  A11     : 0x3ffb8058  A12     : 0x00000001  A13     : 0x00000001  
// | A14     : 0x00060021  A15     : 0x00000000  SAR     : 0x00000012  EXCCAUSE: 0x00000007  
// | EXCVADDR: 0x00000000  LBEG    : 0x4000c349  LEND    : 0x4000c36b  LCOUNT  : 0x00000000  
// | Core 1 was running in ISR context:
// | EPC1    : 0x40092cef  EPC2    : 0x00000000  EPC3    : 0x00000000  EPC4    : 0x40080fac
// | 
// | Backtrace: 0x40080fac:0x3ffbe790 0x40080ffd:0x3ffbe7b0 0x40084f71:0x3ffbe7d0 0x40092cec:0x3ffb1b50 0x40093579:0x3ffb1b70 0x400879e4:0x3ffb1b90 0x40152d4f:0x3ffb1c00 0x400fd3b3:0x3ffb1c30 0x0
// +--------------------------------------------------------
//
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/fatal-errors.html
// reads:
//
// Cache disabled but cached memory region accessed
//
// In some situations ESP-IDF will temporarily disable access to external SPI Flash and SPI RAM via 
// caches. For example, this happens with spi_flash APIs are used to read/write/erase/mmap regions 
// of SPI Flash. In these situations, tasks are suspended, and interrupt handlers not registered with 
// ESP_INTR_FLAG_IRAM are disabled. Make sure that any interrupt handlers registered with this flag 
// have all the code and data in IRAM/DRAM. Refer to the SPI flash API documentation for more details.
void IRAM_ATTR isrRotaryEncoderAB( void )
{
  portENTER_CRITICAL_ISR( &gpioMux );

  uint8_t state = (encoder_state << 2) & 0x0F;
  
  // According to 
  // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-gpio.c
  // digitalRead() has IRAM_ATTR, so this is fine:
  if (digitalRead(ROTARYENCODER_PINA))
    state |= 0b0001;
  if (digitalRead(ROTARYENCODER_PINB))
    state |= 0b0010;

  if (button_state == 0 && digitalRead(ROTARYENCODER_PINSW))
    encoder_value += encoder_stepTable[state];

  encoder_state = state;
  portEXIT_CRITICAL_ISR( &gpioMux );
}

RotaryEncoder::RotaryEncoder( void )
{
}

int RotaryEncoder::getRotationValue()
{
  return encoder_value;
}

void RotaryEncoder::init( void )
{
  encoder_value = 0;
  
  encoder_state = 0; 
  if (digitalRead(ROTARYENCODER_PINA))
    encoder_state |= 0b0001;
  if (digitalRead(ROTARYENCODER_PINB))
    encoder_state |= 0b0010;
  
  button_state = 0;
  buttonPressCount = 0; 

  pinMode( ROTARYENCODER_PINA, INPUT_PULLUP );
  pinMode( ROTARYENCODER_PINB, INPUT_PULLUP );
  pinMode( ROTARYENCODER_PINSW, INPUT_PULLUP );

  attachInterrupt( ROTARYENCODER_PINA, isrRotaryEncoderAB, CHANGE );
  attachInterrupt( ROTARYENCODER_PINB, isrRotaryEncoderAB, CHANGE );
}

void RotaryEncoder::poll()
{
  // Don't use millis(), because according to 
  // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-misc.c
  // it uses a division which might be costly (mulshift instead?)
  // There also is micros(), way cheaper.
  if (digitalRead(ROTARYENCODER_PINSW))
  {
    // not pressed
    button_state = 0;
  }
  else
  {
    unsigned long m = micros();
    if (button_state == 0)
    {
      buttonMicrosecsWhenPressed = m;
      button_state = 1;
    }
    else if (button_state == 1 && m - buttonMicrosecsWhenPressed > 40000)
    {      
      button_state = 2;
      buttonPressCount++;
    }
  }
}
