//
// Copyright (c) Peter Siegmund and contributors. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include <libopencm3/cm3/scb.h>

#include "global.h"
#include "blink.h"

#define BOOTLOADER_SIZE (0x8000U)

static void vector_setup(void) {
    SCB_VTOR = BOOTLOADER_SIZE;
}

int main(void) {
    vector_setup();
    gpio_setup();
    tim_setup();

    while(true)
        ;

    return 0;
}
