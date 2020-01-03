#include <Arduino.h>

struct Co2Measure
{
    bool IsPrepare;
    uint16_t Co2;
    uint16_t Tvoc;
};

struct DhtMeasure
{
    bool IsPrepare;
    float Temperature;
    float Humidity;
};