/*
 * sound.h
 *
 *  Created on: 25.01.2012
 *      Author: kreyl
 */

#ifndef SOUND_H_
#define SOUND_H_

#include "delay_util.h"
#include "kl_lib.h"

//#define BEEP_DISABLE    // Uncomment this in case of emergency

#define BEEP_MAX_CHUNKS 6

typedef struct {
    uint16_t Freq;
    uint8_t Volume;
    uint16_t Length;
} BeepChunk_t;

struct BeepSnd_t {
    uint8_t ChunkCount;
    BeepChunk_t Chunks[BEEP_MAX_CHUNKS];
};

class Beep_t {
private:
    klPwmChannel_t IPwm;
    uint32_t ITimer, ICounter;
    bool IsSwitchDelay;
    void On(void);
    void Off(void) { IPwm.Disable(); }
    BeepSnd_t *ISnd, *NewSnd;
public:
    void SetSound(BeepSnd_t *ASnd) { if (ISnd != ASnd) NewSnd = ASnd; }  // Check if already set
    void Init(void);
    void Task(void);
};

extern Beep_t Beep;


#endif /* SOUND_H_ */
