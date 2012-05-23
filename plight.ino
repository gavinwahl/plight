#include <Arduino.h>
#include "SPI.h"
#include "LPD8806.h"

#define DATA_PIN_A 4
#define CLOCK_PIN_A 3
#define SENSOR_DATA_PIN 5


#define STRIP_LENGTH 72
#define ONE_LIGHT (STRIP_LENGTH/3)

#define ALL_OFF 0x000000
#define RED 0x00ff00
#define YELLOW 0x507f00 
#define GREEN 0xff0000

LPD8806 strip(STRIP_LENGTH, DATA_PIN_A, CLOCK_PIN_A);

void green_light();
void clear();
void set_light_color(int light, uint32_t color);

void setup() {
  pinMode(SENSOR_DATA_PIN, INPUT);
  strip.begin();
  clear();
  green_light();
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

int read_sensor()
{
  return ! digitalRead(SENSOR_DATA_PIN);
}

int fibs[] = {1, 1, 2, 3, 5};

void loop() {
  static unsigned long yellow_started = 0;
  static uint32_t current_state = GREEN;
  static int easteregg_stage = 0;
  static unsigned long last_close = 0;

  int button;

  button = read_sensor();

  if ( (current_state == GREEN || current_state == YELLOW) && button )
  {
    int desired_time = fibs[easteregg_stage] * 1000;
    unsigned long timedelta = millis() - last_close;
    if ( timedelta >= (desired_time - 900) && timedelta <= (desired_time + 900) )
      easteregg_stage++;
    else
      easteregg_stage = 0;
    last_close = millis();
  }
  if ( easteregg_stage == 5 )
  {
    set_light_color(0, GREEN);
    set_light_color(1, GREEN);
    set_light_color(2, GREEN);
    strip.show();
    delay(2000);
    easteregg_stage = 0;
  }


  if ( button )
  {
    red_light();
    current_state = RED;
    yellow_started = 0;
  }
  else if ( current_state == RED )
  {
    yellow_started = millis();
    yellow_light();
    current_state = YELLOW;
  }
  else if ( current_state == YELLOW )
  {
    // the second clause of the `||` is to account for rollover
    if ( millis() - yellow_started >= 2500 || millis() < yellow_started )
    {
      yellow_started = 0;
      green_light();
      current_state = GREEN;
    }
  }
}
