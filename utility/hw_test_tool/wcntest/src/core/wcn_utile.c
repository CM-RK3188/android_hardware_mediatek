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

int to_upper(char *str)
{
    int i = 0;

    for(i=0; i < (int)strlen(str); i++){
        if(('a' <= str[i]) && (str[i] <= 'z')){
            str[i] = str[i] - ('a' - 'A');
        }
    }
    return 0;
}

int to_upper_n(char *str, int len)
{
    int i = 0;

    for(i=0; i < len; i++){
        if(('a' <= str[i]) && (str[i] <= 'z')){
            str[i] = str[i] - ('a' - 'A');
        }
    }
    return 0;
}

#if 0
int check_hex_str(char *str, int len)
{
    int i = 0;

    for(i=0; i < len; i++){
        if((('a' <= str[i]) && (str[i] <= 'z')) || (('A' <= str[i]) && (str[i] <= 'Z')) || (('0' <= str[i]) && (str[i] <= '9'))){
            ;
        }else{
            return -1;
        }
    }
    return 0;
}
#else
int check_hex_str(char *str, int len)
{
    int i = 0;

    for(i=0; i < len; i++){
        if((('!' <= str[i]) && (str[i] <= '~'))){
            ;
        }else{
            return -1;
        }
    }
    return 0;
}
#endif

int check_dec_str(char *str, int len)
{
    int i = 0;

    for(i=0; i < len; i++){
        if(('0' <= str[i]) && (str[i] <= '9')){
            ;
        }else{
            return -1;
        }
    }
    return 0;
}

int ascii_to_hex(char *in_ascii, uint16_t *out_hex)
{
    int len = (int)strlen(in_ascii);
    int    i = 0;
    uint16_t tmp;

    len = (len > 4)? 4 : len;
    if(check_hex_str(in_ascii, len)){
        return -1;
    }
    to_upper_n(in_ascii, len);
    *out_hex = 0;
    for(i=0; i < len; i++){
        if(in_ascii[len-i-1] < 'A'){
            tmp = in_ascii[len-i-1];
            *out_hex |= ((tmp - '0') << (4*i));
        }else{
            tmp = in_ascii[len-i-1];
            *out_hex |= ((tmp - 'A' + 10) << (4*i));
        }
    }
    return 0;
}

int ascii_to_dec(char *in_ascii, int *out_dec)
{
    int len = (int)strlen(in_ascii);
    int i = 0;
    int flag;
    int multi = 1;

    len = (len > 10)? 10 : len;
    if(in_ascii[0] == '-'){
        flag = -1;
        in_ascii += 1;
        len -= 1;
    }else{
        flag = 1;
    }
    if(check_dec_str(in_ascii, len)){
        return -1;
    }
    *out_dec = 0;
    multi = 1;
    for(i=0; i < len; i++){
        *out_dec += ((in_ascii[len-i-1] - '0') * multi);
        multi *= 10;
    }
    *out_dec *= flag;
    return 0;
}

int trim_string(char **start)
{
    char *end = *start;

    /* Advance to non-space character */
    while(*(*start) == ' '){
        (*start)++;
    }

    /* Move to end of string */
    while(*end != '\0'){
        (end)++;
    }

    /* Backup to non-space character */
    do{
        end--;
    }while((end >= *start) && (*end == ' '));

    /* Terminate string after last non-space character */
    *(++end) = '\0';
    return (end - *start);
}

int trim_path(char **start)
{
    char *end = *start;

    while(*(*start) == ' '){
        (*start)++;
    }

    while(*end != '\0'){
        (end)++;
    }
    
    do{
        end--;
    }while((end >= *start) && ((*end == ' ') || (*end == '\n') || (*end == '\r')));

    *(++end) = '\0';
    return (end - *start);
}

int get_int_val(char *src_val, int *dst_val)
{
    int ret = 0;
    uint16_t tmp_hex;
    int tmp_dec;

    //printf("%s\n", src_val);
    if(memcmp(src_val, "0x", strlen("0x")) == 0){
        src_val += strlen("0x");
        ret = ascii_to_hex(src_val, &tmp_hex);
        if(!ret){
            *dst_val = (int)tmp_hex;
            return 0;
        }else{
            printf("error\n");
            return -1;
        }
    }else{
        ret = ascii_to_dec(src_val, &tmp_dec);
        if(!ret && ((0 <= tmp_dec) && (tmp_dec <= 0xFFFF))){
            *dst_val = tmp_dec;
            return 0;
        }else{
            printf("error\n");
            return -1;
        }
    }
}

