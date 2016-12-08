#include <Adafruit_NeoPixel.h>

#define PIN 15
#define LEDS 38
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

int button_address[BUTTONS] = {8, 9};

// Goggle leds are located in the following positions
// 
//      24 25 26           05 06 07
//   23  33  34  27     04  14  15  08
//  22  32 31 35  28   03  13 12 16  09
//   21  37  36  29     02  18  17  10
//      20 19 30           01 00 11


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
    case 0: off(); break;
    case 1: oneten(); break;
    case 2: strobo(strobo_pattern); break;
    case 4: larson_scanner(larson_colour); break;
  }
}

void trigger_button(int button, int value) {
  if (value == LOW) {
    switch (button) {
      case 0:
        mode = ++mode % 3;
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
int fadelevels[LEDS + 1] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0};
int scancount = 20;
int scanpos[20] = { 9,16,12,13,3,LEDS,28,35,31,32,22,32,31,35,28,LEDS,3,13,12,16 };

void larson_scanner(int colour) {
  if (++larson_counter >= scancount) larson_counter = 0;

  fadelevels[scanpos[larson_counter]] = fademax + 1;
  fadeout(colour);
  delay(80);
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
int strobo_patterns[4][2][8] = {{{255, 0, 255, 0,   0, 0,   0, 0}, {  0, 0,   0, 0, 255, 0, 255, 0}},
                                {{255, 0, 255, 0,   0, 0,   0, 0}, {  0, 0,   0, 0, 255, 0, 255, 0}},
                                {{255, 0,   0, 0, 255, 0,   0, 0}, {255, 0,   0, 0, 255, 0,   0, 0}},
                                {{255, 0,   0, 0,   0, 0,   0, 0}, {255, 0,   0, 0,   0, 0,   0, 0}}};
int strobo_pattern_position = 0;

void strobo(int pattern) {
  strobo_pattern_position = ++strobo_pattern_position % strobo_pattern_length;
  
  boolean police = (pattern == 0);
  int on_off = strobo_patterns[pattern][0][strobo_pattern_position];
  int police_on_off = (police) ? 0 : on_off;
  
  for (int i=0; i<LEDS/2; i++) {
    uint32_t c = strip.Color(on_off, police_on_off, police_on_off);
    strip.setPixelColor(i, c);    
  }
  
  on_off = strobo_patterns[pattern][1][strobo_pattern_position];
  police_on_off = (police) ? 0 : on_off;

  for (int i=LEDS/2; i<LEDS; i++) {
    uint32_t c = strip.Color(police_on_off, police_on_off, on_off);
    strip.setPixelColor(i, c);    
  }
  strip.show();
  delay(50);
}

void off() {
  uint32_t c = strip.Color(0, 0, 0);
  for (int i=0; i<LEDS; i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
  delay(100);
}

int oneten_count = 16;
int oneten_on[16] = { 21,22,23, 29,28,27, 2,3,4,18,14,17,15,10,9,8 };
int wheelpos = 0;

void oneten() {
  wheelpos = (wheelpos + 1) % 256;
  for (int i=0; i<oneten_count; i++) {
    uint32_t c = wheel((i + wheelpos) % 256);
    strip.setPixelColor(oneten_on[i], c);
  }
  strip.show();
  delay(50);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t wheel(byte WheelPos) {
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

