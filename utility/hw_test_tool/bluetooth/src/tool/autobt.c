/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <getopt.h>
#include "bt_em.h"


#define VERSION "2.0"

#define for_each_opt(opt, long, short) while ((opt=getopt_long(argc, argv, short ? short:"+", long, NULL)) != -1)

/*  Global Variables  */
//static int   bt_init = 0;
static bool inquiry_complete = false;

//================       FUNCTIONS       ===================

static int check_hex_str(char *str)
{
    int i = 0;
    int len = strlen(str);
    
    for(; i < len; i++){
        if((('a' <= str[i]) && (str[i] <= 'z')) || (('A' <= str[i]) && (str[i] <= 'Z')) || (('0' <= str[i]) && (str[i] <= '9'))){
            ;
        }
        else{
            return -1;
        }
    }
    return 0;
}

void print_info(char *info)
{
    printf("%s", info);
    if(0 == strcmp(info, "---Inquiry completed---\n"))
        inquiry_complete = true;
}

/*static struct option poweron_options[] = {
    { "help",	0, 0, 'h' },
    { 0, 0, 0, 0 }
};

static const char *poweron_help =
    "Usage:\n"
    "\tpoweron [no option]\n";

static void cmd_power_on(int argc, char **argv)
{
    int opt;
    
    for_each_opt(opt, poweron_options, "+h") {
        switch (opt) {
        case 'h':
        default:
            printf("%s", poweron_help);
            return;
        }
    }
    
    if (EM_BT_init()){
        printf("BT device power on success\n");
        bt_init = 1;
    }
    else {
        printf("BT device power on failed\n");
    }
    return;
}

static struct option poweroff_options[] = {
    { "help",	0, 0, 'h' },
    { 0, 0, 0, 0 }
};

static const char *poweroff_help =
    "Usage:\n"
    "\tpoweroff [no option]\n";

static void cmd_power_off(int argc, char **argv)
{
    int opt;
    
    for_each_opt(opt, poweroff_options, "+h") {
        switch (opt) {
        case 'h':
        default:
            printf("%s", poweroff_help);
            return;
        }
    }
    
    if (bt_init){
        EM_BT_deinit();
        bt_init = 0;
    }
    
    printf("BT device power off\n");
    return;    
}

static struct option reset_options[] = {
    { "help",	0, 0, 'h' },
    { 0, 0, 0, 0 }
};

static const char *reset_help =
    "Usage:\n"
    "\treset [no option]\n";

static void cmd_reset(int argc, char **argv)
{
    int opt;
    
    for_each_opt(opt, reset_options, "+h") {
        switch (opt) {
        case 'h':
        default:
            printf("%s", reset_help);
            return;
        }
    }
    
    if (!bt_init){
        printf("BT device is closed, run \"autobt poweron\" first\n");
        return;
    }
    
    if (EM_BT_reset()){
       printf("BT device reset success\n");
    }
    else {
       printf("BT device reset failed\n");
    }
    return;
}*/

static struct option tx_options[] = {
    { "help",	0, 0, 'h' },
    { "pattern", 1, 0, 'p' },
    { "channel", 1, 0, 'c' },
    { "freq", 1, 0, 'f' },
    { "type", 1, 0, 't' },
    { "length", 1, 0, 'l' },
    { 0, 0, 0, 0 }
};

static const char *tx_help =
    "Usage:\n"
    "\ttx [--pattern] 0x01:\tTx 0000 pattern\n"
    "\t               0x02:\tTx 1111 pattern\n"
    "\t               0x03:\tTx 1010 pattern\n"
    "\t               0x04:\tTx pseudo random bit sequence\n"
    "\t               0x09:\tTx 11110000 pattern\n"
    "\t               0x0A:\tTx single tone\n"
    "\n"
    "\t   [--channel] 0x00:\tSingle frequency\n"
    "\t               0x01:\t79 channels frequency hopping\n"
    "\n"
    "\t   [--freq]    integer (0~78 for single frequency)\n"
    "\n"
    "\t   [--type]    0x00:\tNULL\n"
    "\t               0x01:\tPOLL\n"
    "\t               0x02:\tFHS\n"
    "\t               0x03:\tDM1\n"
    "\t               0x04:\tDH1\n"
    "\t               0x05:\tHV1\n"
    "\t               0x06:\tHV2\n"
    "\t               0x07:\tHV3\n"
    "\t               0x08:\tDV\n"
    "\t               0x09:\tAUX\n"
    "\t               0x0A:\tDM3\n"
    "\t               0x0B:\tDH3\n"
    "\t               0x0E:\tDM5\n"
    "\t               0x0F:\tDH5\n"
    "\t               0x17:\tEV3\n"
    "\t               0x1C:\tEV4\n"
    "\t               0x1D:\tEV5\n"
    "\t               0x24:\t2-DH1\n"
    "\t               0x28:\t3-DH1\n"
    "\t               0x2A:\t2-DH3\n"
    "\t               0x2B:\t3-DH3\n"
    "\t               0x2E:\t2-DH5\n"
    "\t               0x2F:\t3-DH5\n"
    "\t               0x36:\t2-EV3\n"
    "\t               0x37:\t3-EV3\n"
    "\t               0x3C:\t2-EV5\n"
    "\t               0x3D:\t3-EV5\n"
    "\n"
    "\t   [--length]  integer\n"
    "\n\n"
    "\texample:\n"
    "\t\tautobt tx --pattern 0x01 --channel 0x00 --freq 7 --type 0x04 --length 27\n"
    "\n\n"
    "Notice:\n"
    "\tThis command is to start Tx only test, controller will send out the specified Tx packet continuously\n"
    "\tTo end test, please type in \"exit\"\n"
    "\n";

static void cmd_tx_test(int argc, char **argv)
{
    int opt, opt_num = 0;
    unsigned char pattern, channel, type;
    int freq;
    int length;
    
    char buf[5] = {0}; //receive terminal input
    char tmp;
    int i, result;
    
    for_each_opt(opt, tx_options, "+p:c:f:t:l:h") {
        opt_num ++;
        switch (opt) {
        case 'p':
            pattern = (unsigned char)strtol(optarg, NULL, 16);
            break;
            
        case 'c':
            channel = (unsigned char)strtol(optarg, NULL, 16);
            break;
            
        case 'f':
            freq = atoi(optarg);
            if (freq < 0 || freq > 78){
                printf("Invalid command option parameter!\n");
                printf("%s", tx_help);
                return;
            }
            break;
            
        case 't':
            type = (unsigned char)strtol(optarg, NULL, 16);
            break;
            
        case 'l':
            length = atoi(optarg);
            break;
            
        case 'h':
        default:
            printf("%s", tx_help);
            return;
        }
    }
    
    if (opt_num < 5){
        printf("Incomplete command options!\n");
        printf("%s", tx_help);
        return;
    }
    
    // BT power on & initialize
    if (EM_BT_init()){
        printf("BT device power on success\n");
    }
    else {
        printf("BT device power on failed\n");
        return;
    }
    
    printf("Tx pattern: 0x%02x\n", pattern);
    printf("Channel: 0x%02x\n", channel);
    printf("Tx frequency: %d\n", freq);
    printf("Packet type: 0x%02x\n", type);
    printf("Packet length: %d\n", length);
    
    result = EM_BT_TxOnlyTest_start(
                   pattern, 
                   channel, 
                   freq, 
                   type, 
                   length);
    if (result){
        printf("Tx test start...\n");
    }
    else{
        printf("Try to start Tx test, failed\n");
        EM_BT_deinit();
        printf("BT device power off\n");
        return;
    }
    
    // Loop in waiting for user type "exit"
    i = 0;
    do{
        if (i >= 5) 
            i = 0; //rollback
        
        tmp = getchar();
        buf[i] = tmp;
        
        if (tmp != '\r' && tmp != '\n'){
            i ++;
        }
        else{
            buf[i] = '\0';
            if(strcmp(buf, "exit"))
                i = 0; //discard this string
            else
                break;
        }
    } while(1);
    
    
    EM_BT_TxOnlyTest_end();
    printf("Tx test complete\n");
    
    EM_BT_deinit();
    printf("BT device power off\n");
    
    return;
}

static struct option nsrx_options[] = {
    { "help",	0, 0, 'h' },
    { "pattern", 1, 0, 'p' },
    { "freq", 1, 0, 'f' },
    { "type", 1, 0, 't' },
    { "addr", 1, 0, 'a' },
    { 0, 0, 0, 0 }
};

static const char *nsrx_help =
    "Usage:\n"
    "\tnsrx [--pattern] 0x01:\tRx 0000 pattern\n"
    "\t                 0x02:\tRx 1111 pattern\n"
    "\t                 0x03:\tRx 1010 pattern\n"
    "\t                 0x04:\tRx pseudo random bit sequence\n"
    "\t                 0x09:\tRx 11110000 pattern\n"
    "\n"
    "\t     [--freq]    integer (0~78 for single frequency)\n"
    "\n"
    "\t     [--type]    -- BR packet --\n"
    "\t                 0x03:\tDM1\n"
    "\t                 0x04:\tDH1\n"
    "\t                 0x0A:\tDM3\n"
    "\t                 0x0B:\tDH3\n"
    "\t                 0x0E:\tDM5\n"
    "\t                 0x0F:\tDH5\n"
    "\t                 -- EDR packet --\n"
    "\t                 0x24:\t2-DH1\n"
    "\t                 0x28:\t3-DH1\n"
    "\t                 0x2A:\t2-DH3\n"
    "\t                 0x2B:\t3-DH3\n"
    "\t                 0x2E:\t2-DH5\n"
    "\t                 0x2F:\t3-DH5\n"
    "\n"
    "\t     [--addr]    hex XXXXXXXX (UAP+LAP 4 bytes)\n"
    "\t                 if set 0, use default value 0x00A5F0C3\n"
    "\n\n"
    "\texample:\n"
    "\t\tautobt nsrx --pattern 0x02 --freq 5 --type 0x2A --addr 88C0FFEE\n"
    "\n\n"
    "Notice:\n"
    "\tThis command is to start Non-Signal-Rx test\n"
    "\tTo end test, please type in \"exit\", then the PER & BER during test will be returned\n"
    "\n";

static void cmd_non_signal_rx(int argc, char **argv)
{
    int opt, opt_num = 0;
    unsigned char pattern, type;
    int freq;
    unsigned long addr; //4 bytes
    unsigned long pkt_count;
    unsigned long byte_count;
    float PER, BER;
    
    char buf[5] = {0}; //receive terminal input
    char tmp;
    int i, result;
    
    for_each_opt(opt, nsrx_options, "+p:f:t:a:h") {
        opt_num ++;
        switch (opt) {
        case 'p':
            pattern = (unsigned char)strtol(optarg, NULL, 16);
            break;
            
        case 'f':
            freq = atoi(optarg);
            if (freq < 0 || freq > 78){
                printf("Invalid command option parameter!\n");
                printf("%s", nsrx_help);
                return;
            }
            break;
            
        case 't':
            type = (unsigned char)strtol(optarg, NULL, 16);
            break;
            
        case 'a':
            if (strcmp(optarg, "0")){
                if (strlen(optarg) != 8){
                    printf("Invalid command option parameter!\n");
                    printf("%s", nsrx_help);
                    return;
                }
                else{
                    if (check_hex_str(optarg)){
                        printf("Invalid command option parameter!\n");
                        printf("%s", nsrx_help);
                        return;
                    }
                    else{
                        addr = strtoul(optarg, NULL, 16);
                    }
                }
            }
            else{
                addr = 0x00A5F0C3;
            }
            break;
            
        case 'h':
        default:
            printf("%s", nsrx_help);
            return;
        }
    }
    
    if (opt_num < 4){
        printf("Incomplete command options!\n");
        printf("%s", nsrx_help);
        return;
    }
    
    // BT power on & initialize
    if (EM_BT_init()){
        printf("BT device power on success\n");
    }
    else {
        printf("BT device power on failed\n");
        return;
    }
    
    printf("Rx pattern: 0x%02x\n", pattern);
    printf("Rx frequency: %d\n", freq);
    printf("Packet type: 0x%02x\n", type);
    printf("Tester address: %08x\n", addr);
    
    result = EM_BT_NonSignalRx_start(
                   pattern, 
                   freq, 
                   type, 
                   addr);
    if (result){
        printf("Non-Signal-Rx test start...\n");
    }
    else {
        printf("Try to start Non-Signal-Rx test, failed\n");
        EM_BT_deinit();
        printf("BT device power off\n");
        return;
    }
    
    // Loop in waiting for user type "exit"
    i = 0;
    do{
        if (i >= 5) 
            i = 0; //rollback
        
        tmp = getchar();
        buf[i] = tmp;
        
        if (tmp != '\r' && tmp != '\n'){
            i ++;
        }
        else{
            buf[i] = '\0';
            if(strcmp(buf, "exit"))
                i = 0; //discard this string
            else
                break;
        }
    } while(1);
    
    
    result = EM_BT_NonSignalRx_end(
                   &pkt_count,
                   &PER,
                   &byte_count,
                   &BER);
    if (result){
        printf("Non-Signal-Rx test complete\n");
        printf("Receive packet count: %d\n", pkt_count);
        printf("Packet Error Rate: %f%%\n", PER);
        printf("Receive byte count: %d\n", byte_count);
        printf("Bit Error Rate: %f%%\n", BER);
    }
    else{
        printf("Try to end Non-Signal-Rx test, failed\n");
    }
    
    EM_BT_deinit();
    printf("BT device power off\n");
    return;
}

static struct option testmode_options[] = {
    { "help",	0, 0, 'h' },
    { "power", 1, 0, 'p' },
    { 0, 0, 0, 0 }
};

static const char *testmode_help =
    "Usage:\n"
    "\ttestmode [--power] integer (range: 0~7)\n"
    "\t                   if not set, use default value 7\n"
    "\n\n"
    "\texample:\n"
    "\t\tautobt testmode --power 6\n"
    "\n\n"
    "Notice:\n"
    "\tThis command is to enable BT device under test mode\n"
    "\tTo exit test mode, please type in \"exit\"\n"
    "\n";

static void cmd_test_mode(int argc, char **argv)
{
    int opt;
    int power = 7;
    
    char buf[5] = {0}; //receive terminal input
    char tmp;
    int i, result;
    
    for_each_opt(opt, testmode_options, "+p:h") {
        switch (opt) {
        case 'p':
            power = atoi(optarg);
            if (power < 0 || power > 7){
                printf("Invalid command option parameter!\n");
                printf("%s", testmode_help);
                return;
            }
            break;
            
        case 'h':
        default:
            printf("%s", testmode_help);
            return;
        }
    }
    
    // BT power on & initialize
    if (EM_BT_init()){
        printf("BT device power on success\n");
    }
    else {
        printf("BT device power on failed\n");
        return;
    }
    
    result = EM_BT_TestMode_enter(power);
    
    if (result){
        printf("Test mode entered, you can start to test...\n");
    }
    else {
        printf("Enable BT device under test mode failed\n");
        EM_BT_deinit();
        printf("BT device power off\n");
        return;
    }
    
    // Loop in waiting for user type "exit"
    i = 0;
    do{
        if (i >= 5) 
            i = 0; //rollback
        
        tmp = getchar();
        buf[i] = tmp;
        
        if (tmp != '\r' && tmp != '\n'){
            i ++;
        }
        else{
            buf[i] = '\0';
            if(strcmp(buf, "exit"))
                i = 0; //discard this string
            else
                break;
        }
    } while(1);
    
    
    EM_BT_TestMode_exit();
    printf("Test mode exit\n");
    
    EM_BT_deinit();
    printf("BT device power off\n");
    
    return;
}

static struct option inquiry_options[] = {
    { "help",	0, 0, 'h' },
    { 0, 0, 0, 0 }
};

static const char *inquiry_help =
    "Usage:\n"
    "\tinquiry [no command option]\n"
    "\n\n"
    "\texample:\n"
    "\t\tautobt inquiry\n"
    "\n";

static void cmd_inquiry(int argc, char **argv)
{
    int opt;
    int result;
    
    for_each_opt(opt, inquiry_options, "+h") {
        switch (opt) {
        case 'h':
        default:
            printf("%s", inquiry_help);
            return;
        }
    }
    
    // BT power on & initialize
    if (EM_BT_init()){
        printf("BT device power on success\n");
    }
    else {
        printf("BT device power on failed\n");
        return;
    }
    
    result = EM_BT_Inquiry(print_info);
    
    if (result){
        printf("Inquiry remote devices...\n");
    }
    else {
        printf("Start inquiry procedure failed\n");
        EM_BT_deinit();
        printf("BT device power off\n");
        return;
    }
    
    // Loop in waiting for inquiry complete
    inquiry_complete = false;
    do{
        sleep(7); //Since the inquiry length is 6.4s set in driver
    } while(!inquiry_complete);
    
    
    EM_BT_deinit();
    printf("BT device power off\n");
    
    return;
}


static struct {
    char *cmd;
    void (*func)(int argc, char **argv);
    char *doc;
} command[] = {
    { "tx",        cmd_tx_test,        "Tx only test"                      },
    { "nsrx",      cmd_non_signal_rx,  "Non-Signal-Rx test"                },
    { "testmode",  cmd_test_mode,      "Enable BT device under test mode"  },
    { "inquiry",   cmd_inquiry,        "Inquiry remote devices"            },
    { NULL, NULL, 0 }
};

static void usage(void)
{
    int i;
    
    printf("autobt test tool - ver %s\n", VERSION);
    printf("Usage:\n"
        "\tautobt <command> [command options] [command parameters]\n");
    printf("Commands:\n");
    for (i = 0; command[i].cmd; i++){
        printf("\t%-8s\t%s\n", command[i].cmd, command[i].doc);
    }
    printf("\n"
        "For more information on the usage of each command use:\n"
        "\tautobt <command> --help\n\n\n");
}

static struct option main_options[] = {
    { "help",	0, 0, 'h' },
    { 0, 0, 0, 0 }
};

int main(int argc, char *argv[])
{
    int opt, i;
    
    while ((opt=getopt_long(argc, argv, "+h", main_options, NULL)) != -1) {
        switch (opt) {
        case 'h':
        default:
            usage();
            exit(0);
        }
    }
    
    argc -= optind;
    argv += optind;
    optind = 0;
    
    if (argc < 1) {
        usage();
        exit(0);
    }
    
    for (i = 0; command[i].cmd; i++) {
        if (strcmp(command[i].cmd, argv[0]))
           continue;
        command[i].func(argc, argv);
        break;
    }
    
    return 0;
}
