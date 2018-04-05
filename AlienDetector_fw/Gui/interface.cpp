/*
 * Controls.cpp
 *
 *  Created on: 23 θώνÿ 2016 γ.
 *      Author: Kreyl
 */

//#include "fnt_Verdana27x27.h"
#include "fnt_Tahoma10x11.h"
#include "ControlClasses.h"
#include "kl_lib.h"
#include "interface.h"
#include "gui_engine.h"

#if 1 // ========================== Chart ======================================
Chart_t Chart(
        0, 39, LCD_W, 200,            // Top, Left, Width, Height
        0, LCD_W, 0, 100,        // Xmin, Xmax, Ymin, Ymax
        &fntTahoma10x11, clWhite,   // Text
        clBlack                     // Back color
        );

Series_t Series[COL_CNT];
#endif


#if 1 // ======================== Page Profile =================================
const Control_t* PageProfileCtrls[] = {
//        (Control_t*)&LineTop,
};

const Page_t PageProfile = { PageProfileCtrls, countof(PageProfileCtrls) };
#endif // Page 0


const Page_t* Page[] = {
        &PageProfile,
};

void InitInterface() {
    Gui.Init();
    Chart.Clear();
    // Init series
    for(int i=0; i<COL_CNT; i++) {
        Series[i].Parent = &Chart;
        Series[i].Color = COL_CLR;
        Series[i].ColWidth = (LCD_W / COL_CNT);
    }
}
