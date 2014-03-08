#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>

#define ATE_KEYWORD_LENGTH      32
#define ATE_VALUE_LENGTH		32
#define ATE_COMMAND_LENGTH		64
#define IWPRIV_CMD_RX_COUNT 	7
#define IWPRIV_CMD_TX_COUNT 	16

#define SUCCESS                 0
#define ERROR                   -1
#define TRUE 					1
#define FALSE                   0

#define MT76xx_WLAN_INTERFACE "ra0"

#define LOGD printf
#define LOGE printf

typedef enum t_ATE_CMD_INDEX
{
	ATE = 0,
	ATESA,
	ATEDA,
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
	ATE_CMD_ENUM_MAX
}ATE_CMD_INDEX;

typedef struct t_ATE_CMD
{
	ATE_CMD_INDEX iIndex;
	char szKeyword[ATE_KEYWORD_LENGTH];
	char szValue[ATE_VALUE_LENGTH];
}ATE_CMD_st;

ATE_CMD_st g_iwpriv_cmd_tx[] = 
{
	{ATE, "ATE", "ATESTART"},
	{ATEDA, "ATEDA", "00:11:22:33:44:55"},
	{ATECHANNEL, "ATECHANNEL", "1"},
	{ATETXMODE, "ATETXMODE", "2"},
	{ATETXMSC, "ATETXMSC", "1"},
	{ATETXBW, "ATETXBW", "1"},
	{ATETXGI, "ATETXGI", "0"},
	{ATETXLEN, "ATETXLEN", "1024"},
	{ATETXFREQOFFSET, "ATETXFREQOFFSET", "36"},
	{ATETXANT, "ATETXANT", "0"},
	{ATETXPOW0, "ATETXPOW0", "15"},
	{ATETXPOW1, "ATETXPOW1", "15"},
	{ATETXPOW2, "ATETXPOW2", "15"},
	{ATETXCNT, "ATETXCNT", "100000"},
	{ATEMODETYPE, "ATE", "TXFRAME"},
	{ATE_CMD_ENUM_MAX, "", ""},
};

ATE_CMD_st g_iwpriv_cmd_rx[] = 
{
	{ATE, "ATE", "ATESTART"},
	{ATECHANNEL, "ATECHANNEL", "1"},
	{ATETXMODE, "ATETXMODE", "2"},
	{ATETXMSC, "ATETXMSC", "1"},
	{ATETXBW, "ATETXBW", "1"},
	{ATETXGI, "ATETXGI", "0"},
	{ATETXFREQOFFSET, "ATETXFREQOFFSET", "36"},
	{ATETXANT, "ATETXANT", "0"},
	{ATEMODETYPE, "ATE", "RXFRAME"},
	{ATE_CMD_ENUM_MAX, "", ""},
};

int UpdateATECommand(ATE_CMD_st *ATE_CMD_List, ATE_CMD_INDEX KeyWordIndex, char *pValue)
{
	if (NULL == pValue || KeyWordIndex > ATE_CMD_ENUM_MAX)
	{
		LOGE("%s: Value is NULL or Index invalid", __func__);
		return ERROR;
	}
	
	int i = 0;
	while (TRUE)
	{
		if (ATE_CMD_ENUM_MAX == ATE_CMD_List[i].iIndex)
		{
			LOGD("%s: KeyWord Index %d can't find", __func__, KeyWordIndex);
			break;
		}
		
		if (KeyWordIndex == ATE_CMD_List[i].iIndex)
		{
			LOGD("%s: Before update %s = %s\n", __func__, ATE_CMD_List[i].szKeyword, 
											ATE_CMD_List[i].szValue);
			memset(ATE_CMD_List[i].szValue, '\0', ATE_VALUE_LENGTH);
			strncpy(ATE_CMD_List[i].szValue, pValue, strlen(pValue));
			LOGD("%s: After update %s = %s\n", __func__, ATE_CMD_List[i].szKeyword, 
											ATE_CMD_List[i].szValue);
			break;
		}

		i ++;
	}
	
	return SUCCESS;
} 

int SendATECommand(ATE_CMD_st *ATE_CMD_List)
{
	char pATECmdBuf[ATE_COMMAND_LENGTH] = "";
	FILE *fpOut = NULL;
	char pResult[1024] = "";
	
	if (NULL == ATE_CMD_List)
	{
		LOGE("%s: ATE CMD List is NULL\n", __func__);
		return ERROR;
	}
	
	int i = 0;
	while (TRUE)
	{
		if (ATE_CMD_ENUM_MAX == ATE_CMD_List[i].iIndex)
		{
			LOGD("%s: ATE CMD Send done!\n", __func__);
			break;
		}
		memset(pATECmdBuf, '\0', ATE_COMMAND_LENGTH);
		snprintf(pATECmdBuf, ATE_COMMAND_LENGTH, "iwpriv %s set %s=%s", MT76xx_WLAN_INTERFACE, 
							 ATE_CMD_List[i].szKeyword, ATE_CMD_List[i].szValue);
/*
		fpOut = popen(pATECmdBuf, "r");
		if (NULL == fpOut)
		{
			LOGE("%s: popen error!\n", __func__);
			return ERROR;
		}
		
		fgets(pResult, 1024, fpOut);
		printf("%s\n", pResult);
		pclose(fpOut);
*/		
		LOGD("%s: [%d] %s\n", __func__, i, pATECmdBuf);
		i ++;
	}
	
	return SUCCESS;
}


#define BINAARY 	0x01
#define OCTAL 		0x02
#define DECIMAL 	0x03
#define HEXADECIMAL 0x04
static int Str2Int(char *pStr, int iSystem)
{
	int iResult = 0;
	char *pCur = NULL;
	int iCount = 0;
	int iPostion = 0;
	int iMulNum = 0;
	int iMulSum = 0;
	int iMulTmp = 0;
	
	if (NULL == pStr)
	{
		return -1;
	}
	
	iCount = strlen(pStr);
	//pCur pointer the last character of pStr
	pCur = pStr + iCount -1;
	printf("%d\n", iCount);
	
	switch (iSystem)
	{
		case BINAARY:
			break;
		case OCTAL:
			break;
		case DECIMAL:
			break;
		case HEXADECIMAL:
			iMulNum = 16;
			iMulSum = 1;
			while (iPostion < iCount)
			{
				if (0 != iPostion)
				{
					iMulSum = iMulSum * iMulNum;
				}
				
				if (*pCur >= 0x30 && *pCur <= 0x39)
				{
					iResult += (*pCur - 0x30) * iMulSum; 
				}
				else if (*pCur >= 0x61 && *pCur <= 0x66)
				{
					iResult += (*pCur - 0x61 + 10) * iMulSum;
				}
				else if (*pCur >= 0x41 && *pCur <= 0x46)
				{
					iResult += (*pCur - 0x41 + 10) * iMulSum;
				}
				else
				{
					return -1;
				}
				iPostion ++;
				pCur --;
			}
			break;
		default:
			break;
	}
	
	return iResult;
}

static char *Int2Str(int iNumber, char *pString, int iSystem)
{
	char szTmp[64] = {0};
	char *pRet = NULL;
	int sign = 0;
	int iNum = 0;
	int iRe = 0;
	int i = 0;
	
	iNum = iNumber;
	pRet = pString;
	
	if (0 == iNum)
	{
		*pRet = '0';
		return pString;
	}
	
	if (iNum < 0)
	{
		sign = 1;
		iNum = -iNumber;
		*pRet++ = '-';
	}

	while (iNum > 0)
	{
		iRe = iNum % iSystem;
		iNum = iNum / iSystem;
		if (iRe < 10)
		{
			szTmp[i] = '0' + iRe;
		}
		else
		{
			szTmp[i] = 'a' + iRe - 10;
		}
		i ++;
	}

	while (i > 0)
	{
		*pRet++ = szTmp[--i];
	}
	*pRet = 0;
	
	return pString;
}

int main(int argc, char *argv[])
{
	FILE *fpOut = NULL;
	char pResult[1024] = "";
	
	SendATECommand(g_iwpriv_cmd_rx);
	SendATECommand(g_iwpriv_cmd_tx);
	
	UpdateATECommand(g_iwpriv_cmd_rx, ATECHANNEL, (char *)"13");
	UpdateATECommand(g_iwpriv_cmd_rx, ATETXMODE, (char *)"0");
	UpdateATECommand(g_iwpriv_cmd_rx, ATETXBW, (char *)"0");
	
	
	UpdateATECommand(g_iwpriv_cmd_tx, ATECHANNEL, (char *)"8000");
	UpdateATECommand(g_iwpriv_cmd_tx, ATETXMODE, (char *)"8000");
	UpdateATECommand(g_iwpriv_cmd_tx, ATETXMSC, (char *)"8000");
	UpdateATECommand(g_iwpriv_cmd_tx, ATETXBW, (char *)"8000");
	UpdateATECommand(g_iwpriv_cmd_tx, ATETXGI, (char *)"8000");
	UpdateATECommand(g_iwpriv_cmd_tx, ATETXLEN, (char *)"8000");
	UpdateATECommand(g_iwpriv_cmd_tx, ATETXPOW0, (char *)"8000");
	UpdateATECommand(g_iwpriv_cmd_tx, ATETXCNT, (char *)"8000");
	UpdateATECommand(g_iwpriv_cmd_tx, ATEMODETYPE, (char *)"2");

	fpOut = popen("ifconfig", "r");
		if (NULL == fpOut)
		{
			LOGE("%s: popen error!\n", __func__);
			return ERROR;
		}
		printf("===========================\n");
		while (NULL != fgets(pResult, 1024, fpOut))
		{
		printf("%s", pResult);
		}
		printf("===========================\n");
	pclose(fpOut);
	SendATECommand(g_iwpriv_cmd_rx);
	SendATECommand(g_iwpriv_cmd_tx);
	
	int a = -23834;
	int b = 12344;
	int c = 0xac4c;
	char tmp[32] = {0};
	
	printf("Int2Str %d to %s\n", a, Int2Str(a, tmp, 10));
	printf("Int2Str %d to %s\n", b, Int2Str(b, tmp, 10));
	printf("Int2Str %x to %s\n", c, Int2Str(c, tmp, 16));
	
	char szHex[] = "40";
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("Hex2Int: %s = %d\n", szHex, Str2Int(szHex, HEXADECIMAL));
	return SUCCESS;
}