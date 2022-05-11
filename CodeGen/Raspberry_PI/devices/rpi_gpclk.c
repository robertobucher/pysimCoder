/**
 * (C) 2015 by Martin Prudek prudemar@fel.cvut.cz
 * (C) 2015 by Pavel Pisa pisa@cmp.felk.cvut.cz
 *
 * Configuration of the General Purpose Clocks outputs
 * Inspired by wiringPi written by Gordon Henderson.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

#include "rpi_gpio.h"
#include "rpi_gpclk.h"

#define CLK_GPx_CTL(chan) \
  (rpi_registers_mapping.clk_base[28 + 2 * (chan)])

#define CLK_GPx_DIV(chan) \
  (rpi_registers_mapping.clk_base[29 + 2 * (chan)])

#define CLK_CTL_SRC_OSC  1  /* 19.2 MHz */
#define CLK_CTL_SRC_PLLC 5  /* 1000 MHz */
#define CLK_CTL_SRC_PLLD 6  /*  500 MHz */
#define CLK_CTL_SRC_HDMI 7  /*  216 MHz */

#define CLK_PASSWD  (0x5A<<24)
#define CLK_CTL_MASH(x)((x)<<9)
#define CLK_CTL_BUSY    (1 <<7)
#define CLK_CTL_KILL    (1 <<5)
#define CLK_CTL_ENAB    (1 <<4)
#define CLK_CTL_SRC(x) ((x)<<0)

#define CLK_DIV_DIVI(x) ((x)<<12)
#define CLK_DIV_DIVF(x) ((x)<< 0)

uint32_t rpi_gpclk_src_to_reg[] = {
    [RPI_GPCLK_PLLD_500_MHZ] = CLK_CTL_SRC_PLLD,
    [RPI_GPCLK_OSC_19_MHZ_2] = CLK_CTL_SRC_OSC,
    [RPI_GPCLK_HDMI_216_MHZ] = CLK_CTL_SRC_HDMI,
    [RPI_GPCLK_PLLC_1000_MHZ] = CLK_CTL_SRC_PLLD,
};

int rpi_gpclk_setup(int chan, int source, int div_int, int div_frac)
{
    int MASH = 0;
    uint32_t clksrc;

    if (!rpi_registers_mapping.mapping_initialized)
        return -1;

    if ((source < 0) || (source > sizeof(rpi_gpclk_src_to_reg) /
         sizeof(*rpi_gpclk_src_to_reg) ))
        return -2;

    if ((div_int  < 2) || (div_int   > 4095))
        return -3;

    if ((div_frac < 0) || (div_frac  > 4095))
        return -4;

    if ((MASH   < 0) || (MASH   > 3))
        return -5;

    clksrc = rpi_gpclk_src_to_reg[source];

    CLK_GPx_CTL(chan) = CLK_PASSWD | CLK_CTL_KILL;

    while (CLK_GPx_CTL(chan) & CLK_CTL_BUSY){
        usleep(10);
    }

    CLK_GPx_DIV(chan) = (CLK_PASSWD | CLK_DIV_DIVI(div_int) | CLK_DIV_DIVF(div_frac));

    usleep(10);

    CLK_GPx_CTL(chan) = (CLK_PASSWD | CLK_CTL_MASH(MASH) | CLK_CTL_SRC(clksrc));

    usleep(10);

    CLK_GPx_CTL(chan) |= (CLK_PASSWD | CLK_CTL_ENAB);

    return 0;
}
