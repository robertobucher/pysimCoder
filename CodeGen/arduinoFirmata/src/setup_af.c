#include <stdio.h>
#include <pyblock.h>
#include "firmata.h"


t_firmata * af_firmataInstance;

// TODO - this block should be initialized befdore any other arduinoFirmata blocks
//  currently it seems it is not possible to oreder it in python api
static void init(python_block * block) {
  char * port = (char * ) block -> str;

  af_firmataInstance = firmata_new(port);
  while (!af_firmataInstance -> isReady) {
    firmata_pull(af_firmataInstance);
  }
}

static void end(python_block * block) {
  printf("af - setup - end\n");
  // TODO: flush serial - inspiration Firmata Test Program -> void Serial::Close(void)
}

void setup_af(int flag, python_block * block) {

  if (flag == CG_OUT) {
  } else if (flag == CG_STUPD) {
  } else if (flag == CG_END) {
    /* termination */
    end(block);
  } else if (flag == CG_INIT) {
    /* initialisation */
    init(block);
  }
}
