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
#include "fm_wrapper.h"
#include "wcn_utile.h"
#include "libfm.h"



//fm wrapper APIs
int fmw_open_dev(struct wcn_cmd *cmd)
{
    return fm_open_dev(NULL);
}

int fmw_close_dev(struct wcn_cmd *cmd)
{
    return fm_close_dev();
}

int fmw_power_up(struct wcn_cmd *cmd)
{
    return fm_power_up(900);
}

int fmw_power_up_tx(struct wcn_cmd *cmd)
{
    return fm_power_up_tx(1000);
}

int fmw_power_down(struct wcn_cmd *cmd)
{
    return fm_power_down();
}

int fmw_rds_on_off(struct wcn_cmd *cmd)
{
    int on_off;

    if (strcmp(cmd->para[0], "on") == 0) {
        on_off = 1; 
    } else if (strcmp(cmd->para[0], "off") == 0){
        on_off = 0;
    } else {
        return -1;
    }

    return fm_rds_on_off(on_off);
}

int fmw_tune(struct wcn_cmd *cmd)
{
    int ret;
    int freq;

    ret = get_int_val(cmd->para[0], &freq);
    if (ret) {
        return -1;
    }
    
    return fm_tune(freq);
}


int fmw_tune_tx(struct wcn_cmd *cmd)
{
    int ret;
    int freq;

    ret = get_int_val(cmd->para[0], &freq);
    if (ret) {
        return -1;
    }

    return fm_tune_tx(freq);
}


int fmw_rds_tx(struct wcn_cmd *cmd)
{
    int ret;
    int pi;
    
    ret = get_int_val(cmd->para[0], &pi);
    if (ret) {
        return -1;
    }

    return fm_rds_tx(pi, 0, NULL);
}


int fmw_seek(struct wcn_cmd *cmd)
{
    int dir;

    if (strcmp(cmd->para[0], "up") == 0) {
        dir = 0; 
    } else if (strcmp(cmd->para[0], "down") == 0){
        dir = 1;
    } else {
        return -1;
    }

    return fm_seek(dir);
}


int fmw_scan(struct wcn_cmd *cmd)
{
    return fm_scan();
}


int fmw_jammer_scan(struct wcn_cmd *cmd)
{
    return fm_jammer_scan();
}


int fmw_getcqi(struct wcn_cmd *cmd)
{
    int ret;
    int tmp;

    ret = get_int_val(cmd->para[0], &tmp);
    if (ret) {
        return -1;
    }
    
    return fm_getcqi(tmp);
}


int fmw_scan_force_stop(struct wcn_cmd *cmd)
{
    return fm_scan_force_stop();
}


//scan test.
int fmw_seek_all(struct wcn_cmd *cmd)
{
    return fm_seek_all();
}


int fmw_mute(struct wcn_cmd *cmd)
{
    return fm_mute();
}


int fmw_unmute(struct wcn_cmd *cmd)
{
    return fm_unmute();
}


int fmw_setvol(struct wcn_cmd *cmd)
{
    int ret;
    int tmp;

    ret = get_int_val(cmd->para[0], &tmp);
    if (ret) {
        return -1;
    }
    
    return fm_setvol(tmp);
}


int fmw_getvol(struct wcn_cmd *cmd)
{
    return fm_getvol();
}


int fmw_getrssi(struct wcn_cmd *cmd)
{
   int *pRssi;

   return fm_getrssi(pRssi);
}


int fmw_switchana(struct wcn_cmd *cmd) 
{
    int ana;

    if (strcmp(cmd->para[0], "long") == 0) {
        ana = 0; 
    } else if (strcmp(cmd->para[0], "short") == 0){
        ana = 1;
    } else {
        return -1;
    }

    return fm_switchana(ana);
}


int fmw_32kppm_compensation(struct wcn_cmd *cmd)
{
    int ret;
    int age;
    int drift;
    
    ret = get_int_val(cmd->para[0], &age);
    if (ret) {
        return -1;
    }

    ret = get_int_val(cmd->para[1], &drift);
    if (ret) {
        return -1;
    }

    return fm_32kppm_compensation(age, drift);
}


int fmw_i2s_set(struct wcn_cmd *cmd)
{
    int onoff;
    int mode;
    int sample;
    

    if (strcmp(cmd->para[0], "on") == 0) {
        onoff = 1; 
    } else if (strcmp(cmd->para[0], "off") == 0){
        onoff = 0;
    } else {
        return -1;
    }

    if (strcmp(cmd->para[1], "master") == 0) {
        mode = 1; 
    } else if (strcmp(cmd->para[1], "slave") == 0){
        mode = 0;
    } else {
        return -1;
    }

    if (strcmp(cmd->para[2], "32k") == 0) {
        sample = 0; 
    } else if (strcmp(cmd->para[2], "44k") == 0){
        sample = 1;
    } else if (strcmp(cmd->para[2], "48k") == 0){
        sample = 2;
    } else {
        return -1;
    }

    return fm_i2s_set(onoff, mode, sample);
}


int fmw_over_bt(struct wcn_cmd *cmd)
{
    int viabt;

    if (strcmp(cmd->para[0], "on") == 0) {
        viabt = 1; 
    } else if (strcmp(cmd->para[0], "off") == 0){
        viabt = 0;
    } else {
        return -1;
    }
    
    return fm_over_bt(viabt);
}

int fmw_read_reg(struct wcn_cmd *cmd)
{
    int ret;
    int addr;

    ret = get_int_val(cmd->para[0], &addr);
    if (ret) {
        return -1;
    }

    return fm_read_reg(addr);
}


int fmw_print_reg(struct wcn_cmd *cmd)
{
    int out = -1;

    if (strcmp(cmd->para[0], "shell") == 0) {
        out = 0; 
    } else if (strcmp(cmd->para[0], "file") == 0){
        out = 1;
    } else {
        return -1;
    }
    
    return fm_print_reg(out);
}


int fmw_write_reg(struct wcn_cmd *cmd)
{
    int ret;
    int addr;
    int val;

    ret = get_int_val(cmd->para[0], &addr);
    if (ret) {
        return -1;
    }

    ret = get_int_val(cmd->para[1], &val);
    if (ret) {
        return -1;
    }

    return fm_write_reg((uint8_t)addr, (uint16_t)val);
}


/*
 * fm_mod_reg
 * mod addr mask val -> mod 0x60 0xFFFE 0x0001
 */
int fmw_mod_reg(struct wcn_cmd *cmd)
{
    int ret;
    int addr;
    int mask;
    int val;

    ret = get_int_val(cmd->para[0], &addr); //get addr
    if (ret) {
        return -1;
    }
        
    ret = get_int_val(cmd->para[1], &mask); //get mask
    if (ret) { 
        return -1;
    }

    ret = get_int_val(cmd->para[2], &val); //get bits
    if (ret) {
        return -1;
    }
    
    return fm_mod_reg(addr, val, mask);
}


int fmw_delay(struct wcn_cmd *cmd)
{
    int ret = 0;
    int tmp;
    int sec;
    int microsec;
    
    ret = get_int_val(cmd->para[0], &tmp);
    if (ret) {
        return -1;
    }

    if (strcmp(cmd->para[1], "s") == 0) {
        sec = tmp;
        microsec = 0;
    } else if (strcmp(cmd->para[1], "ms") == 0){
        sec = 0;
        microsec = tmp*1000;
    } else if (strcmp(cmd->para[1], "us") == 0){
        sec = 0;
        microsec = tmp;
    } else {
        return -1;
    }

    return fm_delay(sec, microsec);
}

