/*
 * Controls.cpp
 *
 *  Created on: 23 θώνÿ 2016 γ.
 *      Author: Kreyl
 */

#include "fnt_Verdana27x27.h"
#include "fnt_Tahoma10x11.h"
#include "ControlClasses.h"
#include "kl_lib.h"

// ==== Theme ====
// Button
#define BTN_FNT             &fntVerdana27x27
#define BTN_CLR_TXT         clWhite
#define BTN_CLR_REL_TOP     (Color_t){0, 99, 00}
#define BTN_CLR_REL_BOT     (Color_t){0, 27, 00}
#define BTN_CLR_PRS_TOP     (Color_t){0, 27, 00}
#define BTN_CLR_PRS_BOT     (Color_t){0, 99, 00}

#define BTN_MODE_CLR_REL_TOP    (Color_t){0, 99, 99}
#define BTN_MODE_CLR_REL_BOT    (Color_t){0, 27, 27}
#define BTN_MODE_CLR_PRS_TOP    (Color_t){0, 27, 27}
#define BTN_MODE_CLR_PRS_BOT    (Color_t){0, 99, 99}

#if 1 // ========================== Global =====================================
#if 1 // ==== Temperature & time txtboxes ====
#define TXT_T_H         36
#define TXT_T_W         75
#define TXT_T_Y0        0
#define TXT_TPCB_X0     0
#define TXT_THTR_X0     (TXT_TPCB_X0 + TXT_T_W + 4)
#define TXT_TIME_X0     (TXT_THTR_X0 + TXT_T_W + 4)

static char STPcb[7] = "---";
const Textbox_t txtTPcb {
    TXT_TPCB_X0, TXT_T_Y0, TXT_T_W, TXT_T_H,
    STPcb, &fntVerdana27x27, clRed,     // Text
    clBlack                             // Back
};

#endif

// Lines
const LineHoriz_t LineTop   { 0, (TXT_T_H + 2), LCD_W, 1, clWhite };
#endif

#if 1 // ======================== Page Profile =================================
const Control_t* PageProfileCtrls[] = {
        (Control_t*)&LineTop,
};

const Page_t PageProfile = { PageProfileCtrls, countof(PageProfileCtrls) };
#endif // Page 0


const Page_t* Page[] = {
        &PageProfile,
};
