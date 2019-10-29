/*
 *  MultiStepper Arduino Library
 *  Guy Carpenter, Clearwater Software - 2012
 *
 *  Licensed under the BSD2 license, see license.txt for details.
 *
 *  All text above must be included in any redistribution.
 */

#include "MultiStepper.h"
#include <Arduino.h>

// This table defines the acceleration curve as a list of (step, delay) pairs.
// 1st value is the cumulative step count since starting from rest, 2nd value is
// delay in microseconds. 1st value in each subsequent row must be > 1st value
// in previous row The delay in the last row determines the maximum angular
// with my hand-made needles made by cutting up aluminium from
// floppy disk sliders.  A lighter needle will go faster.

// State  3 2 1 0   Value
// 0      1 0 0 1   0x9
// 1      0 0 0 1   0x1
// 2      0 1 1 1   0x7
// 3      0 1 1 0   0x6
// 4      1 1 1 0   0xE
// 5      1 0 0 0   0x8
static uint8_t stepperState[] = {0b1001, 0b0001, 0b0111, 0b0110, 0b1110, 0b1000};
MultiStepper::MultiStepper() {
  for (uint32_t i = 0; i < NUMSTEPPERS; i++) {
    steppers[i].currentStepState = 0;
    steppers[i].dir = CW;
    steppers[i].accelIndex = 0;
    steppers[i].currentStep = 0;
    steppers[i].targetStep = 0;
  }
}
void MultiStepper::zeroAll() {
  for (uint32_t i = 0; i < NUMSTEPPERS; i++) {
    steppers[i].currentStep = 0;
  }
}
bool MultiStepper::stepperDone(uint8_t i) {
  if (steppers[i].dir == NONE) {
    return true;
  }
  return false;
}
bool MultiStepper::steppersDone() {
  for (uint8_t i = 0; i < NUMSTEPPERS; i++) {
    if (stepperDone(i) == false)
      return false;
    else
      return true;
  }
}
void MultiStepper::setTarget(uint8_t i, int32_t pos, uint32_t startSpeed, uint32_t endSpeed, int32_t stepsWidth) {
  if (i >=  NUMSTEPPERS ) {
    return;
  }
  if (endSpeed < startSpeed) {
    return;
  }
  if (startSpeed > MAXSTARTSPEED)
    startSpeed = MAXSTARTSPEED;
   int32_t deltaPos;   // We calculate the delta betweeen start and finish and make sure we are bugger than stepWidth}
  int32_t deltaSteps; // This is the interval at which we modify the speed for
                      // acceleration
  if (pos > steppers[i].currentStep) {
    steppers[i].dir = CW;
    deltaSteps = stepsWidth / ACCELSTEPS;
    // deltaPos = (pos - steppers[i].currentStep) / 2;
    // if (deltaPos < stepsWidth)
    //   deltaSteps = stepsWidth / ACCELSTEPS;
    // else
    //   deltaSteps = deltaPos / ACCELSTEPS;
  } else {
    steppers[i].dir = CCW;
    deltaSteps = -stepsWidth / ACCELSTEPS;
    // deltaPos = (steppers[i].currentStep - pos) / 2;
    // if (deltaPos < stepsWidth)
    //   deltaSteps = -stepsWidth / ACCELSTEPS;
    // else
    //   deltaSteps = -deltaPos / ACCELSTEPS;
  }
  steppers[i].targetStep = pos;

  if (endSpeed > MAXSPEED) {
    endSpeed = MAXSPEED;
  }
  uint32_t startSpeedMicro = 1000000 / startSpeed;
  uint32_t endSpeedMicro = 1000000 / endSpeed;
  steppers[i].delayMicro = startSpeedMicro;
  uint32_t deltaSpeedMicro = (startSpeedMicro - endSpeedMicro) / ACCELSTEPS;
  // Build the accelration table
  for (uint8_t j = 0; j < ACCELSTEPS; j++) {
    steppers[i].accelTable[j][0] = steppers[i].currentStep + (j + 1) * deltaSteps;
    steppers[i].accelTable[j][1] = startSpeedMicro - (j + 1) * deltaSpeedMicro;
  }
  for (uint8_t j = ACCELSTEPS; j < ACCELSTEPS * 2; j++) {
    steppers[i].accelTable[j][0] = pos - (ACCELSTEPS * 2 - j) * deltaSteps;
    steppers[i].accelTable[j][1] = startSpeedMicro - (ACCELSTEPS * 2 - j - 1) * deltaSpeedMicro;
  }
  steppers[i].accelIndex = 0;
  steppers[i].lastMicro = micros(); // This required  so that we trigger on the fir past to run
}
void MultiStepper::setTarget(uint8_t i, int32_t pos, uint32_t speed) { setTarget(i, pos, MAXSTARTSPEED, speed, MINACCELWIDTH); }
void MultiStepper::stepUp(uint8_t i) {
  incrementStepState(i);
  steppers[i].currentStep++;
  if (steppers[i].currentStep == steppers[i].targetStep) {
    steppers[i].dir = NONE;
  }
}
void MultiStepper::stepDown(uint8_t i) {
  decrementStepState(i);
  steppers[i].currentStep--;
  if (steppers[i].currentStep == steppers[i].targetStep) {
    steppers[i].dir = NONE;
  }
}
void MultiStepper::setTargetAll(int32_t pos, uint32_t startSpeed, uint32_t endSpeed, uint32_t stepsWidth) {

  for (uint8_t i = 0; i < NUMSTEPPERS; i++) {
    setTarget(i, pos, startSpeed, endSpeed, stepsWidth);
  }
}
void MultiStepper::incrementStepState(uint8_t i) {
  steppers[i].currentStepState++;
  if (steppers[i].currentStepState > STEPSTATECOUNT - 1)
    steppers[i].currentStepState = 0;
}
void MultiStepper::decrementStepState(uint8_t i) {

  steppers[i].currentStepState--;
  if (steppers[i].currentStepState > STEPSTATECOUNT - 1)
    steppers[i].currentStepState = STEPSTATECOUNT - 1;
}

void MultiStepper::run() {
  bool needsTransmit = false;
  uint32_t currentMicro = micros();
  for (uint8_t i = 0; i < NUMSTEPPERS; i++) {
    if (currentMicro - steppers[i].lastMicro >= steppers[i].delayMicro) {
      needsTransmit = true;
      steppers[i].lastMicro = currentMicro;
      if (steppers[i].currentStep == steppers[i].accelTable[steppers[i].accelIndex][0]) {
        steppers[i].delayMicro = steppers[i].accelTable[steppers[i].accelIndex][1];
        steppers[i].accelIndex++;
      }
      switch (steppers[i].dir) {
      case CW:
        stepUp(i);
        break;
      case CCW:
        stepDown(i);
        break;
      }
    }
  }
  if (needsTransmit == true) {
    spiBuildArray();
    // Send stepper spi array
    SPI.write(spiArray, SPIBUFFERL);
  }
}
void MultiStepper::spiBuildArray() {
  uint8_t spiIndex = 0;
  for (uint8_t i = 0; i < NUMSTEPPERS; i += 2) {
    spiArray[spiIndex] = stepperState[steppers[i].currentStepState] | (stepperState[steppers[i + 1].currentStepState] << 4);
    spiIndex++;
  }
}
void MultiStepper::printSPIArray() {
  // for (uint8_t i = 0; i < NUMSTEPPERS/2; i++) {
  //   Serial.print(spiArray[i],BIN);
  // }

  Serial.print(spiArray[0], BIN);
  Serial.println("");
}

int32_t MultiStepper::getCurrentStep(uint8_t i) { return steppers[i].currentStep; }
