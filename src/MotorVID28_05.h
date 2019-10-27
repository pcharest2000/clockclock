#ifndef MotorVID28_05_h
#define MotorVID28_05_h
#include <Arduino.h>

class MotorVID28_05
{
 public:
// OBJECTS
  MotorVID28_05();
   MotorVID28_05(char pin1, unsigned char pin2, unsigned char pin3);

   // for debugg, destination, currentPosition, maxSpeed, accel, distance and moved were changed to long. No improvement


// VARIABLES
   static const unsigned char pinCount = 3;
   unsigned char stateCount;		// 24 microstates
   unsigned char pins[pinCount];
   unsigned char currentState;		// current state in the cycle
   int32_t destination;			// target we are moving to
   int32_t currentPosition;		// our current positioin
   int32_t maxSpeed;				// MAKE A STEP EVERY X TICK. 
   int32_t accel;				// 1= 50 move to accelerate (4.2°)  
   bool stopped;					// true if stopped
   bool forward;					// true if forward, false if backward
   bool posReached;					// true if position is reached
   int32_t waitDelay;			// how long (in milliseconds) shall we wait until the movement can start (to let some handles run later than others)
   bool pause;						// true = don't start yet. After waitDelay has passed, it turns to false
   unsigned int distance; // calculates the distance 

// PROCEDURES
   void stepUp();
   void stepDown();
   void decrementState();
   void incrementState();

  
void run();
   void setDestination(signed long _dest);
   void startNewMove(); //With this command, a new move starts.


 private:
// VARIABLES
	 signed int moved; // how much moves did the stepper do since it started 
	 unsigned long lastStep; // At which tick did the motor the last step?
	 unsigned long lastmicros; // when did the programm run last time (all 600 micro seconds with 8MHz)
	 unsigned long tick; // every X microsecond a tick is initiated and the library checks if the motor has to move

 // PROCEDURES
	 void writeIO();
	 void MoveMotor(); // change name. This moves the stepper, count the position, move etc
	 void checkToRun(); // check if motor has to run

};


#endif
