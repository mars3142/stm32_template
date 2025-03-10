#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#define BOOTLOADER_SIZE (0x8000U)

#define LED_PORT (GPIOC)
#define LED_PIN  (GPIO13)

#define CPU_FREQ     (RCC_CLOCK_3V3_84MHZ)
#define SYSTICK_FREQ (1000)

typedef uint64_t ticks_t;

volatile ticks_t ticks = 0;
void sys_tick_handler(void) {
    ticks++;
}

static ticks_t get_ticks(void) {
    return ticks;
}

static void vector_setup(void) {
    SCB_VTOR = BOOTLOADER_SIZE;
}

static void rcc_setup(void) {
    rcc_clock_setup_pll(&rcc_hsi_configs[CPU_FREQ]);
}

static void gpio_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
}

static void systick_setup(void) {
    uint32_t ahb = rcc_hsi_configs[CPU_FREQ].ahb_frequency;
    systick_set_frequency(SYSTICK_FREQ, ahb);
    systick_counter_enable();
    systick_interrupt_enable();
}

int main(void) {
    vector_setup();
    rcc_setup();
    gpio_setup();
    systick_setup();

    ticks_t start_time = get_ticks();

    while(true) {
        if(get_ticks() - start_time >= SYSTICK_FREQ) {
            gpio_toggle(LED_PORT, LED_PIN);
            start_time = get_ticks();
        }
    }

    return 0;
}
