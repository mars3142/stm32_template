//
// Copyright (c) Peter Siegmund and contributors. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#include "global.h"
#include "blink.h"

#define BOOTLOADER_SIZE (0x8000U)
#define UART_PORT       (USART1)

static void usart_setup(void) {
    // Enable the USART1 interrupt in the NVIC if you want to use the usart1_isr for receiving data.
    nvic_enable_irq(NVIC_USART1_IRQ);
    nvic_set_priority(NVIC_USART1_IRQ, 1);

    // Enable clock for GPIOA (for PA9 TX, PA10 RX)
    rcc_periph_clock_enable(RCC_GPIOA);
    // Enable clock for USART1
    rcc_periph_clock_enable(RCC_USART1);

    // Setup GPIO pins for USART1 TX (PA9) and RX (PA10).
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10);
    gpio_set_af(GPIOA, GPIO_AF7, GPIO9 | GPIO10);

    // Configure USART1 parameters.
    usart_set_baudrate(UART_PORT, 115200);
    usart_set_databits(UART_PORT, 8);
    usart_set_parity(UART_PORT, USART_PARITY_NONE);
    usart_set_stopbits(UART_PORT, USART_STOPBITS_1);
    usart_set_mode(UART_PORT, USART_MODE_TX_RX);
    usart_set_flow_control(UART_PORT, USART_FLOWCONTROL_NONE);

    // Enable USART1.
    usart_enable(UART_PORT);

    // enable RX interrupt for USART1
    usart_enable_rx_interrupt(UART_PORT);
}

void usart1_isr(void) {
    if(usart_get_flag(UART_PORT, USART_SR_RXNE)) {
        usart_send_blocking(UART_PORT, usart_recv(UART_PORT));
    }
}

int main(void) {
    int i, j = 0, c = 0;

    gpio_setup();
    tim_setup();
    usart_setup();

    while(true) {
        usart_send_blocking(UART_PORT, c + '0'); /* USART1: Send byte. */
        c = (c == 9) ? 0 : c + 1; /* Increment c. */
        if((j++ % 80) == 0) { /* Newline after line full. */
            usart_send_blocking(UART_PORT, '\r');
            usart_send_blocking(UART_PORT, '\n');
        }
        for(i = 0; i < 300000; i++) { /* Wait a bit. */
            __asm__("NOP");
        }
    }

    return 0;
}
