/*****************************************************************************
 * Copyright (C) 2001-2007 PEAK System-Technik GmbH
 *
 * linux@peak-system.com
 * www.peak-system.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Maintainer(s): Klaus Hitschler (klaus.hitschler@gmx.de)
 * Contributions: Mudiaga Obada   (obada@vsi.cs.uni-frankfurt.de)
 *                Jörg Diederich  (joerg.diederich@graffiti.net)
 *****************************************************************************/

/*****************************************************************************
 *
 * libpcan.c
 * the shared library to unify the interface to the devices
 * PCAN-ISA, PCAN-Dongle, PCAN-PCI, PCAN-PC104, PCAN-USB via their drivers
 *
 * $Id$
 *
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <libpcan.h>

#include <libprivate.h>

#define PROCFILE		"/proc/pcan"
#define DEVICE_PATH		"/dev/pcan"
#define MAX_LINE_LEN		255

static int resolve(char *buffer, int *nType, unsigned long *dwPort,
			 unsigned short *wIrq, int *nMajor, int *nMinor)
{
	static int m_nMajor = 0;
	char *ptr = buffer;
	char *t;

	if (*ptr == '\n')
		return -1;

	if (*ptr == '*') {
		/* search for nMajor */
		if ((ptr = strstr(ptr, "major"))) {
			t = strtok(ptr, " ");
			t = strtok(NULL, " ");
			m_nMajor = strtoul(t, NULL, 10);
		}
	} else {
		/* skip leading blank */
		if (*ptr == ' ')
			ptr++;

		/* get minor */
		t = strtok(ptr, " ");
		*nMinor = strtoul(ptr, NULL, 10);

		/* get type string */
		t = strtok(NULL, " ");
		if (!strncmp(t, "pci", 3))
			*nType = HW_PCI;
		else if (!strcmp(t, "pcifd"))
			*nType = HW_PCIE_FD;
		else if (!strcmp(t, "epp"))
			*nType = HW_DONGLE_SJA_EPP;
		else if (!strcmp(t, "isa"))
			*nType = HW_ISA_SJA;
		else if (!strcmp(t, "sp"))
			*nType = HW_DONGLE_SJA;
		else if (!strcmp(t, "usb"))
			*nType = HW_USB;
		else if (!strcmp(t, "usbfd"))
			*nType = HW_USB_FD;
		else
			*nType = -1;

		/* jump over ndev */
		while (*t++ == ' ');
		t = strtok(NULL, " ");

		/* get port */
		t = strtok(NULL, " ");
		*dwPort = strtoul(t, NULL, 16);

		/* get irq */
		t = strtok(NULL, " ");
		*wIrq = (unsigned short)strtoul(t, NULL, 10);

		/* set major */
		*nMajor = m_nMajor;

		return 0;
	}

	return -1;
}

/* do a linux like open of the device */
HANDLE LINUX_CAN_Open(const char *szDeviceName, int nFlag)
{
#if !defined(NO_RT) && !defined(__COBALT__)
	char DeviceName[15];
#endif
	struct pcan_handle *desc = NULL;

	errno = 0;

	desc = (struct pcan_handle *)malloc(sizeof(*desc));
	if (!desc)
		goto fail_1;

	desc->szVersionString[0] = 0;
	desc->szDevicePath[0] = 0;

#if !defined(NO_RT) && !defined(__COBALT__)
	sscanf(szDeviceName, "/dev/%s", DeviceName);
	desc->fd = rt_dev_open(DeviceName, nFlag);
#else
	desc->fd = open(szDeviceName, nFlag);
#endif
	if (desc->fd < 0)
		goto fail_2;

	strncpy(desc->szDevicePath, szDeviceName, LOCAL_STRING_LEN);

	return (HANDLE)desc;

fail_2:
#ifndef NO_RT
	errno = -desc->fd;
#endif
	free(desc);

fail_1:
	return NULL;
}

/* do a PEAK-System like open of the device */
HANDLE CAN_Open(WORD wHardwareType, ...)
{
	HANDLE handle = NULL;
	FILE *f = NULL;
	char *p = NULL;
	char *ptr;
	int found = 0;

	va_list args;
	unsigned long m_dwPort = 0;
	unsigned short m_wIrq = 0;

	int nMinor = 0;
	int nMajor = 0;
	unsigned long dwPort;
	unsigned short wIrq;
	int nType;

	char m[MAX_LINE_LEN];

	errno = ENODEV;

	/* read variable length and type argument list */
	va_start(args, wHardwareType);

	/* map compatible device types */
	switch (wHardwareType) {
	case HW_USB_PRO:
		wHardwareType = HW_USB;
		break;
	case HW_USB_PRO_FD:
	case HW_USB_X6:
		wHardwareType = HW_USB_FD;
		break;
	}

	switch (wHardwareType) {
	case HW_DONGLE_SJA:
	case HW_DONGLE_SJA_EPP:
	case HW_ISA_SJA:
		m_dwPort = va_arg(args, unsigned long);
		m_wIrq = (unsigned short)va_arg(args, unsigned long);
		va_end(args);
		break;

	case HW_PCI:
	case HW_PCIE_FD:
		m_dwPort = va_arg(args, int);
		va_end(args);
		break;

	case HW_USB:
	case HW_USB_FD:
		m_dwPort = va_arg(args, unsigned long);
		m_wIrq = (unsigned short)va_arg(args, unsigned long);
		va_end(args);
		break;

	default:
		va_end(args);
		goto fail;
	}

	f = fopen(PROCFILE, "r");
	if (!f)
		goto fail;

	/* read and parse proc entry contents */
	do {
		ptr = m;
		p = fgets(ptr, MAX_LINE_LEN, f);
		if (!p)
			break;

		if (resolve(p, &nType, &dwPort, &wIrq, &nMajor, &nMinor))
			continue;

		if (wHardwareType != nType)
			continue;

		switch (wHardwareType) {
		case HW_DONGLE_SJA:
		case HW_DONGLE_SJA_EPP:
		case HW_ISA_SJA:
			found = (((m_dwPort == dwPort) && (m_wIrq == wIrq)) ||
				((!m_dwPort) && (!m_wIrq)));
			break;

		case HW_PCI:
		case HW_PCIE_FD:
			/* enumerate 1..8 (not 0 .. 7) */
			found = (((m_dwPort - 1) == nMinor) || (!m_dwPort));
			break;

		case HW_USB:
		case HW_USB_FD:

			/* select the USB device if:
			 * - m_wIrq is not 0 and matches the USB device
			 *   number
			 * - m_dwPort matches the device minor
			 * - the 1st usb deviec found otherwise (no irq
			 *   nor port given) */
			if (m_wIrq)
				found = (m_wIrq == wIrq);

			/* enumerate 1..8 (not 32 .. 39) */
			else if (m_dwPort)
				found = ((m_dwPort + 31) == nMinor);
			else
				found = 1;
			break;
		}
	} while ((p) && (!found));

	fclose(f);

	if (found) {
		if (nMinor <= 64) {
			sprintf(m, "%s%d", DEVICE_PATH, nMinor);
			handle = LINUX_CAN_Open(m, O_RDWR);
		}
	}

fail:
	return handle;
}

/* init the CAN chip of this device */
DWORD CAN_Init(HANDLE hHandle, WORD wBTR0BTR1, int nCANMsgType)
{
	struct pcan_handle *desc = (struct pcan_handle *)hHandle;
	TPCANInit init = {
		.ucListenOnly = 0,
		.wBTR0BTR1 = wBTR0BTR1,
		.ucCANMsgType = (nCANMsgType) ?
					MSGTYPE_EXTENDED : MSGTYPE_STANDARD,
	};

	if (!desc) {
		errno = EBADF;
		return errno;
	}

	return __errno_ioctl(desc->fd, PCAN_INIT, &init);
}

DWORD CAN_Close(HANDLE hHandle)
{
	struct pcan_handle *desc = (struct pcan_handle *)hHandle;

	if (desc) {
		if (desc->fd > -1) {
			__close(desc->fd);
			desc->fd = -1;
		}
		free(desc);
	}
	return 0;
}

DWORD CAN_Status(HANDLE hHandle)
{
	struct pcan_handle *desc = (struct pcan_handle *)hHandle;
	TPSTATUS status;
	DWORD err;

	if (!desc) {
		errno = EBADF;
		return errno;
	}

	err = __errno_ioctl(desc->fd, PCAN_GET_STATUS, &status);
	if (!err)
		return status.wErrorFlag;

	return err;
}

DWORD CAN_Write(HANDLE hHandle, TPCANMsg *pMsgBuff)
{
	struct pcan_handle *desc = (struct pcan_handle *)hHandle;

	if (!desc) {
		errno = EBADF;
		return errno;
	}

	return __errno_ioctl(desc->fd, PCAN_WRITE_MSG, pMsgBuff);
}

DWORD LINUX_CAN_Read(HANDLE hHandle, TPCANRdMsg *pMsgBuff)
{
	struct pcan_handle *desc = (struct pcan_handle *)hHandle;

	if (!desc) {
		errno = EBADF;
		return errno;
	}

	return __errno_ioctl(desc->fd, PCAN_READ_MSG, pMsgBuff);
}

DWORD CAN_Read(HANDLE hHandle, TPCANMsg *pMsgBuff)
{
	TPCANRdMsg m;
	DWORD err;

	if (!pMsgBuff) {
		errno = EINVAL;
		return errno;
	}

	err = LINUX_CAN_Read(hHandle, &m);
	if (!err)
		memcpy(pMsgBuff, &m.Msg, sizeof(*pMsgBuff));

	return err;
}

int LINUX_CAN_FileHandle(HANDLE hHandle)
{
	struct pcan_handle *desc = (struct pcan_handle *)hHandle;

	return (desc) ? desc->fd : -EBADF;
}

DWORD LINUX_CAN_Statistics(HANDLE hHandle, TPDIAG *diag)
{
	struct pcan_handle *desc = (struct pcan_handle *)hHandle;

	if (!desc) {
		errno = EBADF;
		return errno;
	}

	return __errno_ioctl(desc->fd, PCAN_DIAG, diag);
}

WORD LINUX_CAN_BTR0BTR1(HANDLE hHandle, DWORD dwBitRate)
{
	struct pcan_handle *desc = (struct pcan_handle *)hHandle;
	TPBTR0BTR1 ratix = {
		.dwBitRate = dwBitRate,
		.wBTR0BTR1 = 0,
	};

	if (!desc)
		errno = EBADF;

	else if (!__ioctl(desc->fd, PCAN_BTR0BTR1, &ratix))
		return ratix.wBTR0BTR1;

	return 0;
}

DWORD CAN_VersionInfo(HANDLE hHandle, LPSTR szTextBuff)
{
	TPDIAG diag;
	DWORD err;

	if (!szTextBuff) {
		errno = EINVAL;
		return errno;
	}

	*szTextBuff = 0;

	err = LINUX_CAN_Statistics(hHandle, &diag);
	if (!err)
		strncpy(szTextBuff, diag.szVersionString, VERSIONSTRING_LEN);

	return err;
}

DWORD LINUX_CAN_Read_Timeout(HANDLE hHandle, TPCANRdMsg* pMsgBuff,
			     int nMicroSeconds)
{
	struct pcan_handle *desc = (struct pcan_handle *)hHandle;
	struct timeval t;
	fd_set fdRead;
	int err;

	if (nMicroSeconds < 0)
		return LINUX_CAN_Read(hHandle, pMsgBuff);

	if (!desc) {
		errno = EBADF;
		return errno;
	}

	/* calculate timeout values */
	t.tv_sec  = nMicroSeconds / 1000000L;
	t.tv_usec = nMicroSeconds % 1000000L;

	FD_ZERO(&fdRead);
	FD_SET(desc->fd, &fdRead);

	/* wait until timeout or a message is ready to read */
	err = select(desc->fd + 1, &fdRead, NULL, NULL, &t);

	/* the only one file descriptor is ready for read */
	if (err > 0)
		return LINUX_CAN_Read(hHandle, pMsgBuff);

	/* nothing is ready, timeout occured */
	if (!err)
		return CAN_ERR_QRCVEMPTY;

	/* anything else is an error */
	return errno;
}

DWORD LINUX_CAN_Write_Timeout(HANDLE hHandle, TPCANMsg* pMsgBuff,
			      int nMicroSeconds)
{
	struct pcan_handle *desc = (struct pcan_handle *)hHandle;
	struct timeval t;
	fd_set fdWrite;
	int err;

	if (nMicroSeconds < 0)
		return CAN_Write(hHandle, pMsgBuff);

	if (!desc) {
		errno = EBADF;
		return errno;
	}

	/* calculate timeout values */
	t.tv_sec  = nMicroSeconds / 1000000L;
	t.tv_usec = nMicroSeconds % 1000000L;

	FD_ZERO(&fdWrite);
	FD_SET(desc->fd, &fdWrite);

	/* wait until timeout or a message is ready to get written */
	err = select(desc->fd + 1,  NULL, &fdWrite, NULL, &t);

	/* the only one file descriptor is ready for write */
	if (err > 0)
		return CAN_Write(hHandle, pMsgBuff);

	/* nothing is free, timeout occured */
	if (!err)
		return CAN_ERR_QXMTFULL;

	/* anything else is an error */
	return errno;
}

DWORD LINUX_CAN_Extended_Status(HANDLE hHandle, int *nPendingReads,
				int *nPendingWrites)
{
	struct pcan_handle *desc = (struct pcan_handle *)hHandle;
	TPEXTENDEDSTATUS status;
	DWORD err;

	if (!desc) {
		errno = EBADF;
		return errno;
	}

	err = __errno_ioctl(desc->fd, PCAN_GET_EXT_STATUS, &status);
	if (!err) {
		if (nPendingReads)
			*nPendingReads  = status.nPendingReads;
		if (nPendingWrites)
			*nPendingWrites = status.nPendingWrites;

		return status.wErrorFlag;
	}

	return err;
}

int nGetLastError(void)
{
	return errno;
}

DWORD CAN_MsgFilter(HANDLE hHandle, DWORD FromID, DWORD ToID, int nCANMsgType)
{
	struct pcan_handle *desc = (struct pcan_handle *)hHandle;
	TPMSGFILTER filter = {
		.FromID  = FromID,
		.ToID = ToID,
		.MSGTYPE = (BYTE )nCANMsgType,
	};

	if (!desc) {
		errno = EBADF;
		return errno;
	}

	return __errno_ioctl(desc->fd, PCAN_MSG_FILTER, &filter);
}

/* for compatibility to MS-Windows only */
DWORD CAN_ResetFilter(HANDLE hHandle)
{
	struct pcan_handle *desc = (struct pcan_handle *)hHandle;

	if (!desc) {
		errno = EBADF;
		return errno;
	}

	return __errno_ioctl(desc->fd, PCAN_MSG_FILTER, NULL);
}
