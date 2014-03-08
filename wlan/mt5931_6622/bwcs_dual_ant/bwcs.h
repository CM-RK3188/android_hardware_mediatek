#ifndef BWCS_H
#define BWCS_H

//#define UNIX_DOMAIN "/data/bwcs/bwcs.bt"
#define UNIX_DOMAIN_BWCS "/data/bwcs/bwcs"
#define UNIX_DOMAIN_BT "/data/bwcs/mt6622"


//unlink things should add into the socket
typedef unsigned long	DWORD;
typedef unsigned short	USHORT;
typedef unsigned char	UCHAR;
typedef unsigned char	BYTE;
typedef unsigned char	BOOL;
typedef unsigned char	BOOLEAN;

#define TRUE      1
#define FALSE     0 

/* bt events to bwcs */
#define BT_EVENT_ON	0x0001
#define BT_EVENT_OFF	0x0002
#define BT_EVENT_A2DP	0x0004
#define BT_EVENT_HSP	0x0008


/* bt actions */
#define BT_ACTION_SCATTER_INQUIRY	0x0010
#define BT_ACTION_WIFI_CHANNEL		0x0020
#define BT_ACTION_POWER_CTRL		0x0040

void bt_status_event( USHORT event);

typedef struct
{
    USHORT	event;
    USHORT	action;
    UCHAR	para[8];
} BWCS_BT;



/* WiFi Driver events to bwcs */
#define WIFI_EVENT_WIFI_OPEN		(DWORD)(1<<0)
#define WIFI_EVENT_WIFI_CLOSE		(DWORD)(1<<1)
#define WIFI_EVENT_CONN_NEW			(DWORD)(1<<2)
#define WIFI_EVENT_CONN_DEL			(DWORD)(1<<3)
#define WIFI_EVENT_STA_CONN_NEW		(DWORD)(1<<4)
#define WIFI_EVENT_STA_CONN_DEL		(DWORD)(1<<5)
#define WIFI_EVENT_P2P_GO_CONN_NEW	(DWORD)(1<<6)
#define WIFI_EVENT_P2P_GO_CONN_DEL	(DWORD)(1<<7)
#define WIFI_EVENT_P2P_GC_CONN_NEW	(DWORD)(1<<8)
#define WIFI_EVENT_P2P_GC_CONN_DEL	(DWORD)(1<<9)
#define WIFI_EVENT_SOFTAP_CONN_NEW	(DWORD)(1<<10)
#define WIFI_EVENT_SOFTAP_CONN_DEL	(DWORD)(1<<11)


#define WIFI_ACTION_CUSTOM_1	0x1001
#define WIFI_ACTION_CUSTOM_2	0x1002
#define WIFI_ACTION_CUSTOM_3	0x1003
#define WIFI_ACTION_CUSTOM_4	0x1004

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


#define BWCS_NAME_MAX_LEN		32
#define BWCS_NAME				"BWCS"

typedef struct __BWCS_WIFI {
	char	name[BWCS_NAME_MAX_LEN];		/*BWCS*/
    USHORT	event;
    UCHAR	para[WIFI_EVENT_PARAM_MAX];
} BWCS_WIFI;

#endif
