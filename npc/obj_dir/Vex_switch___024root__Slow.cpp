// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vex_switch.h for the primary calling header

#include "Vex_switch__pch.h"

void Vex_switch___024root___ctor_var_reset(Vex_switch___024root* vlSelf);

Vex_switch___024root::Vex_switch___024root(Vex_switch__Syms* symsp, const char* namep)
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vex_switch___024root___ctor_var_reset(this);
}

void Vex_switch___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vex_switch___024root::~Vex_switch___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
