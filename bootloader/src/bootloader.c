#include <stdint.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/stm32/rcc.h>

#define BOOTLOADER_SIZE        (0x8000U)
#define MAIN_APP_START_ADDRESS (FLASH_BASE + BOOTLOADER_SIZE)

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

static void rcc_setup(void) {
    rcc_clock_setup_pll(&rcc_hsi_configs[CPU_FREQ]);
}

static void systick_setup(void) {
    systick_set_frequency(SYSTICK_FREQ, rcc_hsi_configs[CPU_FREQ].ahb_frequency);
    systick_counter_enable();
    systick_interrupt_enable();
}

static void jump_to_main(void) {
    typedef void (*void_fn)(void);

    uint32_t* reset_vector_entry = (uint32_t*)(MAIN_APP_START_ADDRESS + sizeof(uint32_t));
    uint32_t* reset_vector = (uint32_t*)(*reset_vector_entry);

    void_fn firmware_main = (void_fn)reset_vector;
    firmware_main();
}

int main(void) {
    rcc_setup();
    systick_setup();

    ticks_t start_time = get_ticks();
    while(true) {
        if(get_ticks() - start_time >= SYSTICK_FREQ * 5) {
            jump_to_main();
        }
    }

    return 0;
}
