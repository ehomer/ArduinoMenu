/* -*- C++ -*- */
/**************

SFE_TouchEncoderIn.h

Dec. 2022
Adapted by Eric Homer ehomer @ alum.mit.edu
Based on clickEncoderIn.h by Christophe Persoz and Rui Azevedo.
This file enables a Capactive Touch Slider/Pad to act as input for menu 
naviation. The device and library are described here: https://github.com/sparkfun/Qwiic_Capacitive_Touch_Slider_CAP1203

There may be better ways to accomplish some tasks, I have not overridden these 
kinds of functions before.

***/


#ifndef __SFE_TouchEncoderIn_h__
  #define __SFE_TouchEncoderIn_h__

  #include <Arduino.h>
  #include <SparkFun_CAP1203.h>
  #include "../menuDefs.h"

  namespace Menu {

    //emulate a stream encoder based on touch pad input
  class TouchEncoderStream:public menuIn {
    public:
      CAP1203 &enc; //associated hardware Capactive Touch Slider (CAP1203)
      
      //touch sensor values storage
      bool left=false;
      bool middle=false;
      bool right=false;
      bool middlelong=false;
      bool swipeleft=false;
      bool swiperight=false;

      inline void update() {
        if ( left || middle || right || swipeleft || swiperight || middlelong) return;
        if (enc.isLeftTouched() == true)
        {
          while (enc.isLeftTouched() == true); // Wait until user removes finger
          left=true;
        }
        else if (enc.isMiddleTouched() == true)
        {
          long currentMillis = millis();
          while (enc.isMiddleTouched() == true); // Wait until user removes finger
          middle=true;
          if ( (millis() - currentMillis) > 2000) {
            middlelong = true;
            middle=false;
          }
        }
        else if (enc.isRightTouched() == true)
        {
          while (enc.isRightTouched() == true); // Wait until user removes finger
          right=true;
        }
      }

      TouchEncoderStream(CAP1203 &enc):enc(enc) {}

      int available(void) {
        return peek() != -1;
      }

      int peek(void) {
        update();
        if (middle == true)
          return options->navCodes[enterCmd].ch;//menu::enterCode;
        if (middlelong == true)
          return options->navCodes[escCmd].ch;//menu::escCode;
        if (right == true)
            return options->navCodes[downCmd].ch;//menu::downCode;
        if (left == true)
            return options->navCodes[upCmd].ch;//menu::upCode;
        return -1;
      }

      int read() {
        int ch = peek();
        left=middle=right=swipeleft=swiperight= middlelong=false;
        return ch;
      }

      void flush() {
          enc.clearInterrupt();
          update();
      }

      size_t write(uint8_t v) {
          return 0;
      }
    };
  }//namespace Menu

#endif /* SFE_TouchEncoderIn_h */
