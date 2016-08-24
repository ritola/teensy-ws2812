#include <Adafruit_NeoPixel.h>

#define PIN 15
#define LEDS 22
#define BUTTONS 2

// Based on Adafruit Neopixel example https://github.com/adafruit/Adafruit_NeoPixel 

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDS, PIN, NEO_GRB + NEO_KHZ800);

int button_address[BUTTONS] = {8, 7};
 
void setup() {
  for (int i=0; i<BUTTONS; i++) pinMode(button_address[i],INPUT_PULLUP);
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

int mode = 0;
int larson_counter = 0;
int larson_colour = 0;
int strobo_pattern = 0;
int button[BUTTONS] = {HIGH, HIGH};

void loop() {
  read_buttons();

  switch (mode) {
    case 0: larson_scanner(larson_colour); break;
    case 1: strobo(strobo_pattern); break;
  }
}

void trigger_button(int button, int value) {
  if (value == LOW) {
    switch (button) {
      case 0:
        mode = ++mode % 2;
        larson_colour = 0;
        strobo_pattern = 0;
        break;
      case 1:
        larson_colour = ++larson_colour % 4;
        strobo_pattern = ++strobo_pattern % 4;
        break;
    } 
  }
}

int fademax = 9;
int fadeouts[4][3][10] = {{{ 0, 0, 0, 1, 2, 4, 16, 64, 128, 255 },
                           { 0, 0, 0, 0, 0, 1,  4, 16,  48, 136 },
                           { 0, 0, 0, 0, 0, 1,  2,  8,  16,  48 }},
                          {{ 0, 0, 0, 1, 2, 4, 16, 64, 128, 255 },
                           { 0, 0, 0, 0, 0, 0,  0,  0,   0,   0 },
                           { 0, 0, 0, 0, 0, 0,  0,  0,   0,   0 }},
                          {{ 0, 0, 0, 0, 0, 0,  0,  0,   0,   0 },
                           { 0, 0, 0, 1, 2, 4, 16, 64, 128, 255 },
                           { 0, 0, 0, 0, 0, 0,  0,  0,   0,   0 }},
                          {{ 0, 0, 0, 0, 0, 0,  0,  0,   0,   0 },
                           { 0, 0, 0, 0, 0, 0,  0,  0,   0,   0 },
                           { 0, 0, 0, 1, 2, 4, 16, 64, 128, 255 }}};
int fadelevels[LEDS] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0};
int scancount = 42;
int scanpos[42] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1 };

void larson_scanner(int colour) {
  if (++larson_counter >= scancount) larson_counter = 0;

  fadelevels[scanpos[larson_counter]] = fademax + 1;
  fadeout(colour);
  delay(50);
}

void fadeout(int colour) {
  for (int i=0; i<LEDS; i++) {
    if (fadelevels[i] > 0) fadelevels[i]--;
    int level_r = fadeouts[colour][0][fadelevels[i]];
    int level_g = fadeouts[colour][1][fadelevels[i]];
    int level_b = fadeouts[colour][2][fadelevels[i]];
    uint32_t c = strip.Color(level_r, level_g, level_b);
    strip.setPixelColor(i, c);
  }
  strip.show();
}

int strobo_pattern_length = 8;
int strobo_patterns[4][2][8] = {{{255, 0,   0, 0,   0, 0,   0, 0}, {255, 0,   0, 0,   0, 0,   0, 0}},
                                {{255, 0,   0, 0, 255, 0,   0, 0}, {255, 0,   0, 0, 255, 0,   0, 0}},
                                {{255, 0, 255, 0,   0, 0,   0, 0}, {  0, 0,   0, 0, 255, 0, 255, 0}},
                                {{  0, 0,   0, 0,   0, 0,   0, 0}, {  0, 0,   0, 0,   0, 0,   0, 0}}};
int strobo_pattern_position = 0;

void strobo(int pattern) {
  strobo_pattern_position = ++strobo_pattern_position % strobo_pattern_length;
  
  int on_off = strobo_patterns[pattern][0][strobo_pattern_position];
  
  for (int i=0; i<LEDS/2; i++) {
    uint32_t c = strip.Color(on_off, on_off, on_off);
    strip.setPixelColor(i, c);    
  }
  on_off = strobo_patterns[pattern][1][strobo_pattern_position];
  for (int i=LEDS/2; i<LEDS; i++) {
    uint32_t c = strip.Color(on_off, on_off, on_off);
    strip.setPixelColor(i, c);    
  }
  strip.show();
  delay(50);
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

int prev_button[BUTTONS] = {HIGH, HIGH};

void read_buttons() {
  for (int i=0; i<BUTTONS; i++) {
    button[i] = digitalRead(button_address[i]);
    if (prev_button[i] != button[i]) { trigger_button(i, button[i]); }
    prev_button[i] = button[i];
  }
}

