#include "hal.h"
#include "uart.h"
#include "kl_lib.h"
#include "MsgQ.h"
#include "Sequences.h"
#include "shell.h"
#include "SimpleSensors.h"
#include "buttons.h"
#include "interface.h"
#include "radio_lvl1.h"
#include "kl_i2c.h"
#include "ee.h"

// Forever
EvtMsgQ_t<EvtMsg_t, MAIN_EVT_Q_LEN> EvtQMain;
extern CmdUart_t Uart;
void OnCmd(Shell_t *PShell);
void ITask();

int16_t ID;
const EE_t ee { &i2c3 };
static uint8_t ISetID(int16_t NewID);
void ReadIDfromEE();

void DisplayRx();

int main(void) {
    // Start Watchdog. Will reset in main thread by periodic 1 sec events.
//    Iwdg::InitAndStart(4500);
    // Setup clock frequency
    Clk.SetCoreClk(cclk48MHzMSI);
    Clk.UpdateFreqValues();
    // Init OS
    halInit();
    chSysInit();

    // ==== Init hardware ====
    EvtQMain.Init();
    Uart.Init(115200);
    Printf("\r%S %S\r\n", APP_NAME, BUILD_TIME);
    Clk.PrintFreqs();

    InitInterface();
//    SimpleSensors::Init();

    i2c3.Init();
//    i2c3.ScanBus();
    ee.Init();
    ReadIDfromEE();

    Radio.RxTable.Clear();
    Radio.Init();

//    TmrOneSecond.StartOrRestart();
    // Main cycle
    ITask();
}

__noreturn
void ITask() {
    while(true) {
        EvtMsg_t Msg = EvtQMain.Fetch(TIME_INFINITE);
        switch(Msg.ID) {
            case evtIdShellCmd:
                OnCmd((Shell_t*)Msg.Ptr);
                ((Shell_t*)Msg.Ptr)->SignalCmdProcessed();
                break;

            case evtIdButtons:
//                Printf("BtnType %u; N=%u; Btns %u %u\r", Msg.BtnEvtInfo.Type, Msg.BtnEvtInfo.BtnCnt, Msg.BtnEvtInfo.BtnID[0], Msg.BtnEvtInfo.BtnID[1]);
//                switch(Msg.BtnEvtInfo.Type) {
//                    case beRelease:
//                        if(Msg.BtnEvtInfo.BtnID[0] == 0) StateMachine(eventShortPress);
//                        else StateMachine(eventShortPress2);
//                        break;
//                    case beLongPress:
//                        if(Msg.BtnEvtInfo.BtnID[0] == 0) StateMachine(eventLongPress);
//                        else StateMachine(eventLongPress2);
//                        break;
//                    case beLongCombo:
//                        StateMachine(eventLongBoth);
//                        break;
//                    default: break;
//                } // switch
                break;

            case evtIdCheckRxTable:
                DisplayRx();
                break;

            // Measure battery and reset watchdog
            case evtIdEverySecond: {
                // Reload IWDG
//                Iwdg::Reload();
                // Check battery
            } break;

            default: break;
        } // switch
    } // while true
}


Firefly_t SortedFirefly[RXTABLE_SZ];

int CompareByRssi(const void* a, const void* b) {
    int ra = ((Firefly_t*)a)->Rssi;
    int rb = ((Firefly_t*)b)->Rssi;
    if(ra <  rb) return 1;
    if(ra == rb) return 0;
    return -1;
}
int CompareByID(const void* a, const void* b) {
    int ra = ((Firefly_t*)a)->ID;
    int rb = ((Firefly_t*)b)->ID;
    if(ra <  rb) return -1;
    if(ra == rb) return 0;
    return 1;
}

void DisplayRx() {
    // Copy table
    for(int i=0; i<RXTABLE_SZ; i++) {
        SortedFirefly[i] = Radio.RxTable.Firefly[i];
//        Printf("Src %u; %d\r", i, SortedFirefly[i].Rssi);
    }
    // Sort table by Rssi
    qsort(SortedFirefly, RXTABLE_SZ, sizeof(Firefly_t), CompareByRssi);
    // Sort first COL_CNT by ID
    qsort(SortedFirefly, COL_CNT, sizeof(Firefly_t), CompareByID);

    // Show result
    for(int i=0; i<COL_CNT; i++) {
//        Printf("Dst %u; %d\r", SortedFirefly[i].ID, SortedFirefly[i].Rssi);
        Series[i].ColSetValue(i, SortedFirefly[i].Rssi + 116);
    }

    Radio.RxTable.Clear();
}


#if 1 // ======================= Command processing ============================
void OnCmd(Shell_t *PShell) {
	Cmd_t *PCmd = &PShell->Cmd;
//    __unused int32_t dw32 = 0;  // May be unused in some configurations
//    Printf("%S  ", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) PShell->Ack(retvOk);
    else if(PCmd->NameIs("Version")) PShell->Printf("%S %S\r", APP_NAME, BUILD_TIME);

    if(PCmd->NameIs("Col")) {
        uint32_t col=0;
        uint32_t val = 0;
        if(PCmd->GetNext<uint32_t>(&col) == retvOk) {
            if(PCmd->GetNext<uint32_t>(&val) == retvOk) {
                Series[col].ColSetValue(col, val);
            }
        }
    }

    else if(PCmd->NameIs("SetID")) {
        int16_t NewID = 0;
        if(PCmd->GetNext<int16_t>(&NewID) != retvOk) { PShell->Ack(retvCmdError); return; }
        uint8_t r = ISetID(NewID);
        PShell->Ack(r);
    }

    //else if(PCmd->NameIs("GetBat")) { PShell->Printf("Battery: %u\r", Audio.GetBatteryVmv()); }

    else PShell->Ack(retvCmdUnknown);
}
#endif

#if 1 // ============================= Settings ================================
// EEAddresses
#define EE_ADDR_DEVICE_ID       0

void ReadIDfromEE() {
    ee.Read<int16_t>(EE_ADDR_DEVICE_ID, &ID);  // Read device ID
    Printf("ID: %d\r", ID);
}

uint8_t ISetID(int16_t NewID) {
    uint8_t rslt = ee.Write<int16_t>(EE_ADDR_DEVICE_ID, &NewID);
    if(rslt == retvOk) {
        ID = NewID;
        Printf("New ID: %d\r", ID);
        return retvOk;
    }
    else {
        Printf("EE error: %u\r", rslt);
        return retvFail;
    }
}
#endif

