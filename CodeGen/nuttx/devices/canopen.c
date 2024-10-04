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

#ifdef CONFIG_CAN
#include <nuttx/can/can.h>
#endif

#ifdef CONFIG_NET_CAN
#include <nuttx/can.h>
#include <netpacket/can.h>
#include <sys/socket.h>
#include <net/if.h>
#endif

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

#ifdef CONFIG_NET_CAN
void socketCAN_saveMsg(struct can_frame m)
{
  int ID = m.can_id;
  uint16_t *index = (uint16_t *) &(m.data[1]);
  uint8_t *subindex = (uint8_t *) &(m.data[3]);
  uint32_t *value = (uint32_t *) &(m.data[4]);

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
#endif

#ifdef CONFIG_CAN
void CAN_saveMsg(struct   can_msg_s m)
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
#endif

/****************************************************************************
 * Name: sendMsg
 *
 * Description:
 *   Send CAN message
 *
 ****************************************************************************/

void sendMsg(uint16_t ID, uint8_t DATA[], int len)
{
  /* Procedure to send a CAN message */

#ifdef CONFIG_NET_CAN
  struct can_frame txmsg;
#else
  struct can_msg_s txmsg;
#endif
  ssize_t nbytes;
  ssize_t msgsize;
  
#ifdef CONFIG_NET_CAN
  txmsg.can_id = ID;
  txmsg.can_dlc = len;

  memcpy(txmsg.data, DATA, len);
#else
  txmsg.cm_hdr.ch_id     = ID;
  txmsg.cm_hdr.ch_rtr    = false;
  txmsg.cm_hdr.ch_dlc    = len;
#ifdef CONFIG_CAN_ERRORS
  txmsg.cm_hdr.ch_error  = 0;
#endif
#ifdef CONFIG_CAN_EXTID
  txmsg.cm_hdr.ch_extid  = extended;
#endif
  /* txmsg.cm_hdr.ch_unused = 0; */
  
  memcpy(txmsg.cm_data, DATA, len);
#endif

    /* Send the TX message */
#ifdef CONFIG_NET_CAN
    nbytes = write(canFD, &txmsg, sizeof(txmsg));
    if (nbytes != sizeof(txmsg)) {
      printf("ERROR: write(%ld) returned %ld\n",
	     (long)sizeof(txmsg), (long)nbytes);
    }
#else
    msgsize = CAN_MSGLEN(len);
    nbytes = write(canFD, &txmsg, msgsize);
    if (nbytes != msgsize) {
      printf("ERROR: write(%ld) returned %ld\n",
	     (long)msgsize, (long)nbytes);
    }
#endif
}

#ifdef CONFIG_CAN
int rcvMsgCob(int cob, uint8_t DATA[], int timeout)
{
  struct can_msg_s rxmsg;
  ssize_t nbytes;
  size_t msgsize;

  msgsize = sizeof(struct can_msg_s);
  do{
  nbytes = read(canFD, &rxmsg, msgsize);
  }while(rxmsg.cm_hdr.ch_id != cob);
 
  nbytes = read(canFD, &rxmsg, msgsize);
  if (nbytes < CAN_MSGLEN(0) || nbytes > msgsize){
    printf("ERROR: read(%ld) returned %ld\n",
	   (long)msgsize, (long)nbytes);
  }
  
  if(rxmsg.cm_hdr.ch_dlc != 0) memcpy(DATA, rxmsg.cm_data, rxmsg.cm_hdr.ch_dlc);
  return rxmsg.cm_hdr.ch_dlc;
}
  
int rcvMsg(uint8_t DATA[], int timeout)
{
  struct can_msg_s rxmsg;
  ssize_t nbytes;
  size_t msgsize;

  msgsize = sizeof(struct can_msg_s);
  nbytes = read(canFD, &rxmsg, msgsize);

  if (nbytes < CAN_MSGLEN(0) || nbytes > msgsize){
    printf("ERROR: read(%ld) returned %ld\n",
	   (long)msgsize, (long)nbytes);
  }

  if(rxmsg.cm_hdr.ch_dlc != 0) memcpy(DATA, rxmsg.cm_data, rxmsg.cm_hdr.ch_dlc);
  return rxmsg.cm_hdr.ch_dlc;
}
#endif

/****************************************************************************
 * Name: rcv
 *
 * Description:
 *   Schedule message receiving thread as real time task
 *
 ****************************************************************************/

void *rcv(void *args)
{
  /* Receiving thread scheduled as RT task */
#if defined(CONFIG_NET_CAN)
  struct can_frame rxmsg;
#else
  struct can_msg_s rxmsg;
#endif
  ssize_t nbytes;
  size_t msgsize;

  struct sched_param param;

  param.sched_priority = (int) args;
  if(sched_setscheduler(0, SCHED_FIFO, &param)==-1){
    perror("sched_setscheduler failed");
    exit(-1);
  }

#ifdef HAVE_MLOCK
  mlockall(MCL_CURRENT | MCL_FUTURE);
#endif
  
#if defined(CONFIG_NET_CAN)
  msgsize = sizeof(rxmsg);
#else  
  msgsize = sizeof(struct can_msg_s);
#endif
  
  while (!endrcv)         /* receiving loop */
  {
    nbytes = read(canFD, &rxmsg, msgsize);

    /* Store messages  */
#if defined(CONFIG_NET_CAN)
    socketCAN_saveMsg(rxmsg);
#else
    if(rxmsg.cm_data[0] != 0x01) CAN_saveMsg(rxmsg);
    else                              saveMsg2(rxmsg);
#endif
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

/****************************************************************************
 * Name: CAN_open
 *
 * Description:
 *   Open CAN characteristic device
 *
 ****************************************************************************/

int CAN_open(char * dev)
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

/****************************************************************************
 * Name: socketCAN_open
 *
 * Description:
 *   Open Socket CAN device
 *
 ****************************************************************************/

#if defined(CONFIG_NET_CAN)
int socketCAN_open(char * dev)
{

  int priority = 90;

  if(!dev_cnt)					/* This task is performed only one time */
  {
    struct sockaddr_can addr;
    struct ifreq ifr;
    int addrlen = sizeof(addr);

	  /* Create CAN Socket */

    canFD = socket(AF_CAN, SOCK_RAW, CAN_RAW);
	  if (canFD < 0)
	  {
	    printf("ERROR: failed to create CAN socket");
	    return -1;
	  }

	  strncpy(ifr.ifr_name, dev, IFNAMSIZ - 1);
	  ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	  ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
	  if (!ifr.ifr_ifindex)
      {
	      printf("ERROR: if_nametoindex");
	      return -1;
	    }

	  memset(&addr, 0, sizeof(addr));
	  addr.can_family = AF_CAN;
	  addr.can_ifindex = ifr.ifr_ifindex;

	  /* Bind CAN Socket */

	  if (bind(canFD, (const struct sockaddr *)&addr, addrlen) < 0)
      {
	      printf("ERROR: binding socket failed");
	      close(canFD);
	      return -1;
	    }
	  pthread_create(&rt_rcv, NULL, rcv, (void *) priority);
    dev_cnt += 1;
	  return 0;
  }
  else
  {
    printf("Already initalized\n");
    return -1;
  }
}
#endif

/****************************************************************************
 * Name: canOpenTH
 *
 * Description:
 *   Calls functions to open CAN device
 *
 ****************************************************************************/

int canOpenTH(char * dev)
{
#if defined(CONFIG_NET_CAN)
  return socketCAN_open(dev);   /* SocketCAN interface is defined in NuttX */
#elif defined(CONFIG_CAN)
  return CAN_open(dev);         /* Low lever CAN driver is defined in NuttX */
#else
  printf("ERROR: CAN driver is not defined\n");
  return -1;
#endif
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
