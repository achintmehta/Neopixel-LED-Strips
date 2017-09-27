#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 6
#define NUM_LEDS 300

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void showStrip() {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  FastLED.show();
#endif
}

typedef enum fillType_t {
  startFromStart,
  startFromEnd,
  meetInMiddle,
  runTowardsEnd,
  maxFillType
} fillType;



void setPixel(int Pixel, byte red, byte green, byte blue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
#endif
}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}
void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  //Serial.begin(9600);

  //randomSeed(analogRead(0));
  randomSeed(550);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  //Serial.print(WheelPos);
  //Serial.print(" ");
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void setPixelBrightness(uint32_t n, uint32_t brightness)
{
  uint32_t c = strip.getPixelColor(n);
  uint8_t
  r = (uint8_t)(c >> 16),
  g = (uint8_t)(c >>  8),
  b = (uint8_t)c;

  r = (r * brightness) >> 8;
  g = (g * brightness) >> 8;
  b = (b * brightness) >> 8;

  //Serial.println(b);
  strip.setPixelColor(n, r, g, b);

}

void wipeBrightnessFromOutsideToCenter(
  uint8_t wait,
  uint32_t brightness) {

  int32_t start_pixel = 0;
  int32_t end_pixel = strip.numPixels() - 1;

  /* Move towards collision point */
  while (start_pixel <= end_pixel)
  {
    setPixelBrightness(start_pixel++, brightness);
    setPixelBrightness(end_pixel--, brightness);
    strip.show();
    delay(wait);
  }


}

void wipeBrightnessFromCenterToOutside(
  uint8_t wait,
  uint32_t brightness) {

  int32_t start_pixel = strip.numPixels() / 2;
  int32_t end_pixel = start_pixel + 1;

  /* Move from collision point towards the end */
  while ((start_pixel >= 0) && (end_pixel <= strip.numPixels()))
  {
    if (start_pixel >= 0)
      setPixelBrightness(start_pixel--, brightness);
    if (end_pixel < strip.numPixels())
      setPixelBrightness(end_pixel++, brightness);
    strip.show();
    delay(wait);
  }

}


void wipeBrightnessFromStartToEnd(
  uint8_t wait,
  uint32_t brightness) {

  int32_t start_pixel = 0;
  int32_t end_pixel = strip.numPixels() - 1;

  /* Move from collision point towards the end */
  while (start_pixel < end_pixel)
  {
    setPixelBrightness(start_pixel, brightness);
    start_pixel++;
    strip.show();
    delay(wait);
  }

}

void wipeBrightnessFromEndToStart(
  uint8_t wait,
  uint32_t brightness) {

  int32_t end_pixel = strip.numPixels();

  /* Move from collision point towards the end */
  while ((end_pixel != 0))
  {
    //Serial.println(end_pixel);
    setPixelBrightness(end_pixel--, brightness);
    strip.show();
    delay(wait);
  }

}

void collision(
  uint8_t wait,
  uint32_t start_color,
  uint32_t end_color,
  uint32_t collision_color,
  uint8_t repeat_count,
  uint8_t clear_after_collision) {
  strip.clear();

  while (repeat_count-- != 0)
  {
    int32_t start_pixel = 0;
    int32_t end_pixel = strip.numPixels() - 1;

    /* Move towards collision point */
    while (start_pixel <= end_pixel)
    {

      strip.setPixelColor(start_pixel++, start_color);
      strip.setPixelColor(end_pixel--, end_color);
      strip.show();
      delay(wait);
    }

    if (clear_after_collision)
      strip.clear();

    /* Move from collision point towards the end */
    while ((start_pixel >= 1) && (end_pixel <= strip.numPixels()))
    {
      if (start_pixel > 0)
        strip.setPixelColor(start_pixel--, collision_color);
      if (end_pixel < strip.numPixels())
        strip.setPixelColor(end_pixel++, collision_color);
      strip.show();
      delay(wait);
    }
  }
}

void drawWorm(
  uint32_t position,
  uint8_t length,
  int8_t worm_direction,
  uint32_t worm_color)
{
  //Serial.print("Worm direction : ");
  //Serial.print(worm_direction);
  //Serial.print(" ");
  for (int i = 0; i < length; i++)
  {
    strip.setPixelColor(position + i, worm_color);
    //Serial.print(position + i);
    //Serial.print(" ");
  }

  if (worm_direction < 0) {
    setPixelBrightness(position + length - 1 , 20);
    setPixelBrightness(position + length - 2 , 45);

    /* Now clear the previous pixel */
    if (worm_direction == -2) { /* Means we have just changed direction */
      strip.setPixelColor(position + length, 0x00);
      strip.setPixelColor(position + length + 1, 0x00);
      //Serial.print(" Wiping -ve positions: ");
      //Serial.println(position + length  );
      //Serial.println(position + length + 1  );
    } else {
      strip.setPixelColor(position + length, 0x00);
      //Serial.print(" Wiping -ve position: ");
      //Serial.println(position + length );
    }
    //Serial.println("--" );
  } else {
    setPixelBrightness(position  , 0x20);
    setPixelBrightness(position + 1 , 0x45);
    /* Now clear the previous pixel */
    if (worm_direction == 2) { /* Means we have just changed direction */
      strip.setPixelColor(position -  1 , 0x00);
      strip.setPixelColor(position -  2 , 0x00);
      //Serial.print(" Wiping +ve position: ");
      //Serial.println(position - 1 );
      //Serial.println("--" );
    } else {
      strip.setPixelColor(position - 1 , 0x00);
      //Serial.print(" Wiping +ve position: ");
      //Serial.println(position - 1 );
      //Serial.println("--" );
    }
  }
}

bool fixWormDirection(
  int32_t start_pixel,
  int32_t end_pixel,
  uint32_t worm_position,
  uint32_t worm_length,
  int8_t &worm_direction)
{
  uint8_t touch_count = 0;
  /* Check if start of worm touches start_pixel */
  if (worm_position <= (start_pixel + 1))
  {
    touch_count += 1;
  }
  /* Check if end of worm touches end_pixel */
  if ((worm_position + worm_length) >= end_pixel)
  {
    touch_count += 2;
  }

  /* Check if worm touches both ends */
  if (touch_count == 3)
    return false; /* Cannot be fixed, being touched on both sides */

  if (touch_count == 1) {
    /*      Serial.print(worm_position);
          Serial.print(" ");

          Serial.print(start_pixel);
          Serial.print(" ");

          Serial.print(end_pixel);
          Serial.print(" ");
    */
    //Serial.println ("Changing direction 2");
    worm_direction = 2; /* Reverse worm direction */
  } else if (touch_count == 2) {
    worm_direction = -2; /* Reverse worm direction */
    //Serial.println ("Changing direction -2");
  } else {
    worm_direction = worm_direction < 0 ? -1 : 1;
  }

  return true;
}

void travellingWorm(
  uint32_t wait,
  uint32_t start_color,
  uint32_t end_color,
  uint32_t worm_color,
  uint8_t repeat_count) {
  //wait = 1000;
  while (repeat_count-- != 0) {
    strip.clear();

    int32_t start_pixel = 1;
    int32_t end_pixel = strip.numPixels();
    uint32_t worm_position = strip.numPixels() / 2;
    uint8_t worm_length = 10;
    int8_t worm_direction = -1;
    /* Move towards collision point */
    while (start_pixel <= end_pixel)
    {
      bool worm_trapped = false;
      /* First move the worm */
      for(int run_count =0; run_count < 3; run_count++) {
        if (false == fixWormDirection(start_pixel, end_pixel, worm_position, worm_length, worm_direction))
        {
          worm_trapped = true;
          break;
        }
        worm_position += worm_direction;
        drawWorm(worm_position, worm_length, worm_direction, worm_color);
        strip.show();
        delay(wait / 2);
      }
      
      /*
       
       if (false == fixWormDirection(
            start_pixel,
            end_pixel,
            worm_position,
            worm_length,
            worm_direction))
        break;
      worm_position += worm_direction;
      drawWorm(worm_position, worm_length, worm_direction, worm_color);
      */

      if (worm_trapped == true)
        break;

        /* Move the ends now after moving the worm */
      strip.setPixelColor(start_pixel++, start_color);
      strip.setPixelColor(end_pixel--, end_color);
      strip.show();

      delay(wait / 2);
    }


    /* Move from collision point towards the end */
    while ((start_pixel >= 1) && (end_pixel <= strip.numPixels()))
    {
      if (start_pixel > 0)
        strip.setPixelColor(start_pixel--, worm_color);
      if (end_pixel < strip.numPixels())
        strip.setPixelColor(end_pixel++, worm_color);
      strip.show();
      delay(wait / 2);
    }
  }

}


void stackSegments(uint8_t wait, int leds_per_stack ) {
  //int leds_per_stack = 10; /* make sure this is divisor of 300 */
  uint32_t current_leds_color[leds_per_stack];
  int i, j;
  int stack_end = strip.numPixels() - leds_per_stack;
  int current_led_color = 0;


  for (i = 0; i < stack_end; i++)
  {
    //Serial.println(stack_end);
    /* First get the color for the leds */
    for (j = 0; j < leds_per_stack; j++)
    {
      
      //current_leds_color[j] = Wheel(current_led_color++ * 256 / strip.numPixels());
      current_leds_color[j] = Wheel(current_led_color++ & 255);
    }

    //Serial.println("");
    /* Color one segment and move it till the stack end  */
    int segment_start_location = 0;
    /* Move the segment to end */
    for (segment_start_location = 0; segment_start_location < stack_end; segment_start_location ++)
    {

      /* Color the segment */
      for (j = 0; j < leds_per_stack; j++)
      {
        strip.setPixelColor(segment_start_location + j + 1, current_leds_color[j]);
      }

      strip.show();
      delay(wait);

      /* Mark the previous LED as black */
      if (segment_start_location != stack_end)
        strip.setPixelColor(segment_start_location, 0x00);
    }

    stack_end -= leds_per_stack;

  }
  //Serial.println("Done");
}



void fillStripWithColor(
  uint32_t color,
  uint32_t wait,
  fillType fillType)
{
  switch (fillType)
  {
    case startFromStart:
      {
        int32_t start_pixel = 0, end_pixel = strip.numPixels() - 1;

        while (start_pixel != end_pixel)
        {
          strip.setPixelColor(start_pixel, color);
          strip.show();
          start_pixel++;
          delay(wait);
        }
      }
      break;
    case startFromEnd:
      {
        int32_t start_pixel = 0, end_pixel = strip.numPixels() - 1;

        //Serial.println("Coloring strip");
        while (start_pixel != end_pixel)
        {
          strip.setPixelColor(end_pixel, color);
          strip.show();
          end_pixel--;
          delay(wait);
        }
        //Serial.println("Done with coloring strip");
      }
      break;
    case meetInMiddle:
      {
        int32_t start_pixel = 0, end_pixel = strip.numPixels() - 1;

        while (start_pixel <= end_pixel)
        {
          strip.setPixelColor(start_pixel++, color);
          strip.setPixelColor(end_pixel--, color);
          strip.show();
          delay(wait);
        }
      }
      break;
    case runTowardsEnd:
      {
        int32_t start_pixel = strip.numPixels() / 2;
        int32_t end_pixel = start_pixel + 1;

        while ((start_pixel > 0) && (end_pixel <=  strip.numPixels()))
        {
          if (start_pixel > 0) strip.setPixelColor(start_pixel--, color);
          if (end_pixel <=  strip.numPixels()) strip.setPixelColor(end_pixel++, color);
          strip.show();
          delay(wait);
        }
      }
      break;
  }
}

void wipeStripBrightness (
  fillType type)
{
  switch (type)
  {
    case startFromStart:
      wipeBrightnessFromStartToEnd(5, 128);
      wipeBrightnessFromStartToEnd(3, 64);
      wipeBrightnessFromStartToEnd(1, 0);
      break;
    case startFromEnd:
      wipeBrightnessFromEndToStart(5, 128);
      wipeBrightnessFromEndToStart(3, 64);
      wipeBrightnessFromEndToStart(1, 0);
      break;
    case meetInMiddle:
      wipeBrightnessFromOutsideToCenter(5, 128);
      wipeBrightnessFromOutsideToCenter(3, 64);
      wipeBrightnessFromOutsideToCenter(1, 0);
      break;
    case runTowardsEnd:
      wipeBrightnessFromCenterToOutside(5, 128);
      wipeBrightnessFromCenterToOutside(3, 64);
      wipeBrightnessFromCenterToOutside(1, 0);
      break;
  }
}

void fillWithColorAndWipe(
  uint32_t color,
  fillType type)
{
  if (type == -1)
    type = (fillType)random(0, maxFillType);

  //type = (fillType)1;
  
  //Serial.print("FillwithColorAndWipe type: ");
  Serial.println(type);
  switch (type)
  {
    case startFromStart:
      if (color == 0) color = strip.getPixelColor(0);
      fillStripWithColor(color, 5, type);
      wipeBrightnessFromStartToEnd(5, 128);
      wipeBrightnessFromEndToStart(1, 0);
      break;
    case startFromEnd:
      if (color == 0) color = strip.getPixelColor(strip.numPixels() - 1);
      fillStripWithColor(color, 5, type);
      //Serial.println("Wipe 1");
      wipeBrightnessFromEndToStart(5, 128);
      //Serial.println("Wipe 2");
      wipeBrightnessFromStartToEnd(3, 0);
      break;
    case meetInMiddle:
      if (color == 0)
        color = strip.getPixelColor(strip.numPixels() / 2);
      fillStripWithColor(color, 5, type);
      wipeBrightnessFromOutsideToCenter(5, 128);
      wipeBrightnessFromCenterToOutside(1, 0);
      break;
    case runTowardsEnd:
      if (color == 0)
        color = strip.getPixelColor(strip.numPixels() / 2);

      fillStripWithColor(
        color,
        5,
        type);

      wipeBrightnessFromCenterToOutside(5, 128);
      wipeBrightnessFromOutsideToCenter(1, 0);
      break;
  }
}

void loop() {
  RunRandomEffect(-1);
}


void StartFire(unsigned long duration /* milliseoncds */)
{
  unsigned long start_time = millis();
  while ((millis() - start_time) < duration)
  {
    Fire(55, 120, 15);
  }


}
void BouncingColoredBalls(int BallCount, byte simple_colors[][3]) {
  float Gravity = -9.81;
  int StartHeight = 1;
  int repeatCount = 100;
  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];
  int timeout = 10000;

  unsigned long start_time = millis();

  for (int i = 0 ; i < BallCount ; i++) {
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i) / pow(BallCount, 2);
  }

  while (true) {
    if ((millis() - start_time) > timeout)
      break;

    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i] / 1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i] / 1000;

      if ( Height[i] < 0 ) {
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();

        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * (NUM_LEDS - 1) / StartHeight);
    }

    for (int i = 0 ; i < BallCount ; i++) {
      setPixel(Position[i], simple_colors[i][0], simple_colors[i][1], simple_colors[i][2]);
    }

    showStrip();
    setAll(0, 0, 0);
  }
}
void BouncingBalls(byte red, byte green, byte blue, int BallCount) {
  float Gravity = -9.81;
  int StartHeight = 1;
  int timeout = 10000;
  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];

  unsigned long start_time = millis();

  for (int i = 0 ; i < BallCount ; i++) {
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i) / pow(BallCount, 2);
  }

  while (true) {

    if ((millis() - start_time) > timeout)
      break;
    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i] / 1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i] / 1000;

      if ( Height[i] < 0 ) {
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();

        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * (NUM_LEDS - 1) / StartHeight);
    }

    for (int i = 0 ; i < BallCount ; i++) {
      setPixel(Position[i], red, green, blue);
    }

    showStrip();
    setAll(0, 0, 0);
  }
}
void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);

    if (cooldown > heat[i]) {
      heat[i] = 0;
    } else {
      heat[i] = heat[i] - cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if ( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160, 255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for ( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature / 255.0) * 191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252

  // figure out which third of the spectrum we're in:
  if ( t192 > 0x80) {                    // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if ( t192 > 0x40 ) {            // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}

void RunningLights(byte red, byte green, byte blue, int WaveDelay) {
  int Position = 0;

  for (int i = 0; i < NUM_LEDS * 2; i++)
  {
    Position++; // = 0; //Position + Rate;
    for (int i = 0; i < NUM_LEDS; i++) {
      // sine wave, 3 offset waves make a rainbow!
      //float level = sin(i+Position) * 127 + 128;
      //setPixel(i,level,0,0);
      //float level = sin(i+Position) * 127 + 128;
      setPixel(i, ((sin(i + Position) * 127 + 128) / 255)*red,
               ((sin(i + Position) * 127 + 128) / 255)*green,
               ((sin(i + Position) * 127 + 128) / 255)*blue);
    }

    showStrip();
    delay(WaveDelay);
  }
}
void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay, uint32_t duration) {
  unsigned long start_time = millis();
  while ((millis() - start_time) < duration)
  {
    setAll(red, green, blue);

    int Pixel = random(NUM_LEDS);
    setPixel(Pixel, 0xff, 0xff, 0xff);
    showStrip();
    delay(SparkleDelay);
    setPixel(Pixel, red, green, blue);
    showStrip();
    delay(SpeedDelay);
  }
}

void Sparkle(byte red, byte green, byte blue, int SpeedDelay, int num_leds, uint32_t duration) {
  unsigned long start_time = millis();
  while ((millis() - start_time) < duration)
  {
    for (int i = 0; i < num_leds; i++)
    {
      int Pixel = random(NUM_LEDS);
      setPixel(Pixel, red, green, blue);
    }
    showStrip();
    delay(SpeedDelay);
    setAll( 0, 0, 0);
  }
}
void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {

  setAll(0, 0, 0);
  for (int i = 0; i < Count; i++) {
    setPixel(random(NUM_LEDS), random(0, 255), random(0, 255), random(0, 255));
    showStrip();
    delay(SpeedDelay);
    if (OnlyOne) {
      setAll(0, 0, 0);
    }
  }

  delay(SpeedDelay);
}
void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0, 0, 0);

  for (int i = 0; i < Count; i++) {
    setPixel(random(NUM_LEDS), red, green, blue);
    showStrip();
    delay(SpeedDelay);
    if (OnlyOne) {
      setAll(0, 0, 0);
    }
  }

  delay(SpeedDelay);
}
void NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}

void CenterToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = ((NUM_LEDS - EyeSize) / 2); i >= 0; i--) {
    setAll(0, 0, 0);

    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);

    setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS - i - j, red, green, blue);
    }
    setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void OutsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = 0; i <= ((NUM_LEDS - EyeSize) / 2); i++) {
    setAll(0, 0, 0);

    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);

    setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS - i - j, red, green, blue);
    }
    setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void LeftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void RightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}
void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {

  for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);

  for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);
}
void FadeInOut(byte red, byte green, byte blue) {
  float r, g, b;

  for (int k = 0; k < 256; k = k + 1) {
    r = (k / 256.0) * red;
    g = (k / 256.0) * green;
    b = (k / 256.0) * blue;
    setAll(r, g, b);
    showStrip();
  }

  for (int k = 255; k >= 0; k = k - 2) {
    r = (k / 256.0) * red;
    g = (k / 256.0) * green;
    b = (k / 256.0) * blue;
    setAll(r, g, b);
    showStrip();
  }
}
void RGBLoop() {
  for (int j = 0; j < 3; j++ ) {
    // Fade IN
    for (int k = 0; k < 256; k++) {
      switch (j) {
        case 0: setAll(k, 0, 0); break;
        case 1: setAll(0, k, 0); break;
        case 2: setAll(0, 0, k); break;
      }
      showStrip();
      delay(3);
    }
    // Fade OUT
    for (int k = 255; k >= 0; k--) {
      switch (j) {
        case 0: setAll(k, 0, 0); break;
        case 1: setAll(0, k, 0); break;
        case 2: setAll(0, 0, k); break;
      }
      showStrip();
      delay(3);
    }
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}



void rainbow(uint8_t wait, uint32_t duration) {
  uint16_t i, j;
  unsigned long start_time = millis();
  while ((millis() - start_time) < duration)
  {
    for (j = 0; j < 256; j++) {
      for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, Wheel((i + j) & 255));
        //Serial.print(Wheel((i + j) & 255));
        //Serial.print(" ");
      }

      Serial.println("");
      
      strip.show();
      delay(wait);
    }
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait, uint32_t duration) {
  uint16_t i, j;
  unsigned long start_time = millis();

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
    if ((millis() - start_time) > duration)
      break;
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait, uint32_t duration) {
  unsigned long start_time = millis();
  while ((millis() - start_time) < duration)
  {
    for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
      for (int q = 0; q < 3; q++) {
        for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
          strip.setPixelColor(i + q, c);  //turn every third pixel on
        }
        strip.show();

        delay(wait);

        for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
          strip.setPixelColor(i + q, 0);      //turn every third pixel off
        }
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait, uint32_t duration) {
  unsigned long start_time = millis();
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }

    if ((millis() - start_time) > duration)
      break;

  }
}

void RunRandomEffect (int effect_number)
{
  const int max_number_effects = 20;
  const int max_simple_colors = 6;
  int simple_color_index = 0;
  static int last_effect_number = -1;

  /* This is array of simple colors that would not use much current */
  byte simple_colors[max_simple_colors][3] = { 
    {0xff, 0, 0},
    {00, 0xff, 0},
    {00, 0   , 0xff},
    {86, 38  , 137},
    {104,7   , 119},
    {5, 69   , 104} 
  };

  const int max_complex_colors = 11;
  int complex_color_index = 0;

  const int max_num_effect = 8;
  /* This is array of simple colors that would not use much current */
  byte complex_colors[max_complex_colors][3] = {
    {0xff, 0, 0},
    {0, 0xff, 0},
    {0   , 0   , 0xff},
    {0xa4, 0x25, 0xff},
    {0xff, 0x4f, 0x27},
    {0x59, 0xff, 0x77},
    {0xff, 0xf4, 0x29},
    {0x78, 0x11, 0xff},
    {0x20, 0xff, 0xaa},
    {0xff, 0x28, 0x70},
    {0xb9, 0xff, 0x36}
  };

  while ((effect_number <= 0) || (effect_number == last_effect_number))
  {
    effect_number = random(1, max_number_effects  + 1);
  }

  last_effect_number = effect_number;

  //effect_number = 20;
  
  Serial.print("Running effect ");
  Serial.println(effect_number);
  switch (effect_number)
  {
    case 1:
      break;
      BouncingColoredBalls(max_complex_colors, complex_colors);
      simple_color_index = random(0, max_simple_colors);
      fillWithColorAndWipe(
        strip.Color(simple_colors[simple_color_index][0],
                    simple_colors[simple_color_index][1],
                    simple_colors[simple_color_index][2]),
        (fillType) - 1);

      break;

    case 2:
      break;
      simple_color_index = random(0, max_simple_colors);
      BouncingBalls(
        simple_colors[simple_color_index][0], simple_colors[simple_color_index][1], simple_colors[simple_color_index][2], 3);

      fillWithColorAndWipe(
        strip.Color(simple_colors[simple_color_index][0],
                    simple_colors[simple_color_index][1],
                    simple_colors[simple_color_index][2]),
        meetInMiddle);
      break;


    case 3:
      break;
      stackSegments(1, 20   );
      simple_color_index = random(0, max_simple_colors);
      fillWithColorAndWipe(
        strip.Color(simple_colors[simple_color_index][0],
                    simple_colors[simple_color_index][1],
                    simple_colors[simple_color_index][2]),
        (fillType) - 1);
      break;

    case 4:
      {
        RGBLoop();
        wipeBrightnessFromStartToEnd(1, 0);
      }
      break;

    case 5:
      break;
      StartFire(45000 /* milliseconds */);
      wipeBrightnessFromStartToEnd(5, 100);
      wipeBrightnessFromEndToStart(1, 0);
      break;

    case 6:
      {
        uint32_t repeat_count = random(4, 10);
        for (int i = 0; i < repeat_count; i++)
        {
          simple_color_index = random(0, max_simple_colors);

          FadeInOut(simple_colors[simple_color_index][0], simple_colors[simple_color_index][1], simple_colors[simple_color_index][2]); // red
        }
        fillWithColorAndWipe(
          strip.Color(simple_colors[simple_color_index][0],
                      simple_colors[simple_color_index][1],
                      simple_colors[simple_color_index][2]),
          (fillType) - 1);
      }
      break;

    case 7:
      {
      break;
        uint8_t repeat_count = random(2, 5);
        //Serial.print("Repat count is ");
        //Serial.println(repeat_count);
        
        for (int i = 0; i < repeat_count; i++) {
          simple_color_index = random(0, max_simple_colors);
          CylonBounce(
            simple_colors[simple_color_index][0],
            simple_colors[simple_color_index][1],
            simple_colors[simple_color_index][2],
            85,
            5,
            5);
        }
        fillWithColorAndWipe(
          strip.Color(simple_colors[simple_color_index][0],
                      simple_colors[simple_color_index][1],
                      simple_colors[simple_color_index][2]),
          (fillType) - 1);
      }
      break;

    case 8:
      break;
      simple_color_index = random(0, max_simple_colors);
      NewKITT(simple_colors[simple_color_index][0], simple_colors[simple_color_index][1], simple_colors[simple_color_index][2], 40, 5, 5);
      fillWithColorAndWipe(
        strip.Color(simple_colors[simple_color_index][0],
                    simple_colors[simple_color_index][1],
                    simple_colors[simple_color_index][2]),
        (fillType) - 1);
      break;

    case 9:
      {
      break;
        uint8_t repeat_count = random(1, 3);
        for (int i = 0; i <= repeat_count; i++) {
          simple_color_index = random(0, max_simple_colors);
          Twinkle(
            simple_colors[simple_color_index][0],
            simple_colors[simple_color_index][1],
            simple_colors[simple_color_index][2],
            160,
            100,
            false);
        }

        fillWithColorAndWipe(
          strip.Color(simple_colors[simple_color_index][0],
                      simple_colors[simple_color_index][1],
                      simple_colors[simple_color_index][2]),
          (fillType) - 1);
      }
      break;

    case 10:
      {
      break;
        uint8_t repeat_count = random(2, 4);
        for (int i = 0; i <= repeat_count; i++) {
          TwinkleRandom(100, 100, false);
        }
      }
      break;

    case 11:
      {
      break;
        uint8_t repeat_count = random(2, 4);
        for (int i = 0; i <= repeat_count; i++) {
          uint8_t color_index = random(0, max_complex_colors);
          Sparkle(
            complex_colors[color_index ][0],
            complex_colors[color_index ][1],
            complex_colors[color_index ][2],
            150,
            50,
            15000);
        }
      }
      break;

    case 12:
      break;
      SnowSparkle(0x10, 0x10, 0x10, 20, random(100, 500), 15000);
      wipeBrightnessFromCenterToOutside(5, 100);
      wipeBrightnessFromOutsideToCenter(1, 0);
      break;

    case 13:
      simple_color_index = random(0, max_simple_colors);
      RunningLights(simple_colors[simple_color_index][0], simple_colors[simple_color_index][1], simple_colors[simple_color_index][2], 50);
      wipeBrightnessFromCenterToOutside(5, 100);
      wipeBrightnessFromOutsideToCenter(1, 0);
      break;

    case 14:
      {
        for (int i = 0; i < random(2, 8); i++) {
          simple_color_index = random(0, max_simple_colors);
          colorWipe(strip.Color(simple_colors[simple_color_index][0], simple_colors[simple_color_index][1], simple_colors[simple_color_index][2]), 3);
          wipeBrightnessFromCenterToOutside(5, 100);
          wipeBrightnessFromOutsideToCenter(1, 0);
        }
      }
      break;

    case 15:
      simple_color_index = random(0, max_simple_colors);
      theaterChase(strip.Color(simple_colors[simple_color_index][0], simple_colors[simple_color_index][1], simple_colors[simple_color_index][2]), 100, 80000);
      wipeBrightnessFromCenterToOutside(5, 100);
      wipeBrightnessFromOutsideToCenter(1, 0);

      break;

    case 16:
      rainbow(20, 85000);
      wipeBrightnessFromCenterToOutside(5, 100);
      wipeBrightnessFromOutsideToCenter(1, 0);
      break;

    case 17:
      rainbowCycle(20, 85000);
      wipeBrightnessFromCenterToOutside(5, 100);
      wipeBrightnessFromOutsideToCenter(1, 0);
      break;

    case 18:
      theaterChaseRainbow(100, 90000);
      wipeBrightnessFromOutsideToCenter(1, 0);
      
      break;

    case 19:
      {
        uint32_t start_color, end_color, collision_color;
        int color_index = random(0, max_simple_colors);
        start_color = strip.Color(simple_colors[color_index][0], simple_colors[color_index][1], simple_colors[color_index][2]);

        if (++color_index == max_simple_colors)
          color_index = 0;
        end_color = strip.Color(simple_colors[color_index][0], simple_colors[color_index][1], simple_colors[color_index][2]);

        if (++color_index == max_simple_colors)
          color_index = 0;
        collision_color = strip.Color(simple_colors[color_index][0], simple_colors[color_index][1], simple_colors[color_index][2]);

        collision(
          20,
          start_color,
          end_color,
          collision_color,
          random(3, 5),
          random(0, 2));

      wipeBrightnessFromCenterToOutside(5, 100);
      wipeBrightnessFromOutsideToCenter(1, 0);

        break;
      }
    case 20:
      uint32_t start_color, end_color, worm_color;
      int color_index = random(0, max_simple_colors);
      start_color = strip.Color(simple_colors[color_index][0], simple_colors[color_index][1], simple_colors[color_index][2]);

      if (++color_index == max_simple_colors)
        color_index = 0;
      end_color = strip.Color(simple_colors[color_index][0], simple_colors[color_index][1], simple_colors[color_index][2]);

      if (++color_index == max_simple_colors)
        color_index = 0;
      worm_color = strip.Color(simple_colors[color_index][0], simple_colors[color_index][1], simple_colors[color_index][2]);

      travellingWorm(
        10,
        start_color,
        end_color,
        worm_color,
        random(1, 4)
      );

      wipeBrightnessFromCenterToOutside(5, 100);
      wipeBrightnessFromOutsideToCenter(1, 0);


      break;
  }

}

