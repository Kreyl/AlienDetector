/*
 * Controls.h
 *
 *  Created on: 15 ??? 2016 ?.
 *      Author: Kreyl
 */

#pragma once

#include "ControlClasses.h"

// Here, in .h file, defined only controls required outside

void InitInterface();

// Chart
#define COL_CNT     11
#define COL_CLR     clLightBlue
extern Chart_t Chart;
extern Series_t Series[COL_CNT];
