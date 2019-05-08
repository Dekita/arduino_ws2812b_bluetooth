/********************************************************
* #######################################################
* 
*  author: dekitarpg@gmail.com
*  Arduino with Bluetooth (HC-05) and WS2812B LEDs
* 
* #######################################################
********************************************************/

// Included Libraries:..
#include "bt-serial.h"
#include <FastLED.h>

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

/**
* use TOSTRING() function to ensure string value for
* sending to the bluetooth client. 
*/
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

/**
* Various preprocessor constants
* Used as example data for sending to client
*/
#define FIRMWARE_VERSION beta


// FastLED Stuff
#define DATA_PIN            3
//#define CLK_PIN           4
#define LED_TYPE            WS2812B
#define COLOR_ORDER         GRB
#define NUM_LEDS            45
#define BRIGHTNESS          94
#define FRAMES_PER_SECOND   30

/**
* Define global variables used.
*/
CRGB leds[NUM_LEDS];
bool gHueSwitching = false;
uint8_t gHue = 0;

// Is changed when bluetooth serial command is recieved.
byte gSelectedAnimation = 1;

/**
* setup();
* Default Arduino setup function
*/
void setup() {
    // small delay for recovery
    delay(1000); 

    // Use pin A1 as INPUT for bt device state
    // This way we know when a device is connected
    pinMode(A1, INPUT);

    // tell FastLED about the LED strip configuration
    FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
    // set master brightness control
    FastLED.setBrightness(BRIGHTNESS);

    // setup BTSerial module
    BTSerial::begin(9600, true);
}

/**
* loop();
* Default Arduino loop function
*/
void loop() {
    // Check if bluetooth modules 'state' pin is high.
    // this pin will be high when a device (ie, your phone) 
    // is connected to the module. 
    bool device_connected = digitalRead(A1) == 1;

    // Call update to update bluetooth serial
    // update will return true if a command is
    // ready to be used. This saves having to Call
    // BTSerial::hasCommand(); after BTSerial::update();
    if (device_connected && BTSerial::update()) {
        // process the available command
        processBTSCommand();
        // Flush the serial buffer
        BTSerial::flush();
    } else if (!device_connected) {
        switch (gSelectedAnimation) {
          case 1: gameofthrones(); break;
          case 2: unicornrainbow(); break;
          case 3: imbatman(); break;
        }
        // send the 'leds' array out to the actual LED strip
        FastLED.show();  
        // insert a delay to keep the framerate modest
        FastLED.delay(1000/FRAMES_PER_SECOND);
    }
}

/**
* processBTSCommand();
* Processes data stored in the most recent bt serial command
*/
void processBTSCommand() {
    // sorts the command data from the buffer string
    // this is required before reading the commands
    BTSCommand comm = BTSerial::getCommand();

    // If command matches disconnection then 
    // return as its not a valid command
    if (strcmp(comm.cn, "+DISC") == 0) {
        Serial.println(""); return;
    }

    // print current command data to serial monitor:..
    Serial.print(comm.cn); Serial.print(" : "); Serial.println(comm.cv);

    // begin checking commands:..
    if (strcmp(comm.cn, "system") == 0) {
        sendStringPairToDevice("version", TOSTRING(FIRMWARE_VERSION));

    } else if (strcmp(comm.cn, "throne") == 0) {
        gSelectedAnimation = 1;
        sendStringPairToDevice("mode", "got");

    } else if (strcmp(comm.cn, "unicorn") == 0) {
        gSelectedAnimation = 2;
        sendStringPairToDevice("mode", "unicorn");

    } else if (strcmp(comm.cn, "batman") == 0) {
        gSelectedAnimation = 3;
        sendStringPairToDevice("mode", "batman");

    } else if (strcmp(comm.cn, "mode") == 0) {
        // convert value to byte:..
        byte value = constrain((byte)atoi(comm.cv), 1, 3);
        sendStringPairToDevice("mode", value);
        gSelectedAnimation = value;
    }
}

/**
* sendStringPairToDevice(k,v);
* Sends k and v strings to device formatted as below
* k:v;
*/
void sendStringPairToDevice(char* k, char* v) {
    BTSerial::print(k);
    BTSerial::print(":");
    BTSerial::print(v);
    BTSerial::println(";");
}

/**
* gameofthrones() led effect function
*/
void gameofthrones(){
  // fadeout all leds
  fadeToBlackBy(leds, NUM_LEDS, 24);
  // get random led id
  int pos = random16(NUM_LEDS);
  // get maximum rand value based on sine wave
  int maxrand = beatsin8(30, 0, 64);
  // set the led id to hue value + rand(maxrand)
  leds[pos] = CHSV(gHue + random8(maxrand), 200, 255);
  // periodic update: 30 milliseconds
  EVERY_N_MILLISECONDS(30) { 
    // check if hue is switching
    if (gHueSwitching) {
      // increase global hue
      gHue++;
      // check if hue is any of desired values
      if (gHue == 0 || gHue == 132){
        // set hue switch flag to false
        gHueSwitching = false;
      }
    }
  }
  // periodic update: 15 seconds
  EVERY_N_SECONDS(15) { 
    // set hue switch flag to true
    gHueSwitching = true;
  } 
}

/**
* unicornrainbow() led effect function
*/
void unicornrainbow(){
  fill_rainbow(leds, NUM_LEDS, gHue, 7);
  byte chance = beatsin8(30, 0, 50);
  if( random8() < chance) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
  EVERY_N_MILLISECONDS(30) { 
    gHue++;
  }
}

/**
* imbatman() led effect function
*/
void imbatman() {
  fadeToBlackBy(leds, NUM_LEDS, 80);
  leds[beatsin16(10, 0, NUM_LEDS-1)] |= CHSV(0, 200, 255);
  leds[beatsin16(11, 0, NUM_LEDS-1)] |= CHSV(100, 200, 255);
  leds[beatsin16(12, 0, NUM_LEDS-1)] |= CHSV(140, 200, 255);
}

/********************************************************
* #######################################################
* 
*  End of Code
* 
* #######################################################
********************************************************/
