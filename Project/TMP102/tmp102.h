#ifndef TMP102_H
#define TMP102_H

#ifdef __cplusplus
extern "C"
{
#endif

void setupTMP102();
uint32_t getTemperature();
#ifdef __cplusplus
}
#endif

#endif