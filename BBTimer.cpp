//
// Created by B Bryant on 9/15/2021
//
// Access the five nRF52840 32-bit hardware timers.
// User must ensure selected hardware timers do not conflict with application
// utilized MBED OS features or other library timer usage.
//
// For each timer used:
//  - Call setupTimer() to set initial period and callback function.
//  - Call startTimer() and stopTimer() as needed.
//  - Use updatePeriod() to change period of a running timer.
//    updatePeriod() can be called from within the user callback.
//
// Note that the callback provided to setupTimer() is called by the timer
// interrupt service routine (ISR), so do not call resources that depend on
// interrupts. Also, be sure callback execution time will not exceed the timer
// period, or the system will lock up.
//
// Although array bounds are managed using BBTimerIdType indexes,
// there is no explicit parameter, bounds, or nullptr error checking.

#pragma once
#include "BBTimer.h"

// Declare and initialize Static BBTimer class members
BBTimer* BBTimer::timer_object[NUM_TIMERS] = {
    nullptr, nullptr, nullptr, nullptr, nullptr
};

// const pointers to non const register structures
NRF_TIMER_Type* const BBTimer::NRF_TIMER_LIST[NUM_TIMERS] = {
    NRF_TIMER0, NRF_TIMER1, NRF_TIMER2, NRF_TIMER3, NRF_TIMER4
};

const IRQn_Type BBTimer::IRQ_LIST[NUM_TIMERS] = {
    TIMER0_IRQn, TIMER1_IRQn, TIMER2_IRQn, TIMER3_IRQn, TIMER4_IRQn
};

const void* BBTimer::ISR_LIST[NUM_TIMERS] = {
    (void*)BBTimer::timer0Isr, (void*)BBTimer::timer1Isr,
    (void*)BBTimer::timer2Isr, (void*)BBTimer::timer3Isr,
    (void*)BBTimer::timer4Isr
};
