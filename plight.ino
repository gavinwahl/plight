#include <Arduino.h>
#include "SPI.h"
#include "LPD8806.h"

#define DATA_PIN_A 4
#define CLOCK_PIN_A 3
#define SENSOR_DATA_PIN 5

#define STRIP_LENGTH 72
#define ONE_LIGHT (STRIP_LENGTH/3)

#define ALL_OFF 0x000000
#define RED strip.Color(32, 0, 0)
#define YELLOW strip.Color(24, 10, 0)
#define GREEN strip.Color(0, 16, 0)

#define DISCO 69
#define DISCO_FREQ 250

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

uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128;   //red down
      g = WheelPos % 128;      //green up
      b = 0;                  //blue off
      break;
    case 1:
      g = 127 - WheelPos % 128;  //green down
      b = WheelPos % 128;      //blue up
      r = 0;                  //red off
      break;
    case 2:
      b = 127 - WheelPos % 128;  //blue down
      r = WheelPos % 128;      //red up
      g = 0;                  //green off
      break;
  }
  return(strip.Color(r,g,b));
}

void party_time(uint8_t wait)
{
  for (int j=0; j < (384 * 6); j+=16) {     // cycle all 384 colors in the wheel six times
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 384));    //turn every third pixel on
        }
        strip.show();

        delay(wait);

        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
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
    if ( random(0, DISCO_FREQ) == 0 )
    {
      current_state = DISCO;
    }
    else
    {
      yellow_started = millis();
      yellow_light();
      current_state = YELLOW;
    }
  }
  else if ( current_state == DISCO )
  {
    party_time(50);
    current_state = GREEN;
  }
  else if ( current_state == YELLOW )
  {
    // the second clause of the `||` is to account for rollover
    if ( millis() - yellow_started >= 30000 || millis() < yellow_started )
    {
      yellow_started = 0;
      current_state = GREEN;
    }
    else {
      yellow_light();
    }
  }
  else
  {
    green_light();
  }
}
