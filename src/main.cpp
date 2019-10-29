#include "Clock.h"
#include "MultiStepper.h"
#include "qfplib-m3.h"
#include <Arduino.h>
#include <SPI.h>
#include <RTClock.h>
//MultiStepper mStepper;
Clock myClock;
RTClock rtclock(RTCSEL_LSE);
tm_t mytime;
int32_t endpos = 2000;
void setup() {
  mytime.year=2019-1970;
  mytime.month=10;
  mytime.day=27;
  mytime.hour=11;
  mytime.minute=38;
  mytime.second=30;
  // put your setup code here, to run once:
  pinMode(PC13, OUTPUT);

  Serial.begin(115200);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  myClock.setTargetAll(endpos,10,50,100);
  //rtclock.setTime(mytime);
}
uint32_t start, delta, end;
uint32_t counter = 10000000;
void loop() {
  start = millis() + 1000;

  // Serial.print("H: crap ");
  // Serial.println(rtclock.getTime());;
  while (millis() < start)
    myClock.run();
   Serial.print("H: ");
   Serial.print(rtclock.hour( ));
   Serial.print("M: ");
   Serial.print(rtclock.minute());
   Serial.print("S: ");
   Serial.println(rtclock.second());
   Serial.println(myClock.getMinutePos(0, 0));
  if (myClock.stepperDone(0)) {
    endpos = endpos * -1;
    myClock.setTargetAll(endpos, 10, 50, 100);
    // mStepper.setTarget(1, endpos, 100,200,1000);
  }
}
