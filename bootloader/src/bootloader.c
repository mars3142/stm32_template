#include <stdint.h>
#include <libopencm3/stm32/memorymap.h>

#define BOOTLOADER_SIZE        (0x8000U)
#define MAIN_APP_START_ADDRESS (FLASH_BASE + BOOTLOADER_SIZE)

static void jump_to_main(void) {
    typedef void (*void_fn)(void);

    uint32_t* reset_vector_entry = (uint32_t*)(MAIN_APP_START_ADDRESS + sizeof(uint32_t));
    uint32_t* reset_vector = (uint32_t*)(*reset_vector_entry);

    void_fn firmware_main = (void_fn)reset_vector;
    firmware_main();
}

int main(void) {
    jump_to_main();

    return 0;
}
