#ifndef _RPI_GPIO_H
#define _RPI_GPIO_H

typedef struct rpi_registers_mapping_t {
    int mapping_initialized;
    int mem_fd;
    void *gpio_map;
    void *pwm_map;
    void *clk_map;
    volatile unsigned *gpio_base;
    volatile unsigned *pwm_base;
    volatile unsigned *clk_base;
} rpi_registers_mapping_t;

extern rpi_registers_mapping_t rpi_registers_mapping;

#define GPIO_SET (*(rpi_registers_mapping.gpio_base+7))
#define GPIO_CLR (*(rpi_registers_mapping.gpio_base+10))
#define GPIO_INP (*(rpi_registers_mapping.gpio_base+13))

static inline int rpi_gpio_get_value(unsigned gpio)
{
    return GPIO_INP & (1 << gpio)? 1: 0;
}

/* Set gpio pin output set to specifies level */
static inline void rpi_gpio_set_value(unsigned gpio, int value)
{
    if (value)
       GPIO_SET = 1 << gpio;
    else
       GPIO_CLR = 1 << gpio;
}

/* Configure gpio_base pin for input */
int rpi_gpio_direction_input(unsigned gpio);

/* Configure gpio_base pin for output */
int rpi_gpio_direction_output(unsigned gpio, int value);

/* Configure gpio_base pin for alternate function */
int rpi_gpio_alt_fnc(unsigned gpio, int alt_fnc);

int rpi_peripheral_registers_map(void);

#endif /*_RPI_BIDIRPWM_H*/
