//
// Copyright (c) Peter Siegmund and contributors. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "blink.h"

#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include "global.h"

void gpio_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
}

void tim2_isr(void) {
    gpio_toggle(LED_PORT, LED_PIN); /* LED on/off. */
    if(timer_interrupt_source(TIM2, TIM_DIER_UIE)) {
        // timer_clear_flag(TIM2, TIM_SR_UIF); /* Clear interrrupt flag. - see https://github.com/libopencm3/libopencm3/issues/1556 */
        TIM_SR(TIM2) &= ~TIM_SR_UIF; /* Clear interrrupt flag. */
    }
}

void tim_setup(void) {
    /* Without this the timer interrupt routine will never be called. */
    nvic_enable_irq(NVIC_TIM2_IRQ);
    nvic_set_priority(NVIC_TIM2_IRQ, 1);

    rcc_periph_clock_enable(RCC_TIM2);

    /* Set timer start value. */
    timer_set_counter(TIM2, 1);

    /* Set timer prescaler and periode. 84MHz/1680 => 50000 counts per second. */
    timer_set_prescaler(TIM2, 1680);
    timer_set_period(TIM2, 50000);

    /* Update interrupt enable. */
    timer_enable_irq(TIM2, TIM_DIER_UIE);

    /* Start timer. */
    timer_enable_counter(TIM2);
}