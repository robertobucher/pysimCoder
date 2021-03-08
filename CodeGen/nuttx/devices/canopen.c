/*
COPYRIGHT (C) 2016  Roberto Bucher (roberto.bucher@supsi.ch)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#include <sys/types.h>
#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/can/can.h>

/* #define VERB */

int canFD;
static int dev_cnt = 0;                    /* CAN devices counter */
static volatile int endrcv = 0;
static pthread_t  rt_rcv;

struct CanMsg{
  int ID;
  uint16_t index;
  uint8_t subindex;
  uint32_t value;
  struct CanMsg *next;
};

struct CanMsg *msgIn = NULL;

int registerMsg(int ID, uint16_t index, uint8_t subindex)
{
  struct CanMsg * el = (struct CanMsg *) malloc(sizeof(struct CanMsg));
  el->next = msgIn;
  msgIn = el;
  el->ID = ID;
  el->index = index;
  el->subindex = subindex;
  el->value = 0;
  return(0);
}

int getValue(int ID, uint16_t index, uint8_t subindex)
{
  struct CanMsg * el = msgIn;
  while(el!=NULL){
    if((el->ID==ID) &&
       (el->index==index) &&
       (el->subindex==subindex))
      return((int) el->value);
    el = el->next;
  }
  return(0);
}

short get2ByteValue(int ID, uint16_t index, uint8_t subindex)
{
  struct CanMsg * el = msgIn;
  while(el!=NULL){
    if((el->ID==ID) &&
       (el->index==index) &&
       (el->subindex==subindex))
      return((short int) el->value);
    el = el->next;
  }
  return(0);
}

void saveMsg(struct   can_msg_s m)
{
  int ID = m.cm_hdr.ch_id;
  uint16_t *index = (uint16_t *) &(m.cm_data[1]);
  uint8_t *subindex = (uint8_t *) &(m.cm_data[3]);
  uint32_t *value = (uint32_t *) &(m.cm_data[4]);

  struct CanMsg * el = msgIn;
  
  while(el!=NULL){
    if((el->ID==ID) &&
       (el->index==*index) &&
       (el->subindex==*subindex)){
      el->value = *value;
    }
    el = el->next;    
  }
}
  
void saveMsg2(struct   can_msg_s m)
{
  int ID = m.cm_hdr.ch_id;
  
  uint16_t index = 0x00;
  uint8_t subindex = 0x00;
  uint32_t value = (m.cm_data[3] << 24)  + (m.cm_data[2] << 16) +
                             (m.cm_data[5] << 8) + m.cm_data[4];

  struct CanMsg * el = msgIn;
  
  while(el!=NULL){
    if((el->ID==ID) &&
       (el->index==index) &&
       (el->subindex==subindex)){
      el->value = value;
    }
    el = el->next;    
  }
}
  
void sendMsg(uint16_t ID, uint8_t DATA[], int len)
{
  /* Procedure to send a CAN message */

  struct can_msg_s txmsg;
  ssize_t nbytes;
  size_t msgsize;

#ifdef VERB
  int i;
#endif

  txmsg.cm_hdr.ch_id     = ID;
  txmsg.cm_hdr.ch_rtr    = false;
  txmsg.cm_hdr.ch_dlc    = len;
#ifdef CONFIG_CAN_ERRORS
  txmsg.cm_hdr.ch_error  = 0;
#endif
#ifdef CONFIG_CAN_EXTID
  txmsg.cm_hdr.ch_extid  = extended;
#endif
  txmsg.cm_hdr.ch_unused = 0;

  /* for (i = 0; i < msgdlc; i++){ */
  /*   txmsg.cm_data[i] = DATA[i]; */
  /* } */
  
  memcpy(txmsg.cm_data, DATA, len);
  
#ifdef VERB
    printf("--> 0x%03x  %d   ",txmsg.cm_hdr.ch_id,  txmsg.cm_hdr.ch_dlc);
    for(i=0;i<len;i++) printf("0x%02x  ", txmsg.cm_data[i]);
    printf("\n");    
#endif

    /* Send the TX message */

    msgsize = CAN_MSGLEN(len);
    nbytes = write(canFD, &txmsg, msgsize);
    if (nbytes != msgsize) {
      printf("ERROR: write(%ld) returned %ld\n",
	     (long)msgsize, (long)nbytes);
    }
}

int rcvMsgCob(int cob, uint8_t DATA[], int timeout)
{
  struct can_msg_s rxmsg;
  ssize_t nbytes;
  size_t msgsize;

#ifdef VERB
  int i;
#endif

  msgsize = sizeof(struct can_msg_s);
  do{
  nbytes = read(canFD, &rxmsg, msgsize);
  }while(rxmsg.cm_hdr.ch_id != cob);
 
  nbytes = read(canFD, &rxmsg, msgsize);
  if (nbytes < CAN_MSGLEN(0) || nbytes > msgsize){
    printf("ERROR: read(%ld) returned %ld\n",
	   (long)msgsize, (long)nbytes);
  }
#ifdef VERB
  else{
    printf("<-- 0x%03x  %d   ",rxmsg.cm_hdr.ch_id, rxmsg.cm_hdr.ch_dlc);
    for(i=0;i<rxmsg.cm_hdr.ch_dlc;i++) printf("0x%02x  ", rxmsg.cm_data[i]);
    printf("\n");
  }
#endif

  if(rxmsg.cm_hdr.ch_dlc != 0) memcpy(DATA, rxmsg.cm_data, rxmsg.cm_hdr.ch_dlc);
  return rxmsg.cm_hdr.ch_dlc;
}
  
int rcvMsg(uint8_t DATA[], int timeout)
{
  struct can_msg_s rxmsg;
  ssize_t nbytes;
  size_t msgsize;

#ifdef VERB
  int i;
#endif

  msgsize = sizeof(struct can_msg_s);
  nbytes = read(canFD, &rxmsg, msgsize);

  if (nbytes < CAN_MSGLEN(0) || nbytes > msgsize){
    printf("ERROR: read(%ld) returned %ld\n",
	   (long)msgsize, (long)nbytes);
  }
#ifdef VERB
  else{
    printf("<-- 0x%03x  %d   ",rxmsg.cm_hdr.ch_id, rxmsg.cm_hdr.ch_dlc);
    for(i=0;i<rxmsg.cm_hdr.ch_dlc;i++) printf("0x%02x  ", rxmsg.cm_data[i]);
    printf("\n");
  }
#endif

  if(rxmsg.cm_hdr.ch_dlc != 0) memcpy(DATA, rxmsg.cm_data, rxmsg.cm_hdr.ch_dlc);
  return rxmsg.cm_hdr.ch_dlc;
}

void *rcv(void *args)
{
  /* Receiving thread scheduled as RT task */
  struct can_msg_s rxmsg;
  ssize_t nbytes;
  size_t msgsize;

#ifdef VERB
  int i;
#endif
  
  struct sched_param param;

  param.sched_priority = (int) args;
  if(sched_setscheduler(0, SCHED_FIFO, &param)==-1){
    perror("sched_setscheduler failed");
    exit(-1);
  }

#ifdef HAVE_MLOCK
  mlockall(MCL_CURRENT | MCL_FUTURE);
#endif
  
  msgsize = sizeof(struct can_msg_s);

  while (!endrcv) {       /* receiving loop */
    nbytes = read(canFD, &rxmsg, msgsize);

#ifdef VERB
    printf("<-- 0x%03x  %d   ",rxmsg.cm_hdr.ch_id, rxmsg.cm_hdr.ch_dlc);
    for(i=0;i<rxmsg.cm_hdr.ch_dlc;i++) printf("0x%02x  ", rxmsg.cm_data[i]);
    printf("\n");
#endif

    /* Store messages  */
    if(rxmsg.cm_data[0] != 0x01) saveMsg(rxmsg);
    else                              saveMsg2(rxmsg);
    
  }
  return 0;
}

int canOpen(char * dev)
{
  if(!dev_cnt){  /* This task is performed only one time */
    canFD = open(dev, O_RDWR);

    if (canFD < 0) {
      printf("ERROR: open %s failed\n", dev);
      return -1;
    }
  }
  dev_cnt++;
  return 0;
}

int canOpenTH(char * dev)
{
  int priority = 90;

  if(!dev_cnt){  /* This task is performed only one time */
    canFD = open(dev, O_RDWR);

    if (canFD < 0) {
      printf("ERROR: open %s failed\n", dev);
     return -1;
    }
    pthread_create(&rt_rcv, NULL, rcv, (void *) priority);  /* Start receiving task */    
  }
  dev_cnt++;
  return 0;
}

void canClose(void)
{
  if(--dev_cnt == 0) {
    close(canFD);
  }
}

void canopen_synch(void)
{
  sendMsg(0x80,NULL,0);
}



