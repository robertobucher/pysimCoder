#include <stdint.h>
#include <stddef.h>

typedef struct {
    volatile uint32_t *lat;
    volatile uint32_t *tris;
    volatile uint32_t *port;
    volatile uint32_t *ansel; 
} gpio_port_hw_t;

typedef enum {
    GPIO_PORT_A = 0,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    GPIO_PORT_E,
    GPIO_PORT_F,
    GPIO_PORT_G,
    GPIO_PORT_H
} gpio_port_t;

typedef enum {
    GPIO_INPUT,
    GPIO_OUTPUT
} gpio_dir_t;

typedef enum {
    GPIO_DIGITAL,
    GPIO_ANALOG
} gpio_mode_t;

void gpio_config(gpio_port_t port,
                 uint8_t pin,
                 gpio_dir_t dir,
                 gpio_mode_t mode);

void gpio_write(gpio_port_t port, uint8_t pin, uint8_t value);

uint8_t gpio_read(gpio_port_t port, uint8_t pin);

uint16_t adc_read(uint8_t an);




