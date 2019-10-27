/*
The VID28_05 is a 6 state stepper motor. Pin 2 and 3 of the stepper motor
are high and low simultaneously. So they share one IO pin.

Note: to use microsteps we need to assing PWM capable outputs



*/


#include <Arduino.h>
#include "MotorVID28_05.h"

// State  2 1 0   Value
// 0      1 0 1   0x5
// 1      0 0 1   0x1
// 2      0 1 1   0x3
// 3      0 1 0   0x2
// 4      1 1 0   0x6
// 5      1 0 0   0x4
static byte stateMap[] = {0x5, 0x1, 0x3, 0x2, 0x6, 0x4};
#define STATE_COUNT 24
// Divdide each step in 4 microsteps makes 24 steps per cycle
static byte microStepState[] = {251, 238, 218, 191,
                                160, 128, 95, 64,
                                37, 17, 4, 0,
                                4, 17, 37, 64,
                                95, 128, 160, 191,
                                218, 238, 251, 255};

#define STARTINDEX_PIN1 18 // 0 // 23-5
#define STARTINDEX_PIN23 10 // 23-13
#define STARTINDEX_PIN4 2 // 23-21

const int	accelArray[50] = { 12,11,10,10,9,9,8,7,7,6,6,6,5,5,4,4,4,3,3,3,3,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,0,1,1,0,0,1,1,0,0,0,1 }; // define how much ticks to wait until the next move
const int   arrayLength = 50;


#define STEPTIME 600  // 600 microsecs between steps, changed because prescaler is changed. works with two handles and 8MHz
MotorVID28_05::MotorVID28_05(){
  
}
MotorVID28_05::MotorVID28_05( char pin1, unsigned char pin2, unsigned char pin3)
{
  this->currentState = 0;
  this->stateCount = 24;
//  this->distanceToGo = 0;
  this->pins[0] = pin1;
  this->pins[1] = pin2;
  this->pins[2] = pin3;
  this->tick = 0;
  this->maxSpeed = 4;			// by default speed of 4 (every 4th tick a move) 
  this->accel=7;				// by default an acceleration if nothing was declared
  this->currentPosition = 456;	// by default the position is zero
  for (int i=0;i<pinCount;i++) {
    pinMode(pins[i], OUTPUT);
  }
  this->stopped = true;			// by default stopped
  this->lastmicros = 0; // when did the programm run last time (all 600 micro seconds)
  this->destination = 0;
  this->waitDelay = 0; // set the time in ms to wait prior to start the programm

}

void MotorVID28_05::incrementState(){
  currentState++;
  if(currentState>STATE_COUNT-1)
    currentState=0;
}
void MotorVID28_05::decrementState(){
  currentState--;
  if(currentState>254){
    currentState=STATE_COUNT-1;
  }
}
void MotorVID28_05::writeIO()
{
  // analogWrite(pins[0], microStepState[(currentState+STARTINDEX_PIN1) % stateCount]);
  // analogWrite(pins[1], microStepState[(currentState+STARTINDEX_PIN23) % stateCount]);
  // analogWrite(pins[2], microStepState[(currentState+STARTINDEX_PIN4) % stateCount]);
}


void MotorVID28_05::stepUp()
{
  incrementState();
  // currentState = (currentState + 1) % stateCount;
  writeIO();
}

void MotorVID28_05::stepDown()
{ decrementState();
  //currentState = (currentState + (stateCount - 1)) % stateCount;
  writeIO();
}

void MotorVID28_05::run() // checks if time is OK for next step. Not needed
{

	if (pause == true) { // if we can't yet start moving handle due to defined start delay
		if (lastmicros >= micros() - (waitDelay * 1000))  // if we have not yet waited the full waitDelay
		{
			return;
		}
		else 
		{
			pause = false; // no pause anymore
			lastmicros = micros(); //	
		}
	}

	if ((lastmicros <= micros() - STEPTIME) && (stopped==false))  // 100micro and 10000 a_tick = 7s per rotation, 8s für 300 micros
	{	
		lastmicros = micros();
		checkToRun();
	}

}



void MotorVID28_05::checkToRun()
{
	tick++;

	if (destination == currentPosition) { // set back values if destintion is reached
		lastStep = 0;
		moved = 0; 
		tick = 0;
		posReached = true;
		stopped = true; // stopps the motor
		return;
	}
	if (forward == true) {
		if ((moved < (arrayLength * accel)) && (moved < (distance / 2))) { // if we are still in accellerating phase 
			if (tick == accelArray[moved / accel] + maxSpeed + lastStep) { // if a move is required according array, offset with vmax to have maximum acceleration slower than vmax
				MoveMotor(); // change name!
			}
		}
		else if (currentPosition < (destination - (arrayLength * accel))) { // if running at max speed (not yet reached decceleration phase. Achtung, bei acceleration=0 geht er einen tick zu weit!
			if (tick >= lastStep + maxSpeed) {
				MoveMotor(); // change name!
				return;
			}
		}
		else if (currentPosition < destination) {
			if (tick == accelArray[(abs(abs(destination) - abs(currentPosition)) - 1) / accel] + maxSpeed + lastStep) { // if a move is required according array
				MoveMotor(); // change name!
				return;
			}
		}
	}	else if (forward==false) { // if it's reverse mode
		if ((-moved < (arrayLength * accel)) && (-moved < (distance / 2))) { // if we are still in accellerating phase 
			if (tick == accelArray[-moved / accel] + maxSpeed + lastStep) { // if a move is required according array, offset with vmax to have maximum acceleration slower than vmax
				MoveMotor(); // change name!
			}
		}
		else if (currentPosition > (destination + (arrayLength * accel))) { // if running at max speed (not yet reached decceleration phase. Achtung, bei acceleration=0 geht er einen tick zu weit!
			if (tick >= lastStep + maxSpeed) {
				MoveMotor(); // change name!
				return;
			}
		}
		else if (currentPosition > destination) {
			if (tick == accelArray[(abs(abs(destination) - abs(currentPosition)) - 1) / accel] + maxSpeed + lastStep) { // if a move is required according array
				MoveMotor(); // change name!
				return;
			}
		}
	}
}

void MotorVID28_05::MoveMotor() { // change name
	if (forward == true) { // go forward
		moved += 1;
		currentPosition += 1;
		lastStep = tick;
		stepUp();
	}
	else { // go backward
		moved -= 1;
		currentPosition -= 1;
		lastStep = tick;
		stepDown();

	}
}

void MotorVID28_05::setDestination(signed long _dest)
{
	destination = _dest;
	distance = abs(destination - currentPosition); // calculate distance at transfer of new destination
	if (destination > currentPosition) { forward = true; }
	else { forward = false; }
}







void MotorVID28_05::startNewMove()
{
	stopped = false;
	pause = true; // activate waiting time
	lastmicros = micros();
	posReached = false;
}






