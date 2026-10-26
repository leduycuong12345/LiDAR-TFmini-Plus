#ifndef VELOCITY_H
#define VECLOCITY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
    #endif
    void Velocity_Init(void);
    void Velocity_Update(unit16_t distance_cm),
                        uint32_t timestamp_ms);
    float Velocity_Get_mps(void);

    #ifdef __cplusplus
}
#endif