// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vex_switch.h for the primary calling header

#ifndef VERILATED_VEX_SWITCH___024ROOT_H_
#define VERILATED_VEX_SWITCH___024ROOT_H_  // guard

#include "verilated.h"


class Vex_switch__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vex_switch___024root final {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(a,0,0);
    VL_IN8(b,0,0);
    VL_OUT8(f,0,0);
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VstlPhaseResult;
    CData/*0:0*/ __Vtrigprevexpr___TOP__a__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__b__0;
    CData/*0:0*/ __VicoDidInit;
    CData/*0:0*/ __VicoPhaseResult;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 2> __VicoTriggered;

    // INTERNAL VARIABLES
    Vex_switch__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vex_switch___024root(Vex_switch__Syms* symsp, const char* namep);
    ~Vex_switch___024root();
    VL_UNCOPYABLE(Vex_switch___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
