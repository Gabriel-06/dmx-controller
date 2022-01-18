/*  Compact 8 Channel DMX Controller
 *   
 *  NOTE: This code is writen to work with revision B and later of board 2
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  Copyright 2021 Gabriel Csizmadia
 *  Open source at GitHub: https://github.com/Gabriel-06/dmx-controller
 */
 
#include <ShiftRegister74HC595.h>
#include <ArduinoDMX.h>

#define DEBUG //Turn debug code on or off

//Define pins where faders and buttons are connected
#define fader0 A0
#define fader1 A1
#define fader2 A2
#define fader3 A3
#define fader4 A4
#define fader5 A5
#define fader6 A6
#define fader7 A7
#define buttons A8

#define CA1 2
#define CA2 3
#define CA3 7

//Define pins for shift register
#define serialDataPin 11
#define clockPin 12
#define latchPin 13

#define countOf(a) (sizeof(a) / sizeof(a[0])) //Function to get count of items in arry
#define universeSize 8 //Set the size of the created DMX universe


byte num[] = {
  B01111110, // Zero
  B00110000, // One
  B01101101, // Two
  B01111001, // Three
  B00110011, // Four
  B01011011, // Five
  B01011111, // Six
  B01110000, // Seven
  B01111111, // Eight
  B01111011, // Nine
};

int buttonValue = 0; //int used to store which button is pressed

int faders[] = {fader0, fader1, fader2, fader3, fader4, fader5, fader6, fader7}; //Array for storing pins of faders
int levels[] = {0, 0, 0, 0, 0, 0, 0, 0}; //Array for storing level of all faders
int channels[] = {1, 2, 3, 4, 5, 6, 7, 8}; //Array for storing which DMX channels to send to
bool flash[] = {false, false, false, false, false, false, false, false}; //Array for storing state of flash buttons


ShiftRegister74HC595<2> leds(serialDataPin, clockPin, latchPin); //Initialize shift registers

void setup() {

  DMX.begin(universeSize); //Begin DMX universe with size defined above

#ifdef DEBUG
  Serial.begin(115200); //Begin serial for debugging
#endif

  for (int i = 0; i < countOf(faders); i++) { //Set all faders to inputs
    pinMode(faders[i], INPUT);
  }
}

void loop() {
  for (int i = 0; i < countOf(flash); i++) { //Reset flash value to false each loop and turn off LEDs
    leds.setNoUpdate(i + 8, LOW);
    if (flash[i]) {
      flash[i] = false;
    }
  }

  buttonValue = analogRead(buttons); //Read buttons to see which, if any, are pressed
  if (buttonValue > 800) {
    switch (buttonValue) {
      case 1009 ... 1017:
        flash[0] = true;
        leds.setNoUpdate(8, HIGH);
        break;
      case 999 ... 1007:
        flash[1] = true;
        leds.setNoUpdate(9, HIGH);
        break;
      case 989 ... 997:
        flash[2] = true;
        leds.setNoUpdate(10, HIGH);
        break;
      case 981 ... 988:
        flash[3] = true;
        leds.setNoUpdate(11, HIGH);
        break;
      case 969 ... 977:
        flash[4] = true;
        leds.setNoUpdate(12, HIGH);
        break;
      case 961 ... 968:
        flash[5] = true;
        leds.setNoUpdate(13, HIGH);
        break;
      case 952 ... 960:
        flash[6] = true;
        leds.setNoUpdate(14, HIGH);
        break;
      case 943 ... 951:
        flash[7] = true;
        leds.setNoUpdate(15, HIGH);
        break;
    }
  }

  for (int i = 0; i < countOf(faders); i++) { //Read the status of all faders and map between 0-255 for DMX
    levels[i] = analogRead(faders[i]);
    
    if (levels[i] > 2) { //If level is higher than 2 turn on corresponding LED
      leds.setNoUpdate(8 + i, HIGH);
    }
    
    levels[i] = map(levels[i], 0, 1023, 0, 255);
  }

  leds.updateRegisters(); //Update the shift registers with the correct status for all LEDs
  
  DMX.beginTransmission(); //Begin the DMX transmission

  for (int i = 0; i < countOf(levels); i++) {
    if (flash[i]) {
      DMX.write(channels[i], 255); //If flash is pressed set DMX channel to max
    }
    else {
      DMX.write(channels[i], levels[i]); //Set the level for all 8 DMX channels
    }
  }

  DMX.endTransmission(); //End the DMX transmission

  // START DEBUG
#ifdef DEBUG
  Serial.print("      Levels: "); //Print the levels faders are set at for debuging
  for (int i = 0; i < countOf(levels); i++) {
    Serial.print(levels[i]);
    Serial.print("  ");
  }

  Serial.println("");

  Serial.print("Flash states: "); //Print the flash status for each channel are set at for debuging
  for (int i = 0; i < countOf(levels); i++) {
    Serial.print(flash[i]);
    Serial.print("  ");
  }

  Serial.println("");
#endif
  // DEBUG END

  delay(100); //Slight delay in order not to overload the Arduino
}
