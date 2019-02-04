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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

/* #define VERB */

#define DWORD  unsigned int
#define WORD   unsigned short
#define BYTE   unsigned char

static int s;                       /* CAN Socket */
static int dev_cnt = 0;                    /* CAN devices counter */

static volatile int endrcv = 0;
static pthread_t  rt_rcv;

struct CanMsg{int ID;
  WORD index;
  BYTE subindex;
  DWORD value;
  struct CanMsg *next;
};

struct CanMsg *msgIn = NULL;

int registerMsg(int ID, WORD index, BYTE subindex)
{
  struct CanMsg * el = (struct CanMsg *) malloc(sizeof(struct CanMsg));
  el->next = msgIn;
  msgIn = el;
  el->ID = ID;
  el->index = index;
  el->subindex = subindex;
  el->value = 0;
}

int getValue(int ID, WORD index, BYTE subindex)
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

short get2ByteValue(int ID, WORD index, BYTE subindex)
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

void saveMsg(struct can_frame m)
{
  int ID = m.can_id;
  
  WORD *index = (WORD *) &(m.data[1]);
  BYTE *subindex = (BYTE *) &(m.data[3]);
  DWORD *value = (DWORD *) &(m.data[4]);

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
  
void saveMsg2(struct can_frame m)
{
  int ID = m.can_id;
  
  WORD index = 0x00;
  BYTE subindex = 0x00;
  DWORD value = (m.data[3] << 24)  + (m.data[2] << 16) +
                             (m.data[5] << 8) + m.data[4];

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
  
void sendMsg(WORD ID, BYTE DATA[], int len)
{
  /* Procedure to send a CAN message */

  struct can_frame msg;

  memset(&msg, 0x00, sizeof(msg));

  msg.can_id = ID;
  msg.can_dlc = len;
  int i;

  memcpy(msg.data, DATA, msg.can_dlc);
  
#ifdef VERB
    printf("--> 0x%03x  %d   ",msg.can_id,msg.can_dlc);
    for(i=0;i<msg.can_dlc;i++) printf("0x%02x  ",msg.data[i]);
    printf("\n");    
#endif

    len = write(s, &msg, sizeof(struct can_frame));
}


  
int rcvMsg(BYTE DATA[], int timeout)
{
  struct can_frame msg;
  int len;

#ifdef VERB
  int i;
#endif

  len = read(s, &msg, sizeof(struct can_frame));

#ifdef VERB
  printf("<-- 0x%03x  %d   ", msg.can_id, msg.can_dlc);
  for(i=0;i<msg.can_dlc;i++) printf("0x%02x  ", msg.data[i]);
    printf("\n");    
#endif

  if(msg.can_dlc != 0) memcpy(DATA, msg.data, msg.can_dlc);
  return msg.can_dlc;
}

void *rcv(void *args)
{
  /* Receiving thread scheduled as RT task */

  struct can_frame msg;
  int len;
  
#ifdef VERB
  int i;
#endif
  struct sched_param param;

  param.sched_priority = (int) args;
  if(sched_setscheduler(0, SCHED_FIFO, &param)==-1){
    perror("sched_setscheduler failed");
    exit(-1);
  }

  mlockall(MCL_CURRENT | MCL_FUTURE);

  while (!endrcv) {       /* receiving loop */
    len = read(s, &msg, sizeof(struct can_frame));

#ifdef VERB
    printf("<-- 0x%03x  %d   ",msg.can_id, msg.can_dlc);
    for(i=0;i<msg.can_dlc;i++) printf("0x%02x  ", msg.data[i]);
    printf("\n");    
#endif

    /* Store messages  */
    saveMsg(msg);    
  }
  return 0;
}

int canOpen()
{
  struct sockaddr_can addr;
  struct ifreq ifr;
  int ret;

  if(!dev_cnt){  /* This task is performed only one time */
    memset(&ifr, 0x00, sizeof(ifr));
    memset(&addr,0x00, sizeof(addr));

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(!s) return -1;
    
    strcpy(ifr.ifr_name, "can0" );
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    int canfd_on = 1;
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on, sizeof(canfd_on));

    /* int loopback = 0; */
    /* setsockopt(s, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback)); */

    ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
  }
  dev_cnt++;
  return 0;
}

int canOpenTH()
{
  struct sockaddr_can addr;
  struct ifreq ifr;
  int priority = 90;
  int ret;

  if(!dev_cnt){  /* This task is performed only one time */
    memset(&ifr, 0x00, sizeof(ifr));
    memset(&addr,0x00, sizeof(addr));

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(!s) return -1;
    
    strcpy(ifr.ifr_name, "can0" );
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    int canfd_on = 1;
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on, sizeof(canfd_on));
    
   /* int loopback = 0; */
    /* setsockopt(s, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback)); */

    ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));

    pthread_create(&rt_rcv, NULL, rcv, (void *) priority);  /* Start receiving task */
  }
  
  dev_cnt++;
  return 0;
}

void canClose()
{
  if(--dev_cnt == 0) {
    close(s);
  }
}

void canopen_synch()
{
  sendMsg(0x80,NULL,0);
}



