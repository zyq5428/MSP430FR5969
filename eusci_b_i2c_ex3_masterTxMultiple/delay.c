/*
 * delay.c
 *
 *  Created on: 2019Äê2ÔÂ25ÈÕ
 *      Author: yqzh02
 */


#include "driverlib.h"

#define TIMER_PERIOD 50000

/*
 * us delay function,Limited to 65536us
 */

void delay_us(int time)
{
    //Start timer
    Timer_B_clearTimerInterrupt(TIMER_B0_BASE);

    Timer_B_initUpModeParam param = {0};
    param.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
    param.timerPeriod = time;
    param.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
    param.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_B_CAPTURECOMPARE_INTERRUPT_ENABLE;
    param.timerClear = TIMER_B_DO_CLEAR;
    param.startTimer = false;
    Timer_B_initUpMode(TIMER_B0_BASE, &param);

    Timer_B_startCounter( TIMER_B0_BASE,
        TIMER_B_UP_MODE
        );

    //Enter LPM0, enable interrupts
    __bis_SR_register(LPM0_bits + GIE);
}

/*
 * ms_in delay function,Limited to 4096ms
 */

static void delay_ms_in(int time)
{
    //Start timer
    Timer_B_clearTimerInterrupt(TIMER_B0_BASE);

    Timer_B_initUpModeParam param = {0};
    param.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_64;
    param.timerPeriod = time * 16;
    param.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
    param.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_B_CAPTURECOMPARE_INTERRUPT_ENABLE;
    param.timerClear = TIMER_B_DO_CLEAR;
    param.startTimer = false;
    Timer_B_initUpMode(TIMER_B0_BASE, &param);

    Timer_B_startCounter( TIMER_B0_BASE,
        TIMER_B_UP_MODE
        );

    //Enter LPM0, enable interrupts
    __bis_SR_register(LPM0_bits + GIE);
}

/*
 * ms delay function,Limited to 4096ms
 */
void delay_ms(int time)
{
    if (time < 4096) {
        delay_ms_in(time);
    } else {
        int i;
        int num = time / 4000;
        int residue = time % 4000;
        for (i = num; num > 0; i--) {
            delay_ms_in(4000);
        }
        delay_ms_in(residue);
    }
}

/*
 * s delay function,Limited to 4096ms
 */
void delay_s(int time)
{
    int i;

    for (i = time; i > 0; i--) {
        delay_ms_in(1000);
    }
}


//******************************************************************************
//
//This is the Timer B0 interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMERB0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMERB0_VECTOR)))
#endif
void TIMERB0_ISR (void)
{
    Timer_B_stop(TIMER_B0_BASE);

    // Exit LPM0
    __bic_SR_register_on_exit(CPUOFF);
}

