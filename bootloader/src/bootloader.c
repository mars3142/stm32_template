//
// Copyright (c) Peter Siegmund and contributors. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/scb.h>

#define BOOTLOADER_SIZE (0x8000U) // Size of the bootloader in bytes (32KB)
#define MAIN_APP_START_ADDRESS \
    (FLASH_BASE + BOOTLOADER_SIZE) // Start address of the main application

#define CPU_FREQ     (RCC_CLOCK_3V3_84MHZ) // Define the CPU frequency
#define SYSTICK_FREQ (1000) // Define the Systick frequency in Hz (1ms tick)

typedef uint32_t ticks_t;

// Global variable to store the number of Systick ticks
volatile ticks_t ticks = 0;

// Systick interrupt handler: increments the global 'ticks' variable.
void sys_tick_handler(void) {
    ticks++;
}

// Calculates the number of Systick ticks for a given delay in seconds.
// @param sec: Delay in seconds.
// @return: Number of Systick ticks corresponding to the delay.
static ticks_t get_delay_ticks(ticks_t sec) {
    return sec * SYSTICK_FREQ;
}

// Configures the system clock (RCC).
// Sets up the PLL to achieve the defined CPU_FREQ using HSI as the source.
static void rcc_setup(void) {
    rcc_clock_setup_pll(&rcc_hsi_configs[CPU_FREQ]);
}

// Configures the Systick timer.
// Sets the Systick timer to interrupt at the SYSTICK_FREQ.
static void systick_setup(void) {
    // Set the Systick reload value to achieve the desired frequency.
    systick_set_frequency(SYSTICK_FREQ, rcc_hsi_configs[CPU_FREQ].ahb_frequency);
    // Enable the Systick counter.
    systick_counter_enable();
    // Enable the Systick interrupt.
    systick_interrupt_enable();
}

static void jump_to_main(void) {
    // Define a function pointer type for the application's reset handler.
    typedef void (*application_entry_t)(void);

    // The first word at the application's vector table is the initial Main Stack Pointer (MSP) value.
    uint32_t app_initial_msp = *((volatile uint32_t*)MAIN_APP_START_ADDRESS);

    // The second word is the application's reset handler address (the entry point).
    uint32_t app_reset_handler_addr =
        *((volatile uint32_t*)(MAIN_APP_START_ADDRESS + sizeof(uint32_t)));

    // Basic validity check for the application:
    // Check if the initial MSP and reset handler address are not 0xFFFFFFFF (common for erased flash).
    // A more robust check would involve verifying MSP is within RAM bounds and reset_handler_addr
    // is within app flash bounds and odd.
    if(app_initial_msp == 0xFFFFFFFF || app_reset_handler_addr == 0xFFFFFFFF) {
        // No valid application found (e.g., flash erased or corrupted).
        // Stay in bootloader. Consider adding error indication (e.g., blinking an LED).
        // This loop prevents further attempts to jump to a non-existent/corrupt application.
        while(1) {
            // Loop indefinitely, or implement a more sophisticated error handling/recovery.
        }
    }

    // Ensure Systick and its interrupt are disabled before jumping.
    systick_counter_disable();
    systick_interrupt_disable();

    // Set the Vector Table Offset Register (VTOR) to the start of the application's vector table.
    SCB_VTOR = MAIN_APP_START_ADDRESS;

    // Load the application's initial stack pointer into the Main Stack Pointer (MSP).
    __asm volatile("MSR msp, %0" : : "r"(app_initial_msp) : "memory");

    // Create a function pointer to the application's reset handler.
    application_entry_t app_entry = (application_entry_t)app_reset_handler_addr;

    // Jump to the application's reset handler.
    app_entry();
}

// Main function of the bootloader.
int main(void) {
    // Initialize RCC and Systick.
    rcc_setup();
    systick_setup();

    // Record the start ticks.
    ticks_t start_ticks = ticks;
    // Loop indefinitely, checking for a timeout to jump to the main application.
    while(true) {
        if(ticks - start_ticks >= get_delay_ticks(5)) {
            jump_to_main();
        }
    }

    return 0;
}
