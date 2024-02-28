/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/

#include <FastLED.h>
#include <Arduino.h>

FASTLED_USING_NAMESPACE

// FAST LED
#define DATA_PIN    14
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

#define BRIGHTNESS          255
//#define FRAMES_PER_SECOND  120

#define LEDS_PER_RING 16
#define RINGS_PER_DECADE 3
#define DECADES 3

#define NUM_LEDS (LEDS_PER_RING * RINGS_PER_DECADE * DECADES)

#define LOOP_FQ 10  // Hz
#define MAX_LOOP_DELTA_MS  10 // ms
const double loop_period_ms = 1000. / LOOP_FQ; // ms

CRGB leds[NUM_LEDS];
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

enum TubeColor {
    RED=0,
    BLUE,
    YELLOW
};

TubeColor intToTubeColor(int i) {
  switch(i) {
    case 0:
      return TubeColor::RED;
    case 1:
      return TubeColor::BLUE;
    case 2:
      return TubeColor::YELLOW;
    // Add more cases as needed for different colors
    default:
      return TubeColor::RED; // Default color if the index is out of range
  }
}


static uint brightest_led_ix = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(115200);
}


// the loop function runs over and over again forever
void loop() {
  static bool led_state = HIGH;
  maintainTiming();
  printState();
  digitalWrite(LED_BUILTIN, led_state);   // turn the LED on (HIGH is the voltage level)
  led_state = !led_state;
  clear_leds(leds, NUM_LEDS);
  brightest_led_ix = (brightest_led_ix + 1) % LEDS_PER_RING;
  for(int i=0; i<DECADES; i++) {
    TubeColor tc= intToTubeColor(i);
    for (int j=0; j<RINGS_PER_DECADE; j++) {
      // Calculate the starting index for each ring
      int start_index = j * LEDS_PER_RING + i * LEDS_PER_RING * RINGS_PER_DECADE;
      fill_ring(leds + start_index, brightest_led_ix, tc);
    }
  }
  checkTimingViolation();
  FastLED.show();
}


void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void fill_ring(CRGB * leds, uint brightest_led_ix, TubeColor color)
{
  for (uint i=0; i<LEDS_PER_RING; i++)
  {
    uint index = (i + brightest_led_ix) % LEDS_PER_RING;
    uint8_t intensity = 255;//max((uint8_t)0, (uint8_t)(255 - i * (255./(double)(LEDS_PER_RING-1))));
    switch (color)
    {
      case TubeColor::RED:
        leds[index] = CRGB(intensity, 0, 0);
        break;
      case TubeColor::BLUE:
        leds[index] = CRGB(0, 0, intensity);    
        break;
      case TubeColor::YELLOW:
        leds[index] = CRGB(intensity, intensity, 0);    
        break;
      default:
        break;
    }
  }
}

void clear_leds(CRGB *leds, int num_leds) {
  for (int i = 0; i < num_leds; i++) {
    leds[i] = CRGB::Black;
  }
}  
  
void printState()
{
  Serial.print("loop at:\t");
  Serial.println(millis() / 1000.);
}


void nbDelay(uint interval) {
  unsigned long initial_milli = millis();
  while (millis() - initial_milli < interval) {
    // Loop until the interval has passed
  }
  return;
}


unsigned long previous_millis_ms = 0;
volatile uint16_t count;
void maintainTiming() {
  while (millis() - previous_millis_ms < loop_period_ms) {
    // Wait until the interval has passed
    count++;
  }
  previous_millis_ms = millis();
}


void checkTimingViolation() {
  unsigned long current_millis = millis();
  if (current_millis - previous_millis_ms > MAX_LOOP_DELTA_MS) {
//#ifdef DEBUG
    Serial.print("Timing violation: Loop running too slow!\t");
    Serial.print(current_millis);
    Serial.print(" - ");
    Serial.print(previous_millis_ms);
    Serial.print(" = ");
    Serial.print(current_millis - previous_millis_ms);
    Serial.print(" > ");
    Serial.println(MAX_LOOP_DELTA_MS);    
//#endif
  }
}
