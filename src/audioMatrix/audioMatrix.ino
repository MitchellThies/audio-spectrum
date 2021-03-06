#include <FastLED.h>
#define NUM_LEDS 50
#define DATA_PIN 2
#define pushButtonPin 3
#define potPin A5
#define maxNumberPatterns 4
#define BRIGHTNESS  128

CRGB leds[NUM_LEDS];
bool prevButtonState = 0;
bool currentButtonState = 1;
int whichPattern = 0;

int potValue = 0;
int minDelay = 10;
int maxDelay = 100;

//Temperature information
#define TEMPERATURE_1 Tungsten100W
#define TEMPERATURE_2 OvercastSky

//Palette inforation
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

/*
  Arduino 3D audio spectrometer 
  by Mitchell Thies
  10-04-2020

  This code mainly merges the two codes listed below with some
  orginal code for mapping the 

  Based on the following codes:
  "Fiber Optic" LED Matrix
  By: jbumstead
  
  Arduino LED Audio Spectrum
  By: aprice27
*/


/*********BACK*********
   49 36 35 22 21 08 07
   48 37 34 23 20 09 06
   47 38 33 24 19 10 05
   46 39 32 25 18 11 04
   45 40 31 26 17 12 03
   44 41 30 27 16 13 02
   43 42 29 28 15 14 01
*********FRONT*********/

// matrix for the LEDs number
int matrix[7][7] = {
  {43, 44, 45, 46, 47, 48, 49},
  {42, 41, 40, 39, 38, 37, 36},
  {29, 30, 31, 32, 33, 34, 35},
  {28, 27, 26, 25, 24, 23, 22},
  {15, 16, 17, 18, 19, 20, 21},
  {14, 13, 12, 11, 10, 9, 8},
  {1, 2, 3, 4, 5, 6, 7}
};

int strobe = 4; // strobe pins on digital 4
int res = 5; // reset pins on digital 5
int left[7]; // store band values in these arrays
int right[7];
int band;
void setup()
{

  Serial.begin(115200);
  pinMode(res, OUTPUT); // reset
  pinMode(strobe, OUTPUT); // strobe

  digitalWrite(res, LOW); // reset low
  digitalWrite(strobe, HIGH); //pin 5 is RESET on the shield

  delay(2000);
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness( BRIGHTNESS );

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;

  pinMode(pushButtonPin, INPUT_PULLUP);
  pinMode(potPin, INPUT);
}

void loop()
{
  readMSGEQ7();


//  delay(100);
  // display values of left channel on serial monitor
  for (band = 0; band < 7; band++)
  {
    Serial.print(left[band]);
    Serial.print(" ");
  }
  Serial.println(" - Left ");
  // display values of right channel on serial monitor
  for (band = 0; band < 7; band++)
  {
    Serial.print(right[band]);
    Serial.print(" ");
  }
  Serial.println(" - Right" );

  currentButtonState = digitalRead(pushButtonPin);
  whichPattern = getWhichPattern(currentButtonState, prevButtonState, whichPattern);
  prevButtonState = currentButtonState;

  if (whichPattern == 0)
  {
  clearLED();

  graph(0, left[0]);
  graph(1, left[1]);
  graph(2, left[2]);
  graph(3, left[3]);
  graph(4, left[4]);
  graph(5, left[5]);
  graph(6, left[6]);

  FastLED.show();
  delay(20);
  
  }
  
  else if (whichPattern == 1) 
  {
  clearLED();

  graph2(0, left[0]);
  graph2(1, left[1]);
  graph2(2, left[2]);
  graph2(3, left[3]);
  graph2(4, left[4]);
  graph2(5, left[5]);
  graph2(6, left[6]);

  FastLED.show();
  delay(20);
    
  }

  else if (whichPattern == 2) 
  {
    cylonDisplay();
    
  }



  else if (whichPattern == 3) 
  {
    topBottomColor();

  }

  else if (whichPattern == 4) {


  }
}

//  
void graph(int col, int height)
{
  if (height > 75)
  {
    leds[matrix[col][0]] = CRGB::Red;
  }
  if (height > 100)
  {
    leds[matrix[col][1]] = CRGB::Red;
  }
  if (height > 150)
  {
    leds[matrix[col][2]] = CRGB::Red;
  }
  if (height > 225)
  {
    leds[matrix[col][3]] = CRGB::Red;
  }
  if (height > 300)
  {
    leds[matrix[col][4]] = CRGB::Red;
  }
  if (height > 400)
  {
    leds[matrix[col][5]] = CRGB::Red;
  }
  if (height > 525)
  {
    leds[matrix[col][6]] = CRGB::Red;
  }
//  FastLED.show();
}

void graph2(int col, int height)
{
  if (height > 75)
  {
    leds[matrix[col][0]] = CRGB::Green;
  }
  if (height > 100)
  {
    leds[matrix[col][1]] = CRGB::Green;
  }
  if (height > 150)
  {
    leds[matrix[col][2]] = CRGB::Green;
  }
  if (height > 225)
  {
    leds[matrix[col][3]] = CRGB::Green;
  }
  if (height > 300)
  {
    leds[matrix[col][4]] = CRGB::Yellow;
  }
  if (height > 400)
  {
    leds[matrix[col][5]] = CRGB::Yellow;
  }
  if (height > 525)
  {
    leds[matrix[col][6]] = CRGB::Red;
  }
//  FastLED.show();
}

void clearLED()
{
  for (int i = 0; i < NUM_LEDS; i++) 
  {
    leds[i] = CRGB::Black;
  }
//  FastLED.show();
}

void readMSGEQ7()
// Function to read 7 band equalizers
{
  digitalWrite(res, HIGH);
  digitalWrite(res, LOW);
  for (band = 0; band < 7; band++)
  {
    digitalWrite(strobe, LOW); // strobe pin on the shield - kicks the IC up to the next band
    delayMicroseconds(30); //
    left[band] = analogRead(0); // store left band reading
    right[band] = analogRead(1); // ... and the right
    digitalWrite(strobe, HIGH);
  }
}

// All code below is the "Fiber Optic" LED Matrix project
void cylonDisplay() {

  static uint8_t hue = 0;

  // First slide the led in one direction
  for (int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to red
    int delayTime = getDelayFromPot(5, 60);

    leds[i] = CHSV(hue++, 255, 255);
//    leds2[i] = CHSV(hue++, 255, 255);

    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();

    currentButtonState = digitalRead(pushButtonPin);
    whichPattern = getWhichPattern(currentButtonState, prevButtonState, whichPattern);
    prevButtonState = currentButtonState;

    if (whichPattern != 2) {
      return;
    }

    // Wait a little bit before we loop around and do it again
    delay(delayTime);
  }

  // Now go in the other direction.
  for (int i = (NUM_LEDS) - 1; i >= 0; i--) {
    // Set the i'th led to red

    int delayTime = getDelayFromPot(5, 60);

    leds[i] = CHSV(hue++, 255, 255);
//    leds2[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();

    currentButtonState = digitalRead(pushButtonPin);
    whichPattern = getWhichPattern(currentButtonState, prevButtonState, whichPattern);
    prevButtonState = currentButtonState;

    if (whichPattern != 2) {
      return;
    }

    // Wait a little bit before we loop around and do it again
    delay(delayTime);
  }

}

int getDelayFromPot(int minTime, int maxTime) {

  // Get pot read
  potValue = analogRead(potPin);
  int delayTime = potValue / 1023.0 * (maxTime - minTime) + minTime;
  // end pot read
  return delayTime;
}

void fadeall() {

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i].nscale8(250);
//    leds2[i].nscale8(250);
  }

}

int getWhichPattern(bool currentButtonState, bool prevButtonState, int whichPattern) {

  if (currentButtonState == 1) { // button is pressed down

    if (prevButtonState == 0) { // button pressed for first time

      blackOut();
      whichPattern = whichPattern + 1; // increment pattern
      if (whichPattern >= maxNumberPatterns) {
        whichPattern = 0;
      }
    }
  }

  return whichPattern;

}

void topBottomColor() {

  int colInd = getDelayFromPot(0, 255);

  for (int i = 0; i < NUM_LEDS; i++) {

    if (colInd > 100) {
      leds[i] = CRGB(colInd, 210, 40);
//      leds2[i] = CRGB(30, 100, colInd);
    }

    else {

      leds[i] = CRGB(colInd, 210, 40);
//      leds2[i] = CRGB(30, colInd, 100);
    }

  }

  FastLED.show();

}

void blackOut() {

  memset8( leds, 0, NUM_LEDS * sizeof(CRGB));
//  memset8( leds2, 0, NUM_LEDS * sizeof(CRGB));
  FastLED.show();
  FastLED.delay(20);

}
