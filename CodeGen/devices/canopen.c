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
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>

#include <pcan.h>
#include <libpcan.h>

/* #define VERB */

static void * canHandle;
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

void saveMsg(TPCANMsg m)
{
  int ID = m.ID;
  WORD *index = (WORD *) &(m.DATA[1]);
  BYTE *subindex = (BYTE *) &(m.DATA[3]);
  DWORD *value = (DWORD *) &(m.DATA[4]);

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
  
void saveMsg2(TPCANMsg m)
{
  int ID = m.ID;
  
  WORD index = 0x00;
  BYTE subindex = 0x00;
  DWORD value = (m.DATA[3] << 24)  + (m.DATA[2] << 16) +
                             (m.DATA[5] << 8) + m.DATA[4];

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

  TPCANMsg Tmsg;

  Tmsg.ID = ID;
  Tmsg.MSGTYPE = MSGTYPE_STANDARD;
  Tmsg.LEN = len;
  int i, errno;

  memcpy(Tmsg.DATA, DATA, len);
  
#ifdef VERB
    printf("--> 0x%03x  %d   ",Tmsg.ID,Tmsg.LEN);
    for(i=0;i<Tmsg.LEN;i++) printf("0x%02x  ",Tmsg.DATA[i]);
    printf("\n");    
#endif

    errno = CAN_Write(canHandle,&Tmsg);
}

int rcvMsgCob(int cob, BYTE DATA[], int timeout)
{
  TPCANRdMsg m;
  int errno;

#ifdef VERB
  int i;
#endif

  do{
    errno = LINUX_CAN_Read_Timeout(canHandle, &m, timeout);
  }while(m.Msg.ID != cob);

  if(errno==0){
#ifdef VERB
    printf("<-- 0x%03x  %d   ",m.Msg.ID,m.Msg.LEN);
    for(i=0;i<m.Msg.LEN;i++) printf("0x%02x  ",m.Msg.DATA[i]);
    printf("\n");    
#endif

    if(m.Msg.LEN != 0) memcpy(DATA,m.Msg.DATA,m.Msg.LEN);
    if(m.Msg.MSGTYPE & MSGTYPE_STATUS) CAN_Status(canHandle);
    return m.Msg.LEN;
  }
  else return 0;
}
  

int rcvMsg(BYTE DATA[], int timeout)
{
  TPCANRdMsg m;
  int errno;

#ifdef VERB
  int i;
#endif

  errno = LINUX_CAN_Read_Timeout(canHandle, &m, timeout);

  if(errno==0){
#ifdef VERB
    printf("<-- 0x%03x  %d   ",m.Msg.ID,m.Msg.LEN);
    for(i=0;i<m.Msg.LEN;i++) printf("0x%02x  ",m.Msg.DATA[i]);
    printf("\n");    
#endif

    if(m.Msg.LEN != 0) memcpy(DATA,m.Msg.DATA,m.Msg.LEN);
    if(m.Msg.MSGTYPE & MSGTYPE_STATUS) CAN_Status(canHandle);
    return m.Msg.LEN;
  }
  else return 0;
}

void *rcv(void *args)
{
  /* Receiving thread scheduled as RT task */

  TPCANMsg m;
  int errno;
#ifdef VERB
  int i;
#endif
  int id, PDOn;
  BYTE * data;
  struct sched_param param;

  param.sched_priority = (int) args;
  if(sched_setscheduler(0, SCHED_FIFO, &param)==-1){
    perror("sched_setscheduler failed");
    exit(-1);
  }

  mlockall(MCL_CURRENT | MCL_FUTURE);

  while (!endrcv) {       /* receiving loop */
    errno = CAN_Read(canHandle, &m);

#ifdef VERB
    printf("<-- 0x%03x  %d   ",m.ID,m.LEN);
    for(i=0;i<m.LEN;i++) printf("0x%02x  ",m.DATA[i]);
    printf("\n");    
#endif

    /* Store messages  */
    if(m.DATA[0] != 0x01) saveMsg(m);
    else                              saveMsg2(m);
    
    if(m.MSGTYPE & MSGTYPE_STATUS) CAN_Status(canHandle);
  }
  return 0;
}

int canOpen()
{
  int bd;

  char txt[VERSIONSTRING_LEN];

  if(!dev_cnt){  /* This task is performed only one time */
    bd = CAN_BAUD_500K;
    canHandle = LINUX_CAN_Open("/dev/pcan32",O_RDWR);
    /* canHandle = CAN_Open(HW_DONGLE_SJA_EPP, port,irq); */

    if(!canHandle) return -1;
    CAN_VersionInfo(canHandle,txt);
    CAN_Init(canHandle, bd, CAN_INIT_TYPE_ST);
  }
  dev_cnt++;
  return 0;
}

int canOpenTH()
{
  int bd;
  int priority = 90;

  char txt[VERSIONSTRING_LEN];

  if(!dev_cnt){  /* This task is performed only one time */
    bd = CAN_BAUD_500K;
    canHandle = LINUX_CAN_Open("/dev/pcan32",O_RDWR);
    /* canHandle = CAN_Open(HW_DONGLE_SJA_EPP, port,irq); */

    if(!canHandle) return -1;
    CAN_VersionInfo(canHandle,txt);
    CAN_Init(canHandle, bd, CAN_INIT_TYPE_ST);
    pthread_create(&rt_rcv, NULL, rcv, (void *) priority);  /* Start receiving task */
  }
  
  dev_cnt++;
  return 0;
}

void canClose()
{
  if(--dev_cnt == 0) {
    CAN_Close(canHandle);
  }
}

void canopen_synch()
{
  sendMsg(0x80,NULL,0);
}



