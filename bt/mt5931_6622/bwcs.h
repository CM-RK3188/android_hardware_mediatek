#ifndef BWCS_H
#define BWCS_H

/* header files for bwcs */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "bt_hci_bdroid.h"
#include "bt_vendor_lib.h"

#define HCI_CMD_MAX_SIZE        251


//#include "bt_sock.h"

#ifdef  BWCS_LOG_TAG
#undef  BWCS_LOG_TAG
#endif
#define BWCS_LOG_TAG         "[BWCS_BT]"
#include <cutils/log.h> 

#define BWCS_DEBUG     1
#define BWCS_ERR(f, ...)	ALOGE("%s: " f, __func__, ##__VA_ARGS__)
#define BWCS_WRN(f, ...)	ALOGW("%s: " f, __func__, ##__VA_ARGS__)

#if BWCS_DEBUG
#define BWCS_DBG(f, ...)	ALOGD("%s: " f, __func__, ##__VA_ARGS__)	
#define BWCS_TRC(f)		ALOGW("%s #%d", __func__, __LINE__)
#else
#define BWCS_DBG(...)        ((void)0)
#define BWCS_TRC(f)          ((void)0)
#endif

#define BWCS_DOMAIN	"/data/bwcs/bwcs"
#define MT6622_DOMAIN	"/data/bwcs/mt6622"
//unlink things should add into the socket

/* data types definition */
typedef unsigned long	DWORD;
typedef unsigned short	USHORT;
typedef unsigned char	UCHAR;
typedef unsigned char	BYTE;
typedef unsigned char	BOOL;
typedef unsigned char	BOOLEAN;

#define TRUE      1
#define FALSE     0 

/* bt & bwcs interact payload */
typedef struct
{
    USHORT	event;  //for event from mt6622 to bwcs
    USHORT	action; //for action from bwcs to mt6622
    UCHAR	para[8];//parameters
} BWCS_BT;

/* bt events to bwcs */
#define BT_EVENT_BT_OPEN	0x0001
#define BT_EVENT_BT_CLOSE	0x0002
#define BT_EVENT_BT_A2DP	0x0004
#define BT_EVENT_BT_HSP		0x0008

/* bt actions from bwcs */
#define BT_ACTION_SCATTER_INQUIRY	0x0010
#define BT_ACTION_WIFI_CHL		0x0020
#define BT_ACTION_POWER_CTRL		0x0040

/*
#define BT_EVENT_WIFI_OPEN	0x0010
#define BT_EVENT_WIFI_CLOSE	0x0020
#define BT_EVENT_WIFI_CHL	0x0040
*/

void bwcs_bt_init(const bt_vendor_callbacks_t* p_cb);
void bwcs_bt_cleanup(void);
void bwcs_action_notify(USHORT action, BWCS_BT bb);
void bt_status_event( USHORT event);



/* wifi related definitions */
/* WiFi Driver events to bwcs */
#define WIFI_EVENT_WIFI_OPEN		(DWORD)(1<<0)
#define WIFI_EVENT_WIFI_CLOSE		(DWORD)(1<<1)
#define WIFI_EVENT_CONN_NEW		(DWORD)(1<<2)
#define WIFI_EVENT_CONN_DEL		(DWORD)(1<<3)
#define WIFI_EVENT_STA_CONN_NEW		(DWORD)(1<<4)
#define WIFI_EVENT_STA_CONN_DEL		(DWORD)(1<<5)
#define WIFI_EVENT_P2P_GO_CONN_NEW	(DWORD)(1<<6)
#define WIFI_EVENT_P2P_GO_CONN_DEL	(DWORD)(1<<7)
#define WIFI_EVENT_P2P_GC_CONN_NEW	(DWORD)(1<<8)
#define WIFI_EVENT_P2P_GC_CONN_DEL	(DWORD)(1<<9)
#define WIFI_EVENT_SOFTAP_CONN_NEW	(DWORD)(1<<10)
#define WIFI_EVENT_SOFTAP_CONN_DEL	(DWORD)(1<<11)

/* WiFi events from bwcs */
#define WIFI_ACTION_CUSTOM_1		0x1001
#define WIFI_ACTION_CUSTOM_2		0x1002
#define WIFI_ACTION_CUSTOM_3		0x1003
#define WIFI_ACTION_CUSTOM_4		0x1004
enum {
	WIFI_CONTYPE_STA = 0,
	WIFI_CONTYPE_P2P,
	WIFI_CONTYPE_SOFTAP,
	WIFI_CONTYPE_MAX
};

enum {
	WIFI_EVENT_PARAM_CH = 0,
	WIFI_EVENT_PARAM_BW,
	WIFI_EVENT_PARAM_FREQ,	/*no used*/
	WIFI_EVENT_PARAM_MAX
};

/* BW */
/* The Bandwidth selection of WIFI_EVENT_PARAM_BW*/
#define WIFI_BW_20		0
#define WIFI_BW_40		1
#define WIFI_BW_80		2
#define WIFI_BW_10		4	/* 802.11j has 10MHz. This definition is for internal usage. doesn't fill in the IE or other field. */


#define BWCS_NAME_MAX_LEN			32
#define BWCS_NAME					"BWCS"

typedef struct __BWCS_WIFI {
    char	name[BWCS_NAME_MAX_LEN];		/*BWCS*/
    USHORT	event;
    UCHAR	para[WIFI_EVENT_PARAM_MAX];
} BWCS_WIFI;

#endif
