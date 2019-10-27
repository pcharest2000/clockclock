#ifndef Clock_h
#define Clock_h
#include "MultiStepper.h"

class Clock : public MultiStepper {
public:
  Clock();
  void setMinuteTarget(uint32_t i, uint32_t j, int32_t pos, uint32_t startSpeed,
                       uint32_t endSpeed, int32_t stepsWidth);
  void setHourTarget(uint32_t i, uint32_t j, int32_t pos, uint32_t startSpeed,
                      uint32_t endSpeed, int32_t stepsWidth);

  bool getMinuteDone(uint32_t i, uint32_t j);
  bool getHourDone(uint32_t i, uint32_t j);
  uint32_t getMinuteIndex(uint32_t i, uint32_t j);
  uint32_t getHourIndex(uint32_t i, uint32_t j);
  int32_t getMinutePos(uint32_t i, uint32_t j);
  int32_t getHourPos(uint32_t i, uint32_t j);

private:
};

#endif // Clock_h
