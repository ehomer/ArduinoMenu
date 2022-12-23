#include <Arduino.h>

/********************
Dec. 2022 Eric Homer - ehomer @ alum.mit.edu

Adapted from adafruitGfx_lcdMono written by Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

menu with SparkFun MicroOLED Breakout - https://github.com/sparkfun/SparkFun_Micro_OLED_Arduino_Library
output: MicroOLED display (I2C) + Serial
input: Serial

***/

#include <Wire.h>
#include <SFE_MicroOLED.h> //Click here to get the library: http://librarymanager/All#SparkFun_Micro_OLED
#include <SparkFun_CAP1203.h> // Click here to get the library: http://librarymanager/All#SparkFun_CAP1203

#include <menu.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
#include <menuIO/keyIn.h>
#include <menuIO/SFE_MicroOledOut.h>
#include <menuIO/SFE_TouchEncoderIn.h>

using namespace Menu;

//SFE_MicroOLED gfx(PIN_RESET); - connecting with I2C
#define PIN_RESET 9 // Optional - Connect RST on display to pin 9 on Arduino
MicroOLED gfx(PIN_RESET); //I2C declaration -  The TwoWire I2C port is passed to .begin instead
// Capacitive Touch Declarations
CAP1203 touchsensor; // Initialize sensor
TouchEncoderStream encStream(touchsensor); // Initialize the encoderStream
serialIn serial(Serial); //define serial input device

#define LEDPIN LED_BUILTIN

result showEvent(eventMask e,navNode& nav,prompt& item) {
  Serial.print(F("event:"));
  Serial.print(e);
  return proceed;
}

int test=55;
int ledCtrl=LOW;

result myLedOn() {
  ledCtrl=HIGH;
  return proceed;
}
result myLedOff() {
  ledCtrl=LOW;
  return proceed;
}

result alert(menuOut& o,idleEvent e);
result doAlert(eventMask e, prompt &item);

TOGGLE(ledCtrl,setLed,"Led: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
  ,VALUE("On",HIGH,doNothing,noEvent)
  ,VALUE("Off",LOW,doNothing,noEvent)
);

const char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
const char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
char buf1[]="0x11";

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,SUBMENU(setLed)
  ,OP("LED On",myLedOn,enterEvent)
  ,OP("LED Off",myLedOff,enterEvent)
  ,OP("Alert test",doAlert,enterEvent)
  ,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
  ,EXIT("<Back")
);

// define menu colors --------------------------------------------------------
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
//monochromatic color table
const colorDef<uint16_t> colors[6] MEMMODE={
  {{WHITE,BLACK},{WHITE,BLACK,BLACK}},//bgColor
  {{BLACK,WHITE},{BLACK,WHITE,WHITE}},//fgColor
  {{BLACK,WHITE},{BLACK,WHITE,WHITE}},//valColor
  {{BLACK,WHITE},{BLACK,WHITE,WHITE}},//unitColor
  {{BLACK,WHITE},{WHITE,WHITE,WHITE}},//cursorColor
  {{BLACK,WHITE},{WHITE,BLACK,BLACK}},//titleColor
};



#define gfxWidth 64
#define gfxHeight 48
#define fontX 5
#define fontY 9
#define MAX_DEPTH 2

//initializing output and menu nav with macros
/*MENU_INPUTS(in,&serial);

MENU_OUTPUTS(out,MAX_DEPTH
  ,SFEGFX_OUT(gfx,colors,fontX,fontY,{0,0,gfxWidth/fontX,gfxHeight/fontY})
  ,SERIAL_OUT(Serial)
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);*/

//initializing output and menu nav without macros
const panel default_serial_panels[] MEMMODE={{0,0,40,10}};
navNode* default_serial_nodes[sizeof(default_serial_panels)/sizeof(panel)];
panelsList default_serial_panel_list(
  default_serial_panels,
  default_serial_nodes,
  sizeof(default_serial_panels)/sizeof(panel)
);

//define output device
idx_t serialTops[MAX_DEPTH]={0};
serialOut outSerial(*(Print*)&Serial,serialTops);

//define outputs controller
idx_t gfx_tops[MAX_DEPTH];
PANELS(gfxPanels,{0,0,gfxWidth/fontX,gfxHeight/fontY});
sfe_mOledOut sfeOut(gfx,colors,gfx_tops,gfxPanels);

menuOut* outputs[] MEMMODE={&outSerial,&sfeOut};//list of output devices
outputsList out(outputs,2);//outputs list controller

menuIn* inputsList[]={&encStream,&serial};
chainStream<2> in(inputsList);

//define navigation root and aux objects
navNode nav_cursors[MAX_DEPTH];//aux objects to control each level of navigation
navRoot nav(mainMenu, nav_cursors, MAX_DEPTH, in, out);

result alert(menuOut& o,idleEvent e) {
  if (e==idling) {
    o.setCursor(0,0);
    o.print(F("alert test"));
    o.setCursor(0,1);
    o.print(F("press [select]"));
    o.setCursor(0,2);
    o.print(F("to continue..."));
  }
  return proceed;
}

result doAlert(eventMask e, prompt &item) {
  nav.idleOn(alert);
  return proceed;
}

//when menu is suspended
result idle(menuOut& o,idleEvent e) {
  o.setCursor(0,0);
  o.print(F("suspended..."));
  o.setCursor(0,1);
  o.print(F("press [select]"));
  o.setCursor(0,2);
  o.print(F("to continue"));
  return proceed;
}

void setup() {
  pinMode(LEDPIN,OUTPUT);
  Serial.begin(9600);
  while(!Serial);
  Serial.println(F("menu 4.x test"));
  Serial.flush();
  nav.idleTask=idle;//point a function to be used when menu is suspended

  Wire.begin();

  gfx.begin(0x3D, Wire);     // Initialize the OLED
  gfx.clear(ALL);  // Clear the library's display buffer
  gfx.display();   // Display what's in the buffer (splashscreen)
  delay(1000);
  gfx.clear(PAGE); // Clear the display's internal memory

  // Setup sensor
  if (touchsensor.begin() == false) {
    Serial.println("Not connected. Please check connections and read the hookup guide.");
    while (1);
  }
  else {
    Serial.println("Connected!");
  }
  
  gfx.println(F("Menu 4.x test on GFX"));
  gfx.display(); // show splashscreen
  delay(2000);
  gfx.clear(PAGE);
  gfx.display(); // show splashscreen
}

void loop() {
  //nav.poll();//it can work like this, followed by the gfx.display()
  //gfx.display();

  //or on a need to draw basis:
  nav.doInput();
  if (nav.changed(0)) {//only draw if changed
    nav.doOutput();
    gfx.display();
  }

  digitalWrite(LEDPIN, ledCtrl);
  delay(100);//simulate a delay when other tasks are done
}
