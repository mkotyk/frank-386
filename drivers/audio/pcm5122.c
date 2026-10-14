#include "pcm5122.h"

#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <stddef.h>
#include <stdint.h>

#define PCM5122_I2C i2c1
#define PCM5122_I2C_ADDR 0x4Cu
#define PCM5122_SDA_PIN 2u
#define PCM5122_SCL_PIN 3u

#define PCM5122_REG_PAGE_SELECT 0x00u
#define PCM5122_REG_PLL_REF 0x0Du
#define PCM5122_REG_ERROR_DETECT 0x25u
#define PCM5122_REG_I2S_CONFIG 0x28u

#define PCM5122_PLL_REF_BCK 0x10u
#define PCM5122_IGNORE_SCK_HALT 0x08u
#define PCM5122_I2S_16BIT 0x00u

typedef struct {
    uint8_t reg;
    uint8_t value;
} pcm5122_reg_write_t;

static bool pcm5122_write_register(uint8_t reg, uint8_t value)
{
    const uint8_t transfer[2] = { reg, value };
    return i2c_write_blocking(PCM5122_I2C, PCM5122_I2C_ADDR,
                              transfer, sizeof(transfer), false) ==
           (int)sizeof(transfer);
}

bool pcm5122_init(void)
{
    i2c_init(PCM5122_I2C, 100000u);
    gpio_set_function(PCM5122_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PCM5122_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PCM5122_SDA_PIN);
    gpio_pull_up(PCM5122_SCL_PIN);

    /*
     * In software mode the DAC is a three-wire I2S slave: BCK is the PLL
     * reference, there is no external SCK, and the serializer supplies 16-bit
     * samples in 32-BCK stereo frames. DCAS=0 keeps automatic clock setup on.
     */
    static const pcm5122_reg_write_t setup[] = {
        { PCM5122_REG_PAGE_SELECT, 0x00u },
        { PCM5122_REG_ERROR_DETECT, PCM5122_IGNORE_SCK_HALT },
        { PCM5122_REG_PLL_REF, PCM5122_PLL_REF_BCK },
        { PCM5122_REG_I2S_CONFIG, PCM5122_I2S_16BIT },
    };

    for (size_t i = 0; i < sizeof(setup) / sizeof(setup[0]); ++i) {
        if (!pcm5122_write_register(setup[i].reg, setup[i].value))
            return false;
    }

    return true;
}
