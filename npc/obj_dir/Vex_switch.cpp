// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vex_switch__pch.h"
#include "verilated_fst_c.h"

//============================================================
// Constructors

Vex_switch::Vex_switch(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vex_switch__Syms(contextp(), _vcname__, this)}
    , a{vlSymsp->TOP.a}
    , b{vlSymsp->TOP.b}
    , f{vlSymsp->TOP.f}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
    contextp()->traceBaseModelCbAdd(
        [this](VerilatedTraceBaseC* tfp, int levels, int options) { traceBaseModel(tfp, levels, options); });
}

Vex_switch::Vex_switch(const char* _vcname__)
    : Vex_switch(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vex_switch::~Vex_switch() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vex_switch___024root___eval_debug_assertions(Vex_switch___024root* vlSelf);
#endif  // VL_DEBUG
void Vex_switch___024root___eval_static(Vex_switch___024root* vlSelf);
void Vex_switch___024root___eval_initial(Vex_switch___024root* vlSelf);
void Vex_switch___024root___eval_settle(Vex_switch___024root* vlSelf);
void Vex_switch___024root___eval(Vex_switch___024root* vlSelf);

void Vex_switch::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vex_switch::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vex_switch___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_activity = true;
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vex_switch___024root___eval_static(&(vlSymsp->TOP));
        Vex_switch___024root___eval_initial(&(vlSymsp->TOP));
        Vex_switch___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vex_switch___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vex_switch::eventsPending() { return false; }

uint64_t Vex_switch::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vex_switch::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vex_switch___024root___eval_final(Vex_switch___024root* vlSelf);

VL_ATTR_COLD void Vex_switch::final() {
    contextp()->executingFinal(true);
    Vex_switch___024root___eval_final(&(vlSymsp->TOP));
    contextp()->executingFinal(false);
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vex_switch::hierName() const { return vlSymsp->name(); }
const char* Vex_switch::modelName() const { return "Vex_switch"; }
unsigned Vex_switch::threads() const { return 1; }
void Vex_switch::prepareClone() const { contextp()->prepareClone(); }
void Vex_switch::atClone() const {
    contextp()->threadPoolpOnClone();
}
std::unique_ptr<VerilatedTraceConfig> Vex_switch::traceConfig() const {
    return std::unique_ptr<VerilatedTraceConfig>{new VerilatedTraceConfig{false}};
};

//============================================================
// Trace configuration

void Vex_switch___024root__trace_decl_types(VerilatedFst* tracep);

void Vex_switch___024root__trace_init_top(Vex_switch___024root* vlSelf, VerilatedFst* tracep);

VL_ATTR_COLD static void trace_init(void* voidSelf, VerilatedFst* tracep, uint32_t code) {
    // Callback from tracep->open()
    Vex_switch___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vex_switch___024root*>(voidSelf);
    Vex_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (!vlSymsp->_vm_contextp__->calcUnusedSigs()) {
        VL_FATAL_MT(__FILE__, __LINE__, __FILE__,
            "Turning on wave traces requires Verilated::traceEverOn(true) call before time 0.");
    }
    vlSymsp->__Vm_baseCode = code;
    tracep->pushPrefix(vlSymsp->name(), VerilatedTracePrefixType::SCOPE_MODULE);
    Vex_switch___024root__trace_decl_types(tracep);
    Vex_switch___024root__trace_init_top(vlSelf, tracep);
    tracep->popPrefix();
}

VL_ATTR_COLD void Vex_switch___024root__trace_register(Vex_switch___024root* vlSelf, VerilatedFst* tracep);

VL_ATTR_COLD void Vex_switch::traceBaseModel(VerilatedTraceBaseC* tfp, int levels, int options) {
    (void)levels; (void)options;
    VerilatedFstC* const stfp = dynamic_cast<VerilatedFstC*>(tfp);
    if (VL_UNLIKELY(!stfp)) {
        vl_fatal(__FILE__, __LINE__, __FILE__,"'Vex_switch::trace()' called on non-VerilatedFstC object;"
            " use --trace-fst with VerilatedFst object, and --trace-vcd with VerilatedVcd object");
    }
    stfp->spTrace()->addModel(this);
    stfp->spTrace()->addInitCb(&trace_init, &(vlSymsp->TOP), name(), false, 3);
    Vex_switch___024root__trace_register(&(vlSymsp->TOP), stfp->spTrace());
}
