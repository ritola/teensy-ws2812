#include <Adafruit_NeoPixel.h>

#define PIN 18
#define LEDS 15

// Based on Adafruit Neopixel example https://github.com/adafruit/Adafruit_NeoPixel 

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(15, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

int fademax = 9;
int fadeouts[10] = { 0, 0, 0, 0, 0, 1, 4, 16, 64, 255 };
int fadelevels[LEDS] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0};
int scancount = 28;
int scanpos[28] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,13,12,11,10,9,8,7,6,5,4,3,2,1 };

void loop() {
  // Some example procedures showing how to display to the pixels:
  for(int i=0; i<scancount; i++) {
    fadelevels[scanpos[i]] = fademax + 1;
    fadeout();
    delay(50);
  }  
}

void fadeout() {
  for (int i=0; i<LEDS; i++) {
    if (fadelevels[i] > 0) fadelevels[i]--;
    int level = fadeouts[fadelevels[i]];
//    uint32_t c = strip.Color(level, 0, level / 2); // Pink
    uint32_t c = strip.Color(level, level, level); // White
    strip.setPixelColor(i, c);
  }
  strip.show();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

