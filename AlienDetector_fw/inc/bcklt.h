/*
 * bcklt.h
 *
 *  Created on: 25.01.2012
 *      Author: kreyl
 */

#ifndef BCKLT_H_
#define BCKLT_H_

#include "stm32f10x.h"


class BckLight_t {
public:
    void Init(void);
    void On(uint8_t ABrightness);
    void Off(void);
};


extern BckLight_t Bcklt;

#endif /* BCKLT_H_ */
