/*
 *  MultiStepper Arduino Library
 *  Guy Carpenter, Clearwater Software - 2012
 *
 *  Licensed under the BSD2 license, see license.txt for details.
 *
 *  All text above must be included in any redistribution.
 */

#ifndef MultiStepper_h
#define MultiStepper_h
#include <Arduino.h>
#include <SPI.h>
#define CLOCKWIDTH 8                            // Width of clock in clocks
#define CLOCKHEIGTH 3                           // Heigth of clock in cloks
#define NUMSTEPPERS 2 * CLOCKWIDTH *CLOCKHEIGTH // Total number of steppers
#define MAXSPEED 400                            // Speed in HZ
#define STEPSTATECOUNT 6                        // Number states for steppers io
#define MINACCELWIDTH 100                      // Used in the acceleration table call this is the minium distance to accellerate from 0 to max speed
#define MAXSTARTSPEED 100                       // HZ acoording to the documentation  this is the maximum start speed
#define ACCELSTEPS 5                            // Number of increments for acceleration calculate
#define SPIBUFFERL NUMSTEPPERS / 2
class MultiStepper {
public:
  typedef enum {
    CW,
    CCW,
    NONE // none means we are done
  } MotorDirection;

  MultiStepper();

  void zeroAll(); // Sets all steppers to 0 value used for homming
  void run();     // runs the steppers must be called often
  // set target calculate the trapezoide shape as function of the start speed
  // end speed and stepWidth of the acceleration speeds are ine herts and width
  // in steps
  void setTarget(uint8_t i, int32_t pos, uint32_t startSpeed, uint32_t endSpeed,
                 int32_t stepsWidth); // position is in steps
  void setTarget(uint8_t i, int32_t pos, uint32_t speed);
  void setTargetAll(int32_t pos, uint32_t startSpeed, uint32_t endSpeed, uint32_t stepsWidth);
  void setSpeed(uint8_t i, uint32_t speed); // speed is in hz
  void printSPIArray();
  bool stepperDone(uint8_t i);
  bool steppersDone();
  int32_t getCurrentStep(uint8_t i);

private:
  struct stepperStruct {
    uint32_t currentStepState;
    uint32_t accelIndex;                   // where we are in the accelration table
    int32_t currentStep;                   // step we are currently at
    int32_t targetStep;                    // target we are moving to
    uint32_t lastMicro;                    // microsecs since last step
    uint32_t delayMicro;                   // micros between steps
    MotorDirection dir;                    // direction -1,0,1
    int32_t accelTable[ACCELSTEPS * 2][2]; // accelration table sets delay for acceleration
  };
  uint8_t spiArray[SPIBUFFERL]; // Holds the bytes to the shift registers
  void spiBuildArray();
  void stepUp(uint8_t i);
  void stepDown(uint8_t i);
  void incrementStepState(uint8_t i);
  void decrementStepState(uint8_t i);
  stepperStruct steppers[NUMSTEPPERS];
};

#endif
