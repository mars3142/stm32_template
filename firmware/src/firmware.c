#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include "firmware.h"

#define BOOTLOADER_SIZE (0x8000U)

static void vector_setup(void) {
    SCB_VTOR = BOOTLOADER_SIZE;
}

static void gpio_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
}

static void nvic_setup(void) {
    /* Without this the timer interrupt routine will never be called. */
    nvic_enable_irq(NVIC_TIM2_IRQ);
    nvic_set_priority(NVIC_TIM2_IRQ, 1);
}

void tim2_isr(void) {
    gpio_toggle(LED_PORT, LED_PIN); /* LED on/off. */
    TIM_SR(TIM2) &= ~TIM_SR_UIF; /* Clear interrrupt flag. */
}

static void timer_setup(void) {
    rcc_periph_clock_enable(RCC_TIM2);

    /* Set timer start value. */
    TIM_CNT(TIM2) = 1;

    /* Set timer prescaler. 84MHz/1680 => 50000 counts per second. */
    TIM_PSC(TIM2) = 1680;

    /* End timer value. If this is reached an interrupt is generated. */
    TIM_ARR(TIM2) = 50000;

    /* Update interrupt enable. */
    TIM_DIER(TIM2) |= TIM_DIER_UIE;

    /* Start timer. */
    TIM_CR1(TIM2) |= TIM_CR1_CEN;
}

int main(void) {
    vector_setup();
    gpio_setup();
    nvic_setup();
    timer_setup();

    while(true)
        ;

    return 0;
}
