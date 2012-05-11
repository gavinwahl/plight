#include <Arduino.h>
#include "SPI.h"
#include "LPD8806.h"

#define DATA_PIN_A 0
#define CLOCK_PIN_A 8
#define BUTTON_PIN 12

#define STRIP_LENGTH 6
#define ONE_LIGHT (STRIP_LENGTH/3)

#define ALL_OFF 0x000000
#define RED 0x00ff00
#define YELLOW 0xbfff00
#define GREEN 0xff0000

LPD8806 strip(STRIP_LENGTH, DATA_PIN_A, CLOCK_PIN_A);

void setup() {
  pinMode(BUTTON_PIN, INPUT);
  strip.begin();
  clear();
}


void clear() {
  int i;
  for ( i = 0; i < STRIP_LENGTH; i++ )
    strip.setPixelColor(i, ALL_OFF);
  strip.show();
}

void set_light_color(int light, uint32_t color)
{
  int i;
  for ( i = ONE_LIGHT * light; i < ONE_LIGHT * (light + 1); i++ )
    strip.setPixelColor(i, color);
} 

void red_light()
{
  set_light_color(0, ALL_OFF);
  set_light_color(1, ALL_OFF);
  set_light_color(2, RED);
  strip.show();
}

void green_light()
{
  set_light_color(0, GREEN);
  set_light_color(1, ALL_OFF);
  set_light_color(2, ALL_OFF);
  strip.show();
}

void yellow_light()
{
  set_light_color(0, ALL_OFF);
  set_light_color(1, YELLOW);
  set_light_color(2, ALL_OFF);
  strip.show();
}



void loop() {
  static int yellow_started = 0;
  static int was_yellow = 1;

  int button;

  button = digitalRead(BUTTON_PIN);

  if ( button )
  {
    was_yellow = 0;
    yellow_started = 0;
    red_light();
  }
  else if ( ! was_yellow )
  {
    if ( ! yellow_started )
      yellow_started = millis();

    if ( millis() - yellow_started >= 2500 )
    {
      yellow_started = 0;
      was_yellow = 1;
    }

    yellow_light();
  }
  else 
  {
    green_light();
    was_yellow = 1;
    yellow_started = 0;
  }
}
