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
 * Qiuhuan Zhao <qiuhuan.zhao@mediatek.com>
 *
 */

//for gps auto test and debug

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
#include "gps_wrapper.h"
#include "wcn_utile.h"
#include "libgps.h"

typedef enum {
    GPS_HOT_RESTART = 0,
    GPS_WARM_RESTART,
    GPS_COLD_RESTART,
    GPS_FULL_RESTART
}GPS_RESTART;

int gps_open()
{
    int res;
    res = ENG_GPS_Open();
    if (!res){
        printf("Open gps sucess\n");
        return 0;
    }else{
        printf("Open gps fail\n");
        return -1;
    }		
}

int gps_hstart()
{
    int res;
    res = ENG_GPS_Restart(GPS_HOT_RESTART);
    if (!res){
        printf("Hstart gps sucess\n");
        return 0;
    }else{
        printf("Hstart gps fail\n");
        return -1;
    }
}

int gps_wstart()
{
    int res;
    res = ENG_GPS_Restart(GPS_WARM_RESTART);
    if (!res){
        printf("Wstart gps sucess\n");
        return 0;
    }else{
        printf("Wstart gps fail\n");
        return -1;
    }
}

int gps_cstart()
{   
    int res;
    res = ENG_GPS_Restart(GPS_COLD_RESTART);
    if (!res){
        printf("Cstart gps sucess\n");
        return 0;
    }else{
        printf("Cstart gps fail\n");
        return -1;
    }
}

int gps_fstart()
{
    int res;
    res = ENG_GPS_Restart(GPS_FULL_RESTART);
    if (!res){
        printf("Fstart gps sucess\n");
        return 0;
    }else{
        printf("Fstart gps fail\n");
        return -1;
    }
}

int gps_close()
{
    int res;
    res = ENG_GPS_Close();
    if (!res){
        printf("Close gps sucess\n");
        return 0;
    }else{
        printf("Close gps fail\n");
        return -1;
    }
}

int gps_getver()
{
    int res;
    res = ENG_GPS_Getver();
    if (!res){
        //printf("Get gps version sucess\n");
        return 0;
    }else{
        //printf("Get gps version fail\n");
        return -1;
    }
	
}
