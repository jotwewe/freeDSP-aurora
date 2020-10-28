#ifndef ROTARYENCODER_H_
#define ROTARYENCODER_H_

class RotaryEncoder
{
public:
  RotaryEncoder( void );
  ~RotaryEncoder( void ) {}

  int getRotationValue();

  uint8_t getButtonPressCount() { return buttonPressCount; }
  
  void init( void );
  
  /**
   * To be called from main loop. Too lazy to implement timer ISR, because 
   * don't know arduino or esp32.
   */
  void poll();
  
private:
  uint8_t buttonPressCount;
  unsigned long buttonMicrosecsWhenPressed;
};

extern RotaryEncoder rotaryEncoder;

#endif
