#include <stdint.h>

union canDATA{
  uint8_t u8[8];
  uint16_t u16[4];
  uint32_t u32[2];
};

typedef union canDATA cdata;

void sendMsg(uint16_t ID, uint8_t DATA[], int len);
int rcvMsg(uint8_t DATA[], int timeout);
int rcvMsgCob(int cob, uint8_t DATA[], int timeout);
int canOpen(char * dev);
int canOpenTH(char * dev);
void canClose(void);
void canopen_synch(void);
int registerMsg(uint16_t ID, uint16_t index, uint8_t subindex);
int getValue(uint16_t  ID, uint16_t index, uint8_t subindex);
short get2ByteValue(uint16_t ID, uint16_t index, uint8_t subindex);
