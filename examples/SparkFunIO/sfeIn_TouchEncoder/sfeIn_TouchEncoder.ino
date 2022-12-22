/********************
Arduino generic menu system
Arduino menu using TouchEncoder and Serial Input/Output

Dec. 2022 Eric Homer - ehomer @ alum.mit.edu

Adapted from Blink and clickEncoder examples

Capactive Touch Encoder:
https://github.com/sparkfun/Qwiic_Capacitive_Touch_Slider_CAP1203

output: Serial
input: CapacitiveTouchEncoder, Serial
*/

#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIn.h>
#include <menuIO/SFE_TouchEncoderIn.h>
#include <menuIO/keyIn.h>
#include <Wire.h>
#include <SparkFun_CAP1203.h> // Click here to get the library: http://librarymanager/All#SparkFun_CAP1203


using namespace Menu;

#define LEDPIN LED_BUILTIN
#define MAX_DEPTH 1


// Capacitive Touch Declarations
CAP1203 touchsensor; // Initialize sensor
TouchEncoderStream encStream(touchsensor); // Initialize the encoderStream

//define serial input device
serialIn serial(Serial);

unsigned int timeOn=10;
unsigned int timeOff=90;


MENU(mainMenu, "Blink menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,FIELD(timeOn,"On","ms",0,1000,10,1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(timeOff,"Off","ms",0,10000,10,1,Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,EXIT("<Back")
);


//initializing output and menu nav with macros
//MENU_INPUTS(in,&encStream,&serial);
//
//MENU_OUTPUTS(out,MAX_DEPTH
//  ,SERIAL_OUT(Serial)
//  ,NONE//must have 2 items at least
//);
//
//NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);


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


menuOut* outputs[] MEMMODE={&outSerial};//list of output devices
outputsList out(outputs,1);//outputs list controller

menuIn* inputsList[]={&encStream,&serial};
chainStream<2> in(inputsList);

//define navigation root and aux objects
navNode nav_cursors[MAX_DEPTH];//aux objects to control each level of navigation
//navRoot nav(mainMenu, nav_cursors, MAX_DEPTH, serial, out);
navRoot nav(mainMenu, nav_cursors, MAX_DEPTH, in, out);



void setup() {
  pinMode(LEDPIN, OUTPUT);

  Wire.begin();       // Join I2C bus
  Serial.begin(9600); // Start serial for output
  while(!Serial);

  // Setup sensor
  if (touchsensor.begin() == false) {
    Serial.println("Not connected. Please check connections and read the hookup guide.");
    while (1);
  }
  else {
    Serial.println("Connected!");
  }
  
  Serial.println("Menu 4.x");
  Serial.println("Use keys + - * /");
  Serial.println("to control the menu navigation");
}

bool blink(int timeOn,int timeOff) {
  return millis()%(unsigned long)(timeOn+timeOff)<(unsigned long)timeOn;
}

void loop() {
  nav.poll();
  digitalWrite(LEDPIN, blink(timeOn,timeOff));
}