/********************************************************
* #######################################################
* 
*  Bluetooth Serial - bt-serial.h
*  dekitarpg@gmail.com
* 
* #######################################################
********************************************************/
#ifndef DEKBTS_h
#define DEKBTS_h

#define DEBUFFER_LIMIT 64 // 63 data bytes + null termiantor

/**
* Included Libraries:..
*/
#include <Arduino.h>
//#include <AltSoftSerial.h>
#include <SoftwareSerial.h>

/**
* BTSCommand {struct}
* n: {String}
* v: {String}
* Stores a commands name and value as strings 
*/
typedef struct {
    char cn[DEBUFFER_LIMIT/2];
    char cv[DEBUFFER_LIMIT/2];
} BTSCommand;

/**
* BTSerial {namespace}
* See bt-serial.cpp for full definitions:..
*/
namespace BTSerial {
    // Functions:..
    void begin(int,bool);
    bool update();
    bool hasCommand();
    BTSCommand getCommand();
    void resetDebuffer(char*);
    void flush();
    void print(char*);
    void println(char*);
    // Variables:..
    static char debuffer[DEBUFFER_LIMIT];
    static int buffer_index;
//    static AltSoftSerial bts;
    static SoftwareSerial bts(8,9);
    static BTSCommand command;
    static int baud_rate;
    static bool comready;
}

#endif
/********************************************************
* #######################################################
* 
*  End of Code
* 
* #######################################################
********************************************************/
