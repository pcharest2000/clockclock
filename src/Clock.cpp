#include "Clock.h"

Clock::Clock() {}

uint32_t Clock::getHourIndex(uint32_t i, uint32_t j) { return j * CLOCKWIDTH * 2 + i * 2 + 1; }
uint32_t Clock::getMinuteIndex(uint32_t i, uint32_t j) { return j * CLOCKWIDTH * 2 + i * 2; }
void Clock::setMinuteTarget(uint32_t i, uint32_t j, int32_t pos, uint32_t startSpeed, uint32_t endSpeed, int32_t stepsWidth) {
  if (i >= CLOCKWIDTH || j >= CLOCKHEIGTH) {
    return;
  }
  setTarget(getMinuteIndex(i, j), pos, startSpeed, endSpeed, stepsWidth);
}

void Clock::setHourTarget(uint32_t i, uint32_t j, int32_t pos, uint32_t startSpeed, uint32_t endSpeed, int32_t stepsWidth) {
  if (i >= CLOCKWIDTH || j >= CLOCKHEIGTH) {
    return;
  }
  setTarget(getHourIndex(i, j), pos, startSpeed, endSpeed, stepsWidth);
}

bool Clock::getMinuteDone(uint32_t i, uint32_t j) {
  if (i >= CLOCKWIDTH || j >= CLOCKHEIGTH) {
    return false;
  }
  return stepperDone(getMinuteIndex(i, j));
}

bool Clock::getHourDone(uint32_t i, uint32_t j) {
  if (i >= CLOCKWIDTH || j >= CLOCKHEIGTH) {
    return false;
  }
  return stepperDone(getHourIndex(i, j));
}
int32_t Clock::getHourPos(uint32_t i, uint32_t j) {
  if (i >= CLOCKWIDTH || j >= CLOCKHEIGTH) {
    return 0;
  }
  return getCurrentStep(getHourIndex(i, j));
}
int32_t Clock::getMinutePos(uint32_t i, uint32_t j) {

  if (i >= CLOCKWIDTH || j >= CLOCKHEIGTH) {
    return 0;
  }
  return getCurrentStep(getMinuteIndex(i, j));
}
