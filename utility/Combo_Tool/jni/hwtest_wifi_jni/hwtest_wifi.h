#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>

#define INIT_CHIP 0x00
#define MT662x    0x01
#define MT76xx    0x02

#define MT662x_WLAN_INTERFACE "wlan0"
#define MT76xx_WLAN_INTERFACE "wlan0"

/***************************************************************************
 * 
 * For MT5931¡¢MT6620¡¢MT6628 WiFi Rx/Tx hwtest
 * 
 * *************************************************************************/
typedef enum {
	WIFI_TEST_PREAMBLE_LONG = 0,
	WIFI_TEST_PREAMBLE_SHORT,
} PreambleType_t;

/* WIFI_TEST_TxDataRate */
#define WIFI_TEST_RATE_AUTO         0
#define WIFI_TEST_RATE_1MBPS		1
#define WIFI_TEST_RATE_2MBPS		2
#define WIFI_TEST_RATE_5_5MBPS		3
#define WIFI_TEST_RATE_6MBPS		4
#define WIFI_TEST_RATE_9MBPS		5
#define WIFI_TEST_RATE_11MBPS		6
#define WIFI_TEST_RATE_12MBPS		7
#define WIFI_TEST_RATE_18MBPS		8
#define WIFI_TEST_RATE_24MBPS		9
#define WIFI_TEST_RATE_36MBPS		10
#define WIFI_TEST_RATE_48MBPS		11
#define WIFI_TEST_RATE_54MBPS		12

/* Supported MCS rates */
typedef enum _tagENUM_WIFI_TEST_MCS_RATE {
	WIFI_TEST_MCS_RATE_0 = 0,
	WIFI_TEST_MCS_RATE_1 = 1,
	WIFI_TEST_MCS_RATE_2 = 2,
	WIFI_TEST_MCS_RATE_3 = 3,
	WIFI_TEST_MCS_RATE_4 = 4,
	WIFI_TEST_MCS_RATE_5 = 5,
	WIFI_TEST_MCS_RATE_6 = 6,
	WIFI_TEST_MCS_RATE_7 = 7,
	WIFI_TEST_MCS_RATE_32= 32,
} ENUM_WIFI_TEST_MCS_RATE, *P_ENUM_WIFI_TEST_MCS_RATE;

/* bandwidth */
#define WIFI_TEST_BW_20  0
#define WIFI_TEST_BW_40  1
#define WIFI_TEST_BW_20U 2
#define WIFI_TEST_BW_20L 3

/* Preamble Type */
typedef enum _tagENUM_WIFI_TEST_PREAMBLE_TYPE {
    WIFI_TEST_PREAMBLE_TYPE_MIXED_MODE = 0,
    WIFI_TEST_PREAMBLE_TYPE_GREENFIELD = 1
} ENUM_WIFI_TEST_PREAMBLE_TYPE, *P_ENUM_WIFI_TEST_PREAMBLE_TYPE;

/* Guard Interval Type */
typedef enum _tagENUM_WIFI_TEST_GI_TYPE {
    WIFI_TEST_GI_TYPE_NORMAL_GI = 0,
    WIFI_TEST_GI_TYPE_SHORT_GI  = 1
} ENUM_WIFI_TEST_GI_TYPE, *P_ENUM_WIFI_TEST_GI_TYPE;


/* function provided by libwifitest.so*/
bool (*WIFI_TEST_OpenDUT)(void);
bool (*WIFI_TEST_CloseDUT)(void);
bool (*WIFI_TEST_TxDataRate)(int TxDataRate);
bool (*WIFI_TEST_SetPreamble)(PreambleType_t PreambleType);
bool (*WIFI_TEST_Channel)(int ChannelNo);
bool (*WIFI_TEST_SetBandWidth)(int bandwidth);
bool (*WIFI_TEST_TxGain)(int TxGain);
bool (*WIFI_TEST_TxBurstInterval)(int SIFS);
bool (*WIFI_TEST_TxPayloadLength)(int TxPayLength);
bool (*WIFI_TEST_TxBurstFrames)(int Frames);
bool (*WIFI_TEST_TxDestAddress)(unsigned char *addr);
bool (*WIFI_TEST_TxStart)(void);
bool (*WIFI_TEST_TxStop)(void);
bool (*WIFI_TEST_RxStart)(void);
bool (*WIFI_TEST_RxStop)(void);
bool (*WIFI_TEST_FRError)(int *FError);
bool (*WIFI_TEST_FRGood)(int *FRGood);
bool (*WIFI_TEST_RSSI)(signed char *RSSI);
bool (*WIFI_TEST_IsRunning)(void);
bool (*WIFI_TEST_IsUp)(void);
bool (*WIFI_TEST_TxDataRate11n)(ENUM_WIFI_TEST_MCS_RATE eDataRate11n, ENUM_WIFI_TEST_PREAMBLE_TYPE ePreambleType, ENUM_WIFI_TEST_GI_TYPE eGIType);
bool (*WIFI_TEST_FrequencyAccuracy)(int ChannelNo);
bool (*WIFI_TEST_FrequencyAccuracy_Stop)();
bool (*WIFI_TEST_MCR_Read)(unsigned int addr, unsigned int *val);
bool (*WIFI_TEST_MCR_Write)(unsigned int addr, unsigned int val);
bool (*WIFI_TEST_EFUSE_Read)(unsigned int offset, unsigned int *val);
bool (*WIFI_TEST_EFUSE_Write)(unsigned int offset, unsigned int val);
bool (*WIFI_TEST_CW_MODE)(int mode);

/*added  by mtk54425 for combotool.apk*/
bool (*WIFI_TEST_Tx_Output_Power)(void);
bool (*WIFI_TEST_Local_Leakage)(void);
bool (*WIFI_TEST_Carrier_Suppression)(void);

/***************************************************************************
 * 
 * For MT7601¡¢MT5370 WiFi Rx/Tx hwtest
 * 
 * *************************************************************************/
#define ATE_KEYWORD_LENGTH      32
#define ATE_VALUE_LENGTH		32
#define ATE_COMMAND_LENGTH		126
#define IWPRIV_CMD_RX_COUNT 	7
#define IWPRIV_CMD_TX_COUNT 	16

#define SUCCESS                 0
#define ERROR                   -1
#define TRUE 					1
#define FALSE                   0

typedef enum t_ATE_CMD_INDEX
{
	ATE = 0,
	ATESA,
	ATEDA,
	ATEBSSID,
	ATECHANNEL,
	ATETXMODE,
	ATETXMSC,
	ATETXBW,
	ATETXGI,
	ATETXLEN,
	ATETXFREQOFFSET,
	ATETXANT,
	ATETXPOW0,
	ATETXPOW1,
	ATETXPOW2,
	ATETXCNT,
	ATEMODETYPE,
	ATECOUNTRESET,
	ATE_CMD_ENUM_MAX
}ATE_CMD_INDEX;

typedef struct t_ATE_CMD
{
	ATE_CMD_INDEX iIndex;
	char szKeyword[ATE_KEYWORD_LENGTH];
	char szValue[ATE_VALUE_LENGTH];
}ATE_CMD_st;


/******************************************
 * 
 * 
 * ****************************************/