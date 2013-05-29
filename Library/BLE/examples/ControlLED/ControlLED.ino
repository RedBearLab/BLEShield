/*
  This example will turn ON/OFF LED on pin 13 when receiving
  command 0x01/0x00.
*/

#include <SPI.h>
#include <ble.h>

/* PIN for LED */
#define PIN    13

/* Command: ON/OFF */
#define ON     0x01
#define OFF    0x00

void setup()
{
  pinMode(PIN, OUTPUT);
  
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(LSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.begin();

  ble_begin();
}

void loop()
{
  if (ble_available())
  {
    byte r = ble_read();
    if (r == ON)
      digitalWrite(PIN, HIGH);
    else if (r == OFF)
      digitalWrite(PIN, LOW);
  }
  
  ble_do_events();
}
