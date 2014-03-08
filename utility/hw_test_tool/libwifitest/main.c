/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "libwifitest.h"
#include "lib.h"

#define PACKAGE     "wifitest"


char *bg_rate[] = {
"RATE_AUTO",         
"RATE_1MBPS",
"RATE_2MBPS",        
"RATE_5_5MBPS",      
"RATE_6MBPS",        
"RATE_9MBPS",        
"RATE_11MBPS",      
"RATE_12MBPS",     
"RATE_18MBPS",     
"RATE_24MBPS",     
"RATE_36MBPS",       
"RATE_48MBPS",       
"RATE_54MBPS",      
};
char *preamble[] = {
"LONG",         
"SHORT",
};
char *bandwidth[] = {
"BW20",         
"BW40",
"BW20U",         
"BW20L",
};

void wifi_sensitivity(int, int);
void wifi_tx();


void signal_handler(int sig) 
{
    int retval = 0;

    retval = WIFI_TEST_CloseDUT();
    printf("\n(%d) aborted ..\n", retval);

    signal(SIGINT, SIG_DFL);
    exit(0);
}

//  "E:e:M:g:G:I:B:R:N:T:m:i:s:p:b:t:hVw:v:l:f    :c:rn:"


void print_help(int exval)
{
    printf("%s -I <interface> -I <interface> [-E][-e][-M][-g][-G][-B][-R][-N][-i][-s][-p][-t][-h][-w][-v][-l][-c][-r][-n]\n\n", PACKAGE);

    exit(exval);
}

static int channel = 1;
static int times = 10;
static int numBurst = 0;
static int txMode = 0;
static char macAddr[] = {0x00, 0x80, 0x12, 0x13, 0x14, 0x15};
static int txGain = 10;
static int payloadLength = 1024;
static int SIFS = 20;
static int g_rate = 6;
static ENUM_WIFI_TEST_MCS_RATE gMCSrate = WIFI_TEST_MCS_RATE_0;
static int g_bandwidth = WIFI_TEST_BW_20;
static ENUM_WIFI_TEST_PREAMBLE_TYPE gMode = WIFI_TEST_PREAMBLE_TYPE_MIXED_MODE;
static ENUM_WIFI_TEST_GI_TYPE giType = WIFI_TEST_GI_TYPE_NORMAL_GI;
extern char WIFI_IF_NAME[256];
static PreambleType_t pType = WIFI_TEST_PREAMBLE_SHORT;
static unsigned int mcr_addr = 0;
static unsigned int mcr_value = 0;
static unsigned int efuse_addr = 0;
static unsigned int efuse_value = 0;
static int cw_mode = -1;

int main(int argc, char *argv[]) 
{
    int opt = 0;
    int rx_flag = 0;    
    int tx_flag = 0;
    int mcr_read_flag = 0;
    int mcr_write_flag = 0;
    int efuse_read_flag = 0;
    int efuse_write_flag = 0;

    if (argc == 1) {
        fprintf(stderr, "This program needs arguments....\n\n");
        print_help(1);
    }

        while ((opt = getopt(argc, argv, "E:e:M:g:G:I:B:R:N:T:m:i:s:p:b:t:hVw:v:l:f:c:rn:")) != -1) {
        
        switch(opt) {
            case 'e':
                {
                    efuse_write_flag = 1;
                    xtoi(optarg, &efuse_addr);
                }
                break;
            case 'E':
                {
                    efuse_read_flag = 1;
                    xtoi(optarg, &efuse_addr);
                }
                break;
            case 'w':
                {
                    mcr_write_flag = 1;
                    xtoi(optarg, &mcr_addr);
                }
            case 'M':
                {
                    mcr_read_flag = 1;
                    xtoi(optarg, &mcr_addr);
                }
                break;
            case 'g':
                if (!atoi(optarg)) {
                    gMode = WIFI_TEST_PREAMBLE_TYPE_MIXED_MODE;
                } else {
                    gMode = WIFI_TEST_PREAMBLE_TYPE_GREENFIELD;
                }
                break;
            case 'G':
                if (!atoi(optarg)) {
                    giType = WIFI_TEST_GI_TYPE_NORMAL_GI;
                } else {
    
                    giType = WIFI_TEST_GI_TYPE_SHORT_GI;
                }
                break;

            case 'I':
                strcpy(WIFI_IF_NAME, optarg);
                break;
            case 'B':
                {
                    int val = atoi(optarg);
                    switch(val) {
                        case 0:
                            g_bandwidth = WIFI_TEST_BW_20;
                            break;
                        case 1:
                            g_bandwidth = WIFI_TEST_BW_40;
                            break;
                        case 2:
                            g_bandwidth = WIFI_TEST_BW_20U;
                            break;
                        case 3:
                            g_bandwidth = WIFI_TEST_BW_20L;
                            break;
                    }
                }           
                break;
            case 'N':
                {
                    int val = atoi(optarg);
                    switch(val) {
                        case 0:
                            gMCSrate = WIFI_TEST_MCS_RATE_0;
                            break;
                        case 1:
                            gMCSrate = WIFI_TEST_MCS_RATE_1;
                            break;
                        case 2:
                            gMCSrate = WIFI_TEST_MCS_RATE_2;
                            break;
                        case 3:
                            gMCSrate = WIFI_TEST_MCS_RATE_3;
                            break;
                        case 4:
                            gMCSrate = WIFI_TEST_MCS_RATE_4;
                            break;
                        case 5:
                            gMCSrate = WIFI_TEST_MCS_RATE_5;
                            break;
                        case 6:
                            gMCSrate = WIFI_TEST_MCS_RATE_6;
                            break;
                        case 7:
                            gMCSrate = WIFI_TEST_MCS_RATE_7;
                            break;
                        case 32:
                            gMCSrate = WIFI_TEST_MCS_RATE_32;
                            break;
                    }
                }           
                break;
            case 'R':
                g_rate = atoi(optarg);
                break;
            
            case 'i':
                SIFS = atoi(optarg);
                break;
            case 'p':
                txGain = atoi(optarg);
                break;
            case 'l':
                payloadLength = atoi(optarg);
                break;
            case 't':
                tx_flag = 1;
                txMode = atoi(optarg);
                break;
            case 'b':
                //isBurst = 1;
                break;

            case 'h':
                print_help(0);
                break;
            case ':':
                print_help(0);
                break;
            case 'n':
                times = atoi(optarg);
                break;
            case 'c':
                channel = atoi(optarg);
                break;
            case 'r':
                {
                    rx_flag = 1;
                    break;
                }
            case 'V':
                break;
            case 'v':
                {
                    xtoi(optarg, &mcr_value);
                }
                break;
            case 's':
                if (!atoi(optarg)) {
                    pType = WIFI_TEST_PREAMBLE_SHORT;
                } else {
                    pType = WIFI_TEST_PREAMBLE_LONG;
                }
		break;
		
	    case 'm':
                cw_mode = atoi(optarg);
                break;

            case '?':
                fprintf(stderr, "%s: Error - No such option: `%c`\r", PACKAGE, optopt);
                print_help(1);
                break;
        }
    }

    // set up the Ctrl + C handler
    signal(SIGINT, signal_handler);


    if (efuse_write_flag) {
        int retval = 0;
        int val = 0;

        retval = WIFI_TEST_OpenDUT();
        printf("(%d) entered RF testing mode ..\n", retval);

        if(retval = WIFI_TEST_EFUSE_Write(efuse_addr, mcr_value)) {
            printf("(%d) Wirte EFUSE addr 0x%x value 0x%x\n", retval, efuse_addr, mcr_value);
        } else{
            printf("(%d) Write EFUSE addr 0x%x value 0x%x\n", retval, efuse_addr, mcr_value);
        }
   
        retval = WIFI_TEST_CloseDUT();
        printf("\n(%d) left RF testing mode ..\n", retval);

    } else if (efuse_read_flag) {
        int retval = 0;
        int val = 0;

        retval = WIFI_TEST_OpenDUT();
        printf("(%d) entered RF testing mode ..\n", retval);

        if(retval = WIFI_TEST_EFUSE_Read(efuse_addr, &val)) {
            printf("(%d) EFUSE addr 0x%x value 0x%x\n", retval, efuse_addr, val);
        } else{
            printf("(%d) EFUSE addr 0x%x value 0x%x\n", retval, efuse_addr, val);
        }
   
        retval = WIFI_TEST_CloseDUT();
        printf("\n(%d) left RF testing mode ..\n", retval);

    } if (mcr_write_flag) {
        int retval = 0;
        if (retval = WIFI_TEST_MCR_Write(mcr_addr, mcr_value)) {
            printf("(%d) MCR addr 0x%x is set to  value 0x%x\n", retval, mcr_addr, mcr_value);
        } else {
            printf("(%d) MCR addr 0x%x is set to  value 0x%x\n", retval, mcr_addr, mcr_value);
        }

    } else if (mcr_read_flag) {
        int retval = 0;
        int val = 0;

        if(retval = WIFI_TEST_MCR_Read(mcr_addr, &val)) {
            printf("(%d) MCR addr 0x%x value 0x%x\n", retval, mcr_addr, val);
        } else {
            printf("(%d) MCR addr 0x%x value 0x%x\n", retval, mcr_addr, val);
        }    

    } else if (rx_flag) {
            wifi_sensitivity(times, channel);
    } else if (tx_flag) {
            wifi_tx();
    }
        return 0;
}

void wifi_sensitivity(int times, int channel)
{
    int i;
    int rxOk, rxErr, rxRssi;
   	unsigned char rxRssiTemp;
    char macAddr[6];
    signed char rxRssisFinal;
    bool retval;
    bool ret[3];

    retval = WIFI_TEST_OpenDUT();
    printf("(%d) entered RF testing mode ..\n", retval);

    sleep(1);

    retval = WIFI_TEST_Channel(channel);
    printf("(%d) changed channel to #%d..\n", retval, channel);

		retval = WIFI_TEST_SetBandWidth(g_bandwidth);
		printf("(%d) Change bandwidth to %s.\n", retval, bandwidth[g_bandwidth]);
		
    retval = WIFI_TEST_RxStart();
    printf("(%d) RX test started..\n", retval);

    for(i = 0 ; i < times ; i++) {
        ret[0] = WIFI_TEST_FRGood(&rxOk);
        ret[1] = WIFI_TEST_FRError(&rxErr);
        ret[2] = WIFI_TEST_RSSI(&rxRssisFinal);
				
//				rxRssiTemp = (unsigned char)rxRssi;
//				rxRssisFinal = (char)rxRssiTemp;
        fprintf(stdout, "[%d] (%d)RX OK: %d / (%d)RX ERR: %d / PER: %d .. / (%d)RSSI: %i\r", i, ret[0], rxOk, ret[1], rxErr,
        								  (100 * rxErr)/(rxOk + rxErr), ret[2], rxRssisFinal);
        fflush(stdout);
        sleep(1);
    }

    retval = WIFI_TEST_RxStop();
    retval = WIFI_TEST_CloseDUT();
    printf("\n(%d) left RF testing mode ..\n", retval);

}

void wifi_tx(void)
{
    int i;
    int rxOk, rxErr, rxRssi;
    bool retval;
    bool ret[3];

    retval = WIFI_TEST_OpenDUT();
    printf("(%d) entered RF testing mode ..\n", retval);

    sleep(1);

    WIFI_TEST_TxDestAddress(macAddr);

    retval = WIFI_TEST_Channel(channel);
    printf("(%d) changed channel to #%d..\n", retval, channel);

    
    retval = WIFI_TEST_SetBandWidth(g_bandwidth);
    printf("(%d) changed bandwidth to %s..\n", retval, bandwidth[g_bandwidth]);

    retval = WIFI_TEST_TxGain(txGain);
    printf("(%d) changed tx gain to %d..\n", retval, txGain);

    retval = WIFI_TEST_TxPayloadLength(payloadLength);
    printf("(%d) changed tx payload to %d bytes..\n", retval, payloadLength);
 

    retval = WIFI_TEST_SetPreamble(pType);

    printf("(%d) changed to %s preamble..\n", retval, preamble[pType]);
 
    retval = WIFI_TEST_TxBurstInterval(SIFS);
    printf("(%d) changed SIFS to %d\n", retval, SIFS);
  

    retval = WIFI_TEST_TxBurstFrames(times);
    if (!txMode) {
        retval = WIFI_TEST_TxDataRate(g_rate);
        printf("(%d) changed to tx rate %s\n", retval, bg_rate[g_rate]);
    } else {
        WIFI_TEST_TxDataRate11n(gMCSrate, gMode, giType);
    }

    //for CW mode
    if(-1 != cw_mode){
				retval = WIFI_TEST_CW_MODE(cw_mode);
				printf("(%d) cw mode set to %d\n", retval, cw_mode);
				retval = WIFI_TEST_CW_MODE_START();
				printf("(%d) cw mode start.\n", retval);
    }else{
    	printf("no cw mode configuration.\n");
    	retval = WIFI_TEST_TxStart();
    	printf("(%d) TX test started..\n", retval);
    }

    sleep(100000);
    retval = WIFI_TEST_TxStop();
    retval = WIFI_TEST_CloseDUT();
    printf("\n(%d) left RF testing mode ..\n", retval);
}
