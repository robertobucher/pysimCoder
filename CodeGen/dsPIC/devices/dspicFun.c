#include <xc.h>
#include <dspicFun.h>
#include <dspicConf.h>

void gpio_config(gpio_port_t port,
                 uint8_t pin,
                 gpio_dir_t dir,
                 gpio_mode_t mode)
{
    const gpio_port_hw_t *p = &gpio_ports[port];

     if (dir == GPIO_OUTPUT)
        *(p->tris) &= ~(1U << pin);
    else
        *(p->tris) |=  (1U << pin);

     if (p->ansel != NULL) {
        if (mode == GPIO_ANALOG)
            *(p->ansel) |=  (1U << pin);
        else
            *(p->ansel) &= ~(1U << pin);
    }
}

void gpio_write(gpio_port_t port, uint8_t pin, uint8_t value)
{
    if (value)
        *(gpio_ports[port].lat) |=  (1U << pin);
    else
        *(gpio_ports[port].lat) &= ~(1U << pin);
}

uint8_t gpio_read(gpio_port_t port, uint8_t pin)
{
    return ( (*(gpio_ports[port].port) >> pin) & 1U );
}

 
