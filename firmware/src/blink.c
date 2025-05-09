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

// Function to set up the GPIO pin for the LED.
void gpio_setup(void) {
    // Enable the clock for GPIOC peripheral.
    // The LED is connected to a pin on GPIOC.
    rcc_periph_clock_enable(RCC_GPIOC);
    // Configure the LED_PIN on LED_PORT (GPIOC) as a general-purpose output.
    // No pull-up or pull-down resistor is needed.
    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
}

// Interrupt Service Routine (ISR) for Timer 2 (TIM2).
// This function is called when the TIM2 update interrupt occurs.
void tim2_isr(void) {
    // Toggle the state of the LED (turn it on if it's off, and off if it's on).
    gpio_toggle(LED_PORT, LED_PIN);
    // Check if the update interrupt source is enabled for TIM2.
    // This is a good practice to ensure the interrupt flag is cleared only if this specific interrupt occurred.
    if(timer_interrupt_source(TIM2, TIM_DIER_UIE)) {
        // Clear the TIM2 update interrupt flag.
        // This is necessary to allow further interrupts from TIM2.
        // The direct register access method is used here due to a potential issue in older libopencm3 versions (see linked GitHub issue).
        //
        // timer_clear_flag(TIM2, TIM_SR_UIF); /* see https://github.com/libopencm3/libopencm3/issues/1556 */
        TIM_SR(TIM2) &= ~TIM_SR_UIF;
    }
}

// Function to set up Timer 2 (TIM2).
void tim_setup(void) {
    // Enable the TIM2 interrupt in the Nested Vector Interrupt Controller (NVIC).
    // Without this, the tim2_isr function will never be called.
    nvic_enable_irq(NVIC_TIM2_IRQ);

    // Set the priority of the TIM2 interrupt. Lower numbers indicate higher priority.
    nvic_set_priority(NVIC_TIM2_IRQ, 1);

    // Enable the clock for the TIM2 peripheral.
    rcc_periph_clock_enable(RCC_TIM2);

    // Set the initial counter value for TIM2.
    timer_set_counter(TIM2, 1);

    // Set the timer prescaler. The APB1 timer clock (typically PCLK1) is divided by (prescaler + 1).
    // Assuming PCLK1 is 84MHz, 84MHz / (1679 + 1) = 84MHz / 1680 = 50,000 Hz (50 kHz).
    // This means the timer counter will increment 50,000 times per second.
    timer_set_prescaler(TIM2, 1680);

    // Set the auto-reload register (ARR) value, also known as the period.
    // The timer will count from 0 up to this value (or from the initial counter value up to this value).
    // When it reaches this value, it generates an update event (and an interrupt if enabled) and resets to 0.
    // With a 50kHz clock, a period of 50,000 will result in an interrupt every 1 second (50,000 / 50,000 = 1s).
    timer_set_period(TIM2, 50000);

    // Enable the update interrupt for TIM2.
    // This will trigger the tim2_isr when the timer overflows (reaches the period value).
    timer_enable_irq(TIM2, TIM_DIER_UIE);

    // Start the TIM2 counter.
    timer_enable_counter(TIM2);
}
