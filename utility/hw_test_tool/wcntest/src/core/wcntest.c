/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
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

/* 
 *
 * (C) Copyright 2011 
 * MediaTek <www.MediaTek.com>
 * Hongcheng Xia <Hongcheng.Xia@MediaTek.com>
 *
 */

//for fm auto test and debug

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/resource.h>
#include "wcn_cmd.h"
#include "wcn_utile.h"
#include "fm_wrapper.h"
#include "gps_wrapper.h"

static int wcn_help(struct wcn_cmd *cmd);
static int wcn_bye(struct wcn_cmd *cmd);
static int wcn_stdsrc_set(struct wcn_cmd *cmd);

static int wcntest_run = 1;

static struct wcn_cmd g_cmd[] = {
    FM_TEST_TBL,
    GPS_TEST_TBL,
    {"setsrc", 2, wcn_stdsrc_set, {NULL, NULL, NULL}, "eg: setsrc in /system/bin/abc.txt, desc: set stdinput"}, 
    {"help", 0, wcn_help, {NULL, NULL, NULL}, "eg: help, desc: show help info"}, 
    {"bye", 0, wcn_bye, {NULL, NULL, NULL}, "eg: bye, desc: exit autofm"}, 
};


static int wcn_help(struct wcn_cmd *cmd)
{
    int id = -1;
    
    printf("-------------------------------------------------------\n");
    printf("%s\t%s\t\t%s\n", "cmd id", "cmd name", "cmd para num");
    printf("-------------------------------------------------------\n");
    while ((++id) < (int)(sizeof(g_cmd)/sizeof(g_cmd[0]))) {
        printf("%d\t%s\t\t%d\t%s\n", id, g_cmd[id].name, g_cmd[id].para_size, g_cmd[id].description);
    }
    printf("-------------------------------------------------------\n");
    printf("OK\n");

    return 0;
}


static int wcn_bye(struct wcn_cmd *cmd)
{
    wcntest_run = 0;
    
    printf("OK\n");
    return 0;
}


static int wcn_stdsrc_set(struct wcn_cmd *cmd)
{
    #define FM_STD_SHELL 0
    #define FM_STD_FILE 1
    int ret;
    int tmp;
    static int fm_stdin = 0;
    static int fm_stdout = 1;
    static int fm_stderr = 2;
    static int input = FM_STD_SHELL;
    static int output = FM_STD_SHELL;
    static int errput = FM_STD_SHELL;
    
    char *path = cmd->para[1];

    if (fm_stdin == 0) {
        fm_stdin = dup(0);
        fm_stdout = dup(1);
        fm_stderr = dup(2);
        printf("backup, stdin:%d stdot:%d stderr:%d\n", fm_stdin, fm_stdout, fm_stderr);
    }

    if (strcmp(cmd->para[0], "in") == 0) {
        if (strcmp(cmd->para[1], "shell") == 0) {
            if (input == FM_STD_FILE) {
                dup2(fm_stdin, 0);
                input = FM_STD_SHELL;
            } else {
                ;//fm already use shell as stdin
            }
        } else if (path != NULL){
            tmp = open(path, O_RDONLY);
            if (tmp < 0) {
                printf("FAIL: open %s err\n", path);
                return -1;
            }
            ret = dup2(tmp, 0);
            if (ret < 0) {
                printf("FAIL: dup %s err\n", path);
                return -1;
            }
            close(tmp);
            input = FM_STD_FILE;
        } else {
            printf("FAIL: path err\n");
            return -1;
        }
    }else if (strcmp(cmd->para[0], "out") == 0) {
        if (strcmp(cmd->para[1], "shell") == 0) {
            if (output == FM_STD_FILE) {
                dup2(fm_stdin, 1);
                output = FM_STD_SHELL;
            } else {
                ;//fm already use shell as stdin
            }
        } else if(path != NULL) {
            tmp = open(path, O_RDWR | O_CREAT, 00660);
            if (tmp < 0) {
                printf("FAIL: open %s err\n", path);
                return -1;
            }
            ret = dup2(tmp, 1);
            if (ret < 0) {
                printf("FAIL: dup %s err\n", path);
                return -1;
            }
            close(tmp);
            output = FM_STD_FILE;
        } else {
            printf("FAIL: path err\n");
            return -1;
        }
    } else if (strcmp(cmd->para[0], "err") == 0){
        if (strcmp(cmd->para[1], "shell") == 0) {
            if (errput == FM_STD_FILE) {
                dup2(fm_stdin, 2);
                errput = FM_STD_SHELL;
            } else {
                ;//fm already use shell as stdin
            }
        } else if (path != NULL){
            tmp = open(path, O_RDWR | O_CREAT, 00660);
            if (tmp < 0) {
                printf("FAIL: open %s err\n", path);
                return -1;
            }
            ret = dup2(tmp, 2);
            if (ret < 0) {
                printf("FAIL: dup %s err\n", path);
                return -1;
            }
            close(tmp);
            errput = FM_STD_FILE;
        } else {
            printf("FAIL: path err\n");
            return -1;
        }
    } else { 
        return -1;
    }
    
    printf("OK:%s\n", cmd->para[0]);

    return 0;
}


static int cmd_parser(char *src, char **new_pos, char **word)
{
    char *p = src;
    char *word_start = NULL;

    enum wcn_cmd_parser_state state = WCN_CMD_STAT_NONE;

    for(p = src; *p != '\0'; p++){
        switch(state){
            case WCN_CMD_STAT_NONE:{
                if(!check_hex_str(p, 1)){
                    //if we get show char in none state, it means a new word start
                    state = WCN_CMD_STAT_WORD;
                    word_start = p;
                }
                break;
            }
            
            case WCN_CMD_STAT_WORD:{
                if(check_hex_str(p, 1)){
                    //if we get non show char in word state, it means a word complete
                    *p = '\0';
                    //FIX_ME
                    //record word
                    state = WCN_CMD_STAT_NONE;
                    trim_string(&word_start);
                    *word = word_start;
                    *new_pos = p + 1;
                    //printf("%s\n", word_start);
                    return 0;
                }
                break;
            }
            default:
                break;
        }
    }
    return -1;
}

static int get_cmd(char *buf, int buf_size)
{
#if 0
    char tmp = 0;

    do{
        tmp = getchar();
        *buf = tmp;
        buf++;
        buf_size--;
    }while((tmp != '\r')  && (tmp != '\n') && (buf_size > 0));
#else
    int ret;
    struct timeval tval;
    tval.tv_sec = 0;
    tval.tv_usec = 100*1000;

    ret = read(0, buf, buf_size);
    //sleep(1); //sleep 1s  
    select(0, NULL, NULL, NULL, &tval);
    
#endif
    return 0;
}


static enum wcn_event_type get_key_type(char *key, struct wcn_cmd **pcmd)
{
    int i;
    
    for (i = 0; i < (int)(sizeof(g_cmd)/sizeof(g_cmd[0])); i++) {
        if (strcmp(g_cmd[i].name, key) == 0) {
            //printf("%s %d\n", g_cmd[i].name, g_cmd[i].para_size);
            *pcmd = &g_cmd[i];
            return WCN_EVENT_CMD;
        }
    }
    return WCN_EVENT_VAL;
}

static int daemonize(int argc, char **argv)
{
    int ret = 0;
    int fd0, fd1, fd2;
    unsigned int i;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    umask(0);

    ret = getrlimit(RLIMIT_NOFILE, &rl);
    if (ret < 0){
        return -1;
    }

    pid = fork();
    if (pid < 0){
        return -1;
    }else if (pid > 0){
        exit(0);
    }

    setsid();

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGHUP, &sa, NULL) < 0){
        return -1;
    }

    pid = fork();
    if (pid < 0){
        return -1;
    }else if (pid > 0){
        exit(0);
    }

    chdir("/");
    
    if (rl.rlim_max == RLIM_INFINITY){
        rl.rlim_max = 1024;    
    }

    for (i = 0; i < (unsigned int)argc; i++){
        printf("%s ", argv[i]);
    }
    printf("\n");

    for (i = 0; i < rl.rlim_max; i++){
        close(i);
    }

    open(argv[1], O_RDWR | O_CREAT, 00660);
    open(argv[2], O_RDWR | O_CREAT, 00660);
    open(argv[3], O_RDWR | O_CREAT, 00660);

    return 0;
}


int main(int argc, char **argv)
{
    int ret = 0;    
    int i;
    char *buf = NULL;
#define BUF_SIZE 1024*16

    buf = malloc(BUF_SIZE);
    if (!buf) {
        printf("fm malloc memory failed!\n");
        return -1;
    }
    
    printf("\n%s:%s:%s\n", argv[0], __DATE__, __TIME__); 
    for (i = 0; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");

#ifdef WCN_TOOL_BUILD_DEAMON
    daemonize(argc, argv);
#endif

    while (wcntest_run) {
        char *p = buf;
        char *key = NULL;
        enum wcn_event_type keytype;
        struct wcn_cmd *cmd = NULL;
        enum wcn_sta sta = WCN_STA_NON;
        
        memset(buf, 0, BUF_SIZE);
        get_cmd(buf, BUF_SIZE); //get a line form input file

        while (cmd_parser(p, &p, &key) == 0) {
            keytype = get_key_type(key, &cmd); //this function will update "cmd" pointer if need
            switch (sta) {
                case WCN_STA_NON:
                    if (keytype == WCN_EVENT_VAL) {
                        printf("bad format\n");
                    } else if (cmd->para_size == 0){
                        //printf("%s\n", cmd->name);
                        cmd->handler(cmd); //yes, do cmd
                    } else {
                        sta = WCN_STA_CMD; //cmd need one or more para
                    }
                    break;
                    
                case WCN_STA_CMD:
                    if (keytype == WCN_EVENT_VAL) {
                        cmd->para[0] = key;
                        if (cmd->para_size == 1) {
                            //printf("%s\n", cmd->name);
                            cmd->handler(cmd); //yes, do cmd
                            sta = WCN_STA_NON;
                        } else {
                            sta = WCN_STA_VAL_1;
                        }
                    } else {
                        printf("bad format\n");
                        if (cmd->para_size == 0) {
                            //printf("%s\n", cmd->name);
                            cmd->handler(cmd); //yes, do cmd 
                            sta = WCN_STA_NON;
                        } else {
                            ;
                        }
                    }
                    break;
                    
                case WCN_STA_VAL_1:
                    if (keytype == WCN_EVENT_VAL) {
                        cmd->para[1] = key;
                        if (cmd->para_size == 2) {
                            //printf("%s\n", cmd->name);
                            cmd->handler(cmd); 
                            sta = WCN_STA_NON;
                        } else {
                            sta = WCN_STA_VAL_2;
                        }
                    } else {
                        printf("bad format\n");
                        if (cmd->para_size == 0) {
                            //printf("%s\n", cmd->name);
                            cmd->handler(cmd); //yes, do cmd 
                            sta = WCN_STA_NON;
                        } else {
                            sta = WCN_STA_CMD;
                        }
                    }
                    break;
                    
                case WCN_STA_VAL_2:
                    if (keytype == WCN_EVENT_VAL) {
                        cmd->para[2] = key;
                        //printf("%s\n", cmd->name);
                        cmd->handler(cmd); //yes, do cmd
                        sta = WCN_STA_NON;
                    } else {
                        printf("bad format\n");
                        if (cmd->para_size == 0) {
                            //printf("%s\n", cmd->name);
                            cmd->handler(cmd); //yes, do cmd 
                            sta = WCN_STA_NON;
                        } else {
                            sta = WCN_STA_CMD;
                        }
                    }
                default:
                    break;
            }
        }
    }

    return 0;
}

