/***************************************************************************
 *
 * comedi_pwm.c
 *
 * TU Berlin -- Fachgebiet Regelungssysteme
 *  -> Dateibeschreibung:  PWM output via national card counter dev
 *  -> Authors:            Ardit Dvorani, Tim M. Berghoff
 *
 */

/* Note: Die Karte hat keine PWM-Modul.
Das Signal wird durch Timer Counter erzeugt */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <asm/types.h>
#include <math.h>
#include <pyblock.h>
#include <comedilib.h>

#define START_SUBD 0

#define CONST_CH 0
#define BRAKE_CH 1
#define HEAT_CH 4

#define DIO_HIGH 1
#define DIO_LOW 0

#define CHANNEL_PAR_NUM 0
#define PRESCALER_PAR_NUM 1
#define PFIOUT_PAR_NUM 2

#define NUM_COUNTER_CH 3

#define INITIAL_PWM 0.0

//#define DEBUG

typedef struct {
	comedi_t * dev;
	unsigned dio;
	unsigned counter;
	unsigned pfiout;
	unsigned prescaler;
	char * devname;
} comedi_pwm ;

static void comedi_pwm_out(python_block * block);
static void comedi_pwm_init(python_block * block);
static void comedi_pwm_end(python_block * block);
static int comedi_pwm_open(comedi_pwm * pwm);
static void comedi_pwm_write_dio(comedi_pwm * pwm, unsigned ch, unsigned val);
static void _comedi_pwm_out(double u, comedi_pwm * pwm);

void comedi_pwm_generator(int flag, python_block * block)
{
	switch (flag) {
	case CG_INIT:
		comedi_pwm_init(block);
		break;

	case CG_OUT:
		comedi_pwm_out(block);
		break;

	case CG_END:
		comedi_pwm_end(block);
		break;
	}
}

static void comedi_pwm_write_dio(comedi_pwm * pwm, unsigned ch, unsigned val)
{
	if (!pwm) {
		fprintf(stderr, "Invalid pwm struct pointer\n");
		return;
	}
	int ret = comedi_dio_write(pwm->dev, pwm->dio, ch, val);
	if (0 > ret)
		fprintf(stderr, "Error in "
			"comedi_pwm_out, data_write "
                        "subd%u ch%u\n", pwm->dio, ch);
}

static void comedi_pwm_end(python_block * block)
{
	comedi_pwm * pwm = (comedi_pwm *)block->ptrPar;

	int retval;
	lsampl_t data1[1];
	comedi_insn comedi_instruction_object;
	
	comedi_pwm_write_dio(pwm, CONST_CH, DIO_LOW);
	comedi_pwm_write_dio(pwm, BRAKE_CH, DIO_LOW);

	memset(&comedi_instruction_object, 0, sizeof(comedi_insn));

	comedi_instruction_object.insn = INSN_CONFIG;
	comedi_instruction_object.subdev = pwm->counter;
	comedi_instruction_object.chanspec = 0;
	comedi_instruction_object.data = data1;
	comedi_instruction_object.n = sizeof(data1) / sizeof(data1[0]);
	data1[0] = INSN_CONFIG_DISARM;

	retval = comedi_do_insn(pwm->dev, &comedi_instruction_object);
	if (retval < 0)
		fprintf(stderr, "comedi_do_insn error "
                        "(disarm) for device %d\n", pwm->counter);
	else
		fprintf(stderr, "disarm OK\n");

	fprintf(stderr, "Closing comedi dev %s\n", pwm->devname);
	comedi_close(pwm->dev);
	free(pwm);
}

static int comedi_pwm_open(comedi_pwm *pwm)
{
	pwm->dev = comedi_open(pwm->devname);
	if (!pwm->dev) {
		fprintf(stderr, "Comedi PWM %s, "
                        "could not be opened. Exiting\n", pwm->devname);
		return -1;
	} else
		fprintf(stderr, "Comedi PWM %s opened.\n", pwm->devname);
	
	/* locate subdev of type counter */
	pwm->counter = comedi_find_subdevice_by_type(pwm->dev,
                                                    COMEDI_SUBD_COUNTER,
                                                    START_SUBD);
	/* check whether a counter subdev was located */
	if (pwm->counter < 0) {
		fprintf(stderr, "Comedi find_subdevice failed (No Counter)\n");
		comedi_close(pwm->dev);
		return -1;
	} else
		fprintf(stderr, "PWM counter autodetection "
				"gave subdev number %d\n", pwm->counter);
	
	/* localte subdev dio */
	pwm->dio = comedi_find_subdevice_by_type(pwm->dev,
                                                 COMEDI_SUBD_DIO, START_SUBD);
	/* check whether a dio subdev was located */
	if (pwm->dio < 0) {
		fprintf(stderr, "Comedi find_subdevice failed (No DIO)\n");
		comedi_close(pwm->dev);
		return -1;
	} else
		fprintf(stderr, "PWM DIO autodetection "
				"gave subdev number %d\n", pwm->dio);
	
	/* config dio as output with pfiout */
	if (comedi_dio_config(pwm->dev, pwm->dio,
	                      pwm->pfiout, COMEDI_OUTPUT) < 0) {
		printf("Comedi PWM DIO config failed for "
                       "subdevice %d\n, ch%u", pwm->dio, pwm->pfiout);
		comedi_close(pwm->dev);
		return -1;
	}
	else
		fprintf(stderr, "DIO Subdevice "
		        "%d configured, ch%u\n", pwm->dio, pwm->pfiout);

	/* config dio as output channel CONST_CH */
	if (comedi_dio_config(pwm->dev, pwm->dio,
	                      CONST_CH, COMEDI_OUTPUT) < 0) {
		printf("Comedi PWM DIO config "
		       "failed for subdevice %d\n, ch%u", pwm->dio, CONST_CH);
		comedi_close(pwm->dev);
		return -1;
	}
	else
		fprintf(stderr, "DIO Subdevice "
			"%d configured, ch%u\n", pwm->dio, CONST_CH);

	/* config dio as output channel BRAKE_CH */
	if (comedi_dio_config(pwm->dev, pwm->dio,
	                      BRAKE_CH, COMEDI_OUTPUT) < 0) {
		printf("Comedi PWM DIO config "
		       "failed for subdevice %d\n, ch%u", pwm->dio, BRAKE_CH);
		comedi_close(pwm->dev);
		return -1;
	}
	else
		fprintf(stderr, "DIO Subdevice "
			"%d configured, ch%u\n", pwm->dio, BRAKE_CH);

	/* config dio as input channel HEAT_CH */
	if (comedi_dio_config(pwm->dev, pwm->dio,
	                      HEAT_CH, COMEDI_INPUT) < 0) {
		printf("Comedi PWM DIO config "
		       "failed for subdevice %d\n, ch%u", pwm->dio, BRAKE_CH);
		comedi_close(pwm->dev);
		return -1;
	}
	else
		fprintf(stderr, "DIO Subdevice "
			"%d configured, ch%u\n", pwm->dio, HEAT_CH);

	/* try to lock the counter found */
	if (comedi_lock(pwm->dev, pwm->counter) < 0) {
		fprintf(stderr, "Comedi lock "
                                "failed for subdev %d\n", pwm->counter);
		comedi_close(pwm->dev);
		return -1;
	}

	return 1;
}

static void comedi_pwm_init(python_block * block)
{
	comedi_pwm * pwm = malloc(sizeof(*pwm));
	if (!pwm) {
		fprintf(stderr, "Mem alloc error in comedi_pwm_init\n");
		exit(-1);
	}
	block->ptrPar = pwm;
	// set device name
	pwm->devname = block->str;
	// set prescaler
	pwm->prescaler = block->intPar[PRESCALER_PAR_NUM];
	if(pwm->prescaler > 16)
	 	pwm->prescaler = 16;
	// set pfiout
	pwm->pfiout = block->intPar[PFIOUT_PAR_NUM];

	fprintf(stderr, "initializing pwm:\n"
		            "devname=%s\nprescaler=%d\npfiout=%d\n",
		            pwm->devname, pwm->prescaler, pwm->pfiout);

	// try to open comedi device
	if(comedi_pwm_open(pwm) < 0) {
		fprintf(stderr, "Error opening %s! "
		                "Exiting...\n", pwm->devname);
		free(pwm);
		exit(-1);
	}

	int retval;
	lsampl_t counter_mode;
	const unsigned clock_period_ns = 50;	/* 20MHz clock */
	lsampl_t data1[1];
	lsampl_t data2[2];
	lsampl_t data3[3];
	comedi_insn comedi_instruction_object;
	memset(&comedi_instruction_object, 0, sizeof(comedi_insn));

	retval = comedi_reset(pwm->dev, pwm->counter);
	if (retval < 0)
		fprintf(stderr, "comedi_do_insn error "
                                "(reset_counter) for dev %s\n", pwm->devname);
	else
		fprintf(stderr, "reset counter OK\n");

	unsigned gate = 0;
	comedi_instruction_object.insn = INSN_CONFIG;
	comedi_instruction_object.subdev = pwm->counter;
	comedi_instruction_object.chanspec = 0;
	comedi_instruction_object.data = data3;
	comedi_instruction_object.n = sizeof(data3) / sizeof(data3[0]);
	data3[0] = INSN_CONFIG_SET_GATE_SRC;
	data3[2] = NI_GPCT_DISABLED_GATE_SELECT | CR_EDGE;
	for (; 2 > gate; gate++) {
		data3[1] = gate;
		retval = comedi_do_insn(pwm->dev, &comedi_instruction_object);
		if (retval < 0)
			fprintf(stderr, "comedi_do_insn error (set gate %u)"
                                        " for dev %s\n", gate+1, pwm->devname);
		else
			fprintf(stderr, "set gate %u OK\n", gate+1);    
	}

	counter_mode = NI_GPCT_COUNTING_MODE_NORMAL_BITS;
	// toggle output on terminal count
	counter_mode |= NI_GPCT_OUTPUT_TC_TOGGLE_BITS;
	// load on terminal count
	counter_mode |= NI_GPCT_LOADING_ON_TC_BIT;
	// alternate the reload source between the load a and load b registers
	counter_mode |= NI_GPCT_RELOAD_SOURCE_SWITCHING_BITS;
	// count down
	counter_mode |= NI_GPCT_COUNTING_DIRECTION_DOWN_BITS;
	// initialize load source as load b register
	counter_mode |= NI_GPCT_LOAD_B_SELECT_BIT;
	// do not stop on terminal count
	counter_mode |= NI_GPCT_STOP_ON_GATE_BITS;
	// do not disarm on terminal count or gate signal
	counter_mode |= NI_GPCT_NO_HARDWARE_DISARM_BITS;

	retval = comedi_set_counter_mode(pwm->dev,
                                         pwm->counter, 0, counter_mode);
	if (retval < 0)
		fprintf(stderr, "comedi_do_insn error "
                                "(set counter) for dev %s\n", pwm->devname);
	else
		fprintf(stderr, "set counter OK\n");    

	comedi_instruction_object.insn = INSN_CONFIG;
	comedi_instruction_object.subdev = pwm->counter;
	comedi_instruction_object.chanspec = 0;
	comedi_instruction_object.data = data3;
	comedi_instruction_object.n = sizeof(data3) / sizeof(data3[0]);
	data3[0] = INSN_CONFIG_SET_CLOCK_SRC;
	data3[1] = NI_GPCT_TIMEBASE_1_CLOCK_SRC_BITS;
	data3[2] = clock_period_ns;
	retval = comedi_do_insn(pwm->dev, &comedi_instruction_object);
	if (retval < 0)
		fprintf(stderr, "comedi_do_insn error "
                                "(set clock) for dev %s\n", pwm->devname);
	else
		fprintf(stderr, "set clock OK\n");    

	comedi_instruction_object.insn = INSN_CONFIG;
	comedi_instruction_object.subdev = pwm->counter;
	comedi_instruction_object.chanspec = 0;
	comedi_instruction_object.data = data2;
	comedi_instruction_object.n = sizeof(data2) / sizeof(data2[0]);
	data2[0] = INSN_CONFIG_ARM;
	data2[1] = NI_GPCT_ARM_IMMEDIATE;
	retval = comedi_do_insn(pwm->dev, &comedi_instruction_object);
	if (retval < 0)
		fprintf(stderr, "comedi_do_insn error "
                                "(arm) for dev %s\n", pwm->devname);
	else
		fprintf(stderr, "arm OK\n");    

	/* set some initial pwm */
	_comedi_pwm_out(INITIAL_PWM, pwm);
}

static void comedi_pwm_out(python_block * block)
{
    	comedi_pwm * pwm = (comedi_pwm *)block->ptrPar;
	double u = *(double *)block->u[0];
	_comedi_pwm_out(u, pwm);
}

static __inline void _comedi_pwm_out(double u, comedi_pwm * pwm)
{
	unsigned up_ticks, down_ticks;
	const unsigned clock_period_ns = 50;	/* 20MHz clock */

	if (u < 0)
		u = -u;
	if (u > 0.99)
		u = 0.99;

	u = 1 - u;

	/* Standard Pulse delay (expressed in multiples of 20 MHz) */
	int pulse_period = 2048 * (pwm->prescaler + 1);
	/* Standard Pulse width */
	int pulse_width = (2048 * (pwm->prescaler + 1) - 20) * u;
	unsigned period_ns = clock_period_ns * pulse_period;
	unsigned up_time_ns = pulse_width * clock_period_ns;

	up_ticks = (up_time_ns + clock_period_ns/2)/clock_period_ns;
	down_ticks = (period_ns + clock_period_ns/2)/clock_period_ns - up_ticks;
	/* set initial counter value by writing to channel 0 */
	/* set "load a" register to the number of clock ticks */
        /* the counter output should remain low by writing to channel 1 */
	/* set "load b" register to the number of clock ticks */
        /* the counter output should remain high by writing to channel 2 */
	int ret = 0;
	unsigned ch = 0;
	for (; NUM_COUNTER_CH > ch; ch++) {
		ret = comedi_data_write(pwm->dev, pwm->counter, ch, 0, 0,
                                        2 == ch ? up_ticks : down_ticks);
		if (0 > ret)
			fprintf(stderr, "Error in "
                                "comedi_pwm_out, data_write "
                                "subd%u ch%u\n", pwm->counter, ch);
	}

	/* Heat read of 1 is normal, 0 means brake */
	unsigned heat_read = 0;
	if (comedi_dio_read(pwm->dev, pwm->dio, HEAT_CH, &heat_read) < 0)
		fprintf(stderr, "error in comedi dio heat read\n");
	comedi_pwm_write_dio(pwm, CONST_CH, DIO_HIGH);
	comedi_pwm_write_dio(pwm, BRAKE_CH,
                             DIO_HIGH == heat_read ? DIO_HIGH : DIO_LOW);
#ifdef DEBUG
	fprintf(stderr, "comedi_pwm_out: up_ticks %u\n"
                        "comedi_pwm_out: down_ticks %u\n"
	                "comedi_pwm_out: heat_read %u\n\n",
                        up_ticks, down_ticks, heat_read);
#endif
}
