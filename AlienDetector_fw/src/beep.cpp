/*
 * beep.cpp
 *
 *  Created on: 25.01.2012
 *      Author: kreyl
 */

#include "beep.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"

Beep_t Beep;

void Beep_t::Init() {
    NewSnd = 0;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    IPwm.Init(TIM4, 200, 1600, 1, TIM_OCPolarity_High);
    IPwm.Disable();
    // Setup GPIO
    klGpio::SetupByN(GPIOB, 6, GPIO_Mode_AF_PP);
}

void Beep_t::Task() {
    if (ISnd == 0) {
        if (NewSnd == 0) return;
        else {  // for initial conditions
            ISnd = NewSnd;
            Delay.Reset(&ITimer);
        }
    }
    // Switch to next sound if current chunk is last
    if ((ISnd != NewSnd) && (ICounter == (ISnd->ChunkCount-1))) {
        ICounter = 0;
        ISnd = NewSnd;
        if (ISnd->Chunks[ICounter].Volume != 0) On();
        else Off();
        Delay.Reset(&ITimer);
    }

    if (Delay.Elapsed(&ITimer, ISnd->Chunks[ICounter].Length)) {
        // Switch to next chunk
        ICounter++;
        if(ICounter == ISnd->ChunkCount) ICounter = 0;  // Current beep finished
        // Set current snd params
        if (ISnd->Chunks[ICounter].Volume != 0) On();
        else Off();
        Delay.Reset(&ITimer);
    }
}

void Beep_t::On() {
    IPwm.Set(ISnd->Chunks[ICounter].Volume);
    IPwm.SetFreq(ISnd->Chunks[ICounter].Freq);
#ifndef BEEP_DISABLE
    IPwm.Enable();
#endif
}

