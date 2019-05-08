/********************************************************
* #######################################################
* 
*  Bluetooth Serial - bt-serial.cpp
*  dekitarpg@gmail.com
* 
* #######################################################
********************************************************/

/**
* Included Libraries:..
*/
#include "bt-serial.h"

/**
* BTSerial::begin(baud_rate);
* @param baud_rate {int}
* Setup the Bluetooth serial module initializing
* communication rate to 'baud_rate'.
*/
void BTSerial::begin(int brate, bool iprint) {
    baud_rate = brate;
    buffer_index = 0;
    if (iprint) {
      Serial.begin(baud_rate);
    }
    bts.begin(baud_rate);
    if (iprint) {
      Serial.println("BTSerial Started!");
      bts.println("BTS says hi!");
    }
}

/**
* BTSerial::update();
* @return hasCommand() {bool}
* Updates to read the serial data for the bluetooth connection.
* Should be called at least once before using getCommand(); 
* after each command has been read, make sure to call update();
* again before calling getCommand(); again.
*/
bool BTSerial::update() {
    char this_char;
    //if (Serial.available()) {
    //    this_char = Serial.read();
    //    bts.print(this_char);
    //}
    if (bts.available()) {
        while (bts.available() > 0){
            // manage overflow:..
            if (buffer_index+1 > DEBUFFER_LIMIT) {
                resetDebuffer("OVERFLOW");
                break;
            }
            // no overflow, lets continue:..
            // get next character of data
            this_char = bts.read();
            //Serial.println(this_char);
            // if char is terminator, then flag command as ready and break. 
            if (this_char == ';') {
                //char msg[] = "message received: ";
                buffer_index = 0;
                comready = true; 
                break;
            } else { // add new char to buffer string.
                debuffer[buffer_index] = this_char;
                debuffer[++buffer_index] = '\0';
            }
        }
    }
    // return bool for if we have command or not
    return hasCommand();
}

/**
* BTSerial::resetDebuffer()
* @return {bool} based on if there is a command ready.
*/
void BTSerial::resetDebuffer(char *reason) {
    char msg[] = "BUFFER RESET: ";
    buffer_index = 0; debuffer[0] = '\0';
    //Serial.print(msg); Serial.println(reason);
    //bts.print(msg); bts.println(reason);
}

/**
* BTSerial::hasCommand()
* @return {bool} based on if there is a command ready.
*/
bool BTSerial::hasCommand() {
    return comready;
}

/**
* BTSerial::flush()
* Flushes the BTSerial buffer
*/
void BTSerial::flush() {
    while(bts.available() > 0) { bts.read(); }
    //Serial.println("Flushed!");
}

/**
* BTSerial::getCommand()
* @return {BTSCommand} containing n and v for command.
* This function splits the buffer string to seperate
* the command n(name) and v(value).
* It then resets the command ready flag to allow for
* the next command to be processed.
*/
BTSCommand BTSerial::getCommand() {
    int valStart = 0;
    for (int i=0; debuffer[i] != '\0'; i++) {
        if (debuffer[i] == ':') {
            valStart = i+1;
        }
        if (valStart > 0) {
            command.cv[i-valStart] = debuffer[i];
            command.cv[i-valStart+1] = '\0';
        } else {
            command.cn[i] = debuffer[i];
            command.cn[i+1] = '\0';
        }
    }
    resetDebuffer("has command!");
    //Serial.print(debuffer);
    comready = false;
    return command;
}

/**
* BTSerial::print()
* BTSerial::println()
* Similar to Serial.print/println functions,
* but for the BTSerial communication line.
*/
void BTSerial::print(char* str) {
    bts.print(str);
}
void BTSerial::println(char* str) {
    bts.println(str);
}

/********************************************************
* #######################################################
* 
*  End of Code
* 
* #######################################################
********************************************************/
