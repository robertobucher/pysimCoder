//****************************************************************************
// Copyright (C) 2001-2007  PEAK System-Technik GmbH
//
// linux@peak-system.com
// www.peak-system.com
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Maintainer(s): Klaus Hitschler (klaus.hitschler@gmx.de)
// Contributions: Mudiaga Obada   (obada@vsi.cs.uni-frankfurt.de)
//****************************************************************************

//****************************************************************************
//
// libpcan.h
// common header to access the functions within pcanlib.so.x.x,
// originally created from Wilhelm Hoppe in pcan_pci.h
//
// $Id$
//
//****************************************************************************

#ifndef __LIBPCAN_H__
#define __LIBPCAN_H__

#include <pcan.h>

/* compatibilty defines */
#if defined(LPSTR) || defined(HANDLE)
#error "double define for LPSTR, HANDLE found"
#endif

#define LPSTR		char *
#define HANDLE		void *

//****************************************************************************
// for CAN_Open(...)

//****************************************************************************
// for CAN_Init(...)

// parameter wBTR0BTR1
// bitrate codes of BTR0/BTR1 registers
#define CAN_BAUD_1M     0x0014  //   1 MBit/s
#define CAN_BAUD_500K   0x001C  // 500 kBit/s
#define CAN_BAUD_250K   0x011C  // 250 kBit/s
#define CAN_BAUD_125K   0x031C  // 125 kBit/s
#define CAN_BAUD_100K   0x432F  // 100 kBit/s
#define CAN_BAUD_50K    0x472F  //  50 kBit/s
#define CAN_BAUD_20K    0x532F  //  20 kBit/s
#define CAN_BAUD_10K    0x672F  //  10 kBit/s
#define CAN_BAUD_5K     0x7F7F  //   5 kBit/s

// parameter nCANMsgType
#define CAN_INIT_TYPE_ST		0x00	//Standart Frame
#define CAN_INIT_TYPE_EX		0x02	//Extended Frame

//****************************************************************************
// error codes are defined in pcan.h
#define CAN_ERR_ANYBUSERR (CAN_ERR_BUSLIGHT | CAN_ERR_BUSHEAVY | CAN_ERR_BUSOFF)

//****************************************************************************
// PROTOTYPES
#ifdef __cplusplus
  extern "C"
{
#endif

//****************************************************************************
//  CAN_Open()
//  creates a path to a CAN port
//
//  for PCAN-Dongle call:             CAN_Open(HW_DONGLE_.., DWORD dwPort, WORD wIrq);
//  for PCAN-ISA or PCAN-PC/104 call: CAN_Open(HW_ISA_SJA, DWORD dwPort, WORD wIrq);
//  for PCAN-PCI call:                CAN_Open(HW_PCI, int nPort); .. enumerate nPort 1..8.
//
//  if ((dwPort == 0) && (wIrq == 0)) CAN_Open() takes the 1st default ISA or DONGLE port.
//  if (nPort == 0) CAN_Open() takes the 1st default PCI port.
//  returns NULL when open failes.
//
//  The first CAN_Open() to a CAN hardware initializes the hardware to default
//  parameter 500 kbit/sec and acceptance of extended frames.
//
HANDLE CAN_Open(WORD wHardwareType, ...);

//****************************************************************************
//  CAN_Init()
//  initializes the CAN hardware with the BTR0 + BTR1 constant "CAN_BAUD_...".
//  nCANMsgType must be filled with "CAN_INIT_TYPE_..".
//  The default initialisation, e.g. CAN_Init is not called,
//  is 500 kbit/sec and extended frames.
//
DWORD CAN_Init(HANDLE hHandle, WORD wBTR0BTR1, int nCANMsgType);

//****************************************************************************
//  CAN_Close()
//  closes the path to the CAN hardware.
//  The last close on the hardware put the chip into passive state.
DWORD CAN_Close(HANDLE hHandle);

//****************************************************************************
//  CAN_Status()
//  request the current (stored) status of the CAN hardware. After the read the
//  stored status is reset.
//  If the status is negative a system error is returned (e.g. -EBADF).
DWORD CAN_Status(HANDLE hHandle);

//****************************************************************************
//  CAN_Write()
//  writes a message to the CAN bus. If the write queue is full the current
//  write blocks until either a message is sent or a error occured.
//
DWORD CAN_Write(HANDLE hHandle, TPCANMsg* pMsgBuff);

//****************************************************************************
//  LINUX_CAN_Write_Timeout()
//  writes a message to the CAN bus. If the (software) message buffer is full
//  the current write request blocks until a write slot gets empty 
//  or a timeout or a error occures.
//  nMicroSeconds  > 0 -> Timeout in microseconds
//  nMicroSeconds == 0 -> polling
//  nMicroSeconds  < 0 -> blocking, same as CAN_Write()
DWORD LINUX_CAN_Write_Timeout(HANDLE hHandle, TPCANMsg* pMsgBuff, int nMicroSeconds);

//****************************************************************************
//  CAN_Read()
//  reads a message from the CAN bus. If there is no message to read the current
//  request blocks until either a new message arrives or a error occures.
DWORD CAN_Read(HANDLE hHandle, TPCANMsg* pMsgBuff);

//****************************************************************************
//  LINUX_CAN_Read()
//  reads a message WITH TIMESTAMP from the CAN bus. If there is no message 
//  to read the current request blocks until either a new message arrives 
//  or a error occures.
DWORD LINUX_CAN_Read(HANDLE hHandle, TPCANRdMsg* pMsgBuff);

//****************************************************************************
//  LINUX_CAN_Read_Timeout()
//  reads a message WITH TIMESTAMP from the CAN bus. If there is no message 
//  to read the current request blocks until either a new message arrives 
//  or a timeout or a error occures.
//  nMicroSeconds  > 0 -> Timeout in microseconds
//  nMicroSeconds == 0 -> polling
//  nMicroSeconds  < 0 -> blocking, same as LINUX_CAN_Read()
DWORD LINUX_CAN_Read_Timeout(HANDLE hHandle, TPCANRdMsg* pMsgBuff, int nMicroSeconds);

//***************************************************************************
// CAN_ResetFilter() - removes all current Message Filters
// Caution! Currently this operation influences all read paths
//
DWORD CAN_ResetFilter(HANDLE hHandle);

//***************************************************************************
// CAN_MsgFilter() - reduce received data in to FromID <= ID <= ToID
// Type may be MSGTYPE_STANDARD or MSGTYPE_EXTENDED
// This function can be called multiple to add more ranges.
// Caution! Currently this operation influences all read paths
//
DWORD CAN_MsgFilter(HANDLE hHandle, DWORD FromID, DWORD ToID, int nCANMsgType);

//***************************************************************************
// LINUX_CAN_FileHandle() - return PCAN driver file handle for select(2)
//
int LINUX_CAN_FileHandle(HANDLE hHandle);

//****************************************************************************
//  LINUX_CAN_Extended_Status()
//  get the same as CAN_Status() with additional informaton about pending reads or writes
//
//  There is a uncertainty of 1 message for "nPendingWrites" for a small amount 
//  of time between the messages is put into the CAN sender and the telegram is
//  successfuly sent or an error is thrown.
DWORD LINUX_CAN_Extended_Status(HANDLE hHandle, int *nPendingReads, int *nPendingWrites);

//****************************************************************************
//  CAN_VersionInfo()
//  returns a text string with driver version info.
//
DWORD CAN_VersionInfo(HANDLE hHandle, LPSTR lpszTextBuff);

//****************************************************************************
//  nGetLastError()
//  returns the last stored error (errno of the shared library). The returend
//  error is independend of any path.
//
int nGetLastError(void);

//****************************************************************************
//  LINUX_CAN_Open() - another open, LINUX like
//  creates a path to a CAN port
//
//  input: the path to the device node (e.g. /dev/pcan0)
//  returns NULL when open failes
//
HANDLE LINUX_CAN_Open(const char *szDeviceName, int nFlag);

//****************************************************************************
//  LINUX_CAN_Statistics() - get statistics about this devices
//
DWORD LINUX_CAN_Statistics(HANDLE hHandle, TPDIAG *diag);

//****************************************************************************
//  LINUX_CAN_BTR0BTR1() - get the BTR0 and BTR1 from bitrate, LINUX like
//
//  input:  the handle to the device node
//          the bitrate in bits / second, e.g. 500000 bits/sec
//
//  returns 0 if not possible
//          BTR0BTR1 for the interface
//
WORD LINUX_CAN_BTR0BTR1(HANDLE hHandle, DWORD dwBitRate);

#ifdef __cplusplus
}
#endif
#endif // __LIBPCAN_H__
