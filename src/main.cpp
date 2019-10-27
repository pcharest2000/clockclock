#include "MultiStepper.h"
#include "qfplib-m3.h"
#include <Arduino.h>
#include <SPI.h>
#include <RTClock.h>
MultiStepper mStepper;
RTClock rtclock(RTCSEL_LSE);
int32_t endpos = 12000;
void setup() {

  // put your setup code here, to run once:
  pinMode(PC13, OUTPUT);

  Serial.begin(115200);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  mStepper.setTargetAll(endpos, 100, 200, 100);
  // rtclock.setTime(0);
}
uint32_t start, delta, end;
uint32_t counter = 10000000;
void loop() {
  start = millis() + 1000;

  // Serial.print("H: crap ");
  // Serial.println(rtclock.getTime());;
  while (millis() < start)
  mStepper.run();
  // Serial.print("H: ");
  // Serial.print(rtclock.getxs( ));
   Serial.print("M: ");
   Serial.print(rtclock.minute());
   Serial.print("S: ");
   Serial.println(rtclock.second());
   Serial.println(mStepper.getCurrentStep(0));
  if (mStepper.stepperDone(0)) {
    endpos = endpos * -1;
    mStepper.setTargetAll(endpos, 100, 200, 100);
    // mStepper.setTarget(1, endpos, 100,200,1000);

  }
}
