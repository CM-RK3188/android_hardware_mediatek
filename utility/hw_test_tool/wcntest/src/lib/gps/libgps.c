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
 * Qiuhuan Zhao <Qiuhuan.Zhao@MediaTek.com>
 *
 */
#include <stdio.h>   
#include <string.h>  
#include <unistd.h>  
#include <fcntl.h>   
#include <errno.h>   
#include <termios.h> 
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cutils/log.h> 
#include <sys/stat.h>

#define C_INVALID_PID  (-1)   /*invalid process id*/
#define C_INVALID_TID  (-1)   /*invalid thread id*/
#define C_INVALID_FD   (-1)   /*invalid file handle*/
#define C_INVALID_SOCKET (-1) /*invalid socket id*/

#define TAG             "[GPS] "
#define INFO_SIZE       1024
#define NMEA_SIZE       10240
    
#define NUM_CH  (20)
#define PSEUDO_CH (32)
#define Knot2Kmhr (1.8532)

char ptr[INFO_SIZE];
typedef enum {
    GPS_HOT_RESTART = 0,
    GPS_WARM_RESTART,
    GPS_COLD_RESTART,
    GPS_FULL_RESTART
}GPS_RESTART;

typedef struct SVInfo
{
    int SVid;            // PRN
    int SNR;
    int elv;             // elevation angle : 0~90
    int azimuth;         // azimuth : 0~360
    unsigned char Fix;   // 0:None , 1:FixSV
} SVInfo;

typedef struct ChInfo
{
    int SVid;            // PRN
    int SNR;             // SNR
    unsigned char Status;// Status(0:Idle, 1:Search, 2:Tracking)
} ChInfo;


unsigned char nmea_buf[NMEA_SIZE];
SVInfo  g_svInfo[NUM_CH];
ChInfo  g_chInfo[PSEUDO_CH];


pid_t mnl_pid = C_INVALID_PID;
int sockfd = C_INVALID_SOCKET;
pthread_t gps_eng_thread_handle = C_INVALID_TID;

typedef struct GPSInfo
{
    int year;
    int mon;
    int day;
    int hour;
    int min;
    float sec;

    float Lat; // Position, +:E,N -:W,S
    float Lon;
    float Alt;
    unsigned char FixService;  // NoFix:0, SPS:1, DGPS:2, Estimate:6
    unsigned char FixType;     // None:0, 2D:1, 3D:2
    float Speed;  // km/hr
    float Track;  // 0~360
    float PDOP;   //DOP
    float HDOP;
    float VDOP;

    int SV_cnt;
    int fixSV[NUM_CH];
}GPSInfo;

GPSInfo g_gpsInfo;

int ttff = 0;
int fixed = 0;

char CHIP_VERS[40];

static int mnl_write_attr(const char *name, unsigned char attr) 
{
    int err, fd = open(name, O_RDWR);
    char buf[] = {attr + '0'};
    
    if (fd == -1) {
        ALOGD("open %s err = %s\n", name, strerror(errno));
        return -errno;
    }
    do { err = write(fd, buf, sizeof(buf) ); }
    while (err < 0 && errno == EINTR);
    
    if (err != sizeof(buf)) { 
        ALOGD("write fails = %s\n", strerror(errno));
        err = -errno;
    } else {
        err = 0;    /*no error*/
    }
    if (close(fd) == -1) {
        ALOGD("close fails = %s\n", strerror(errno));
        err = (err) ? (err) : (-errno);
    }
    ALOGD("write '%d' to %s okay\n", attr, name);    
    return err;
}

unsigned char CheckSum(char *buf, int size)
{
   int i;
   char chksum=0, chksum2=0;

   if(size < 5)
      return 0;

   chksum = buf[1];
   for(i = 2; i < (size - 2); i++)
   {
      if(buf[i] != '*')
      {
        chksum ^= buf[i];
      }
      else
      {
        if(buf[i + 1] >= 'A')
        {
          chksum2 = (buf[i+1]-'A'+10)<<4;
        }
        else
        {
          chksum2 = (buf[i+1]-'0')<<4;
        }

        if(buf[i + 2] >= 'A')
        {
          chksum2 += buf[i+2]-'A'+10;
        }
        else
        {
          chksum2 += buf[i+2]-'0';
        }
        break;
      }
    }

   /* if not found character '*' */
   if(i == (size - 2))
   {
      return (0);
   }

   if(chksum == chksum2)
   {
     return (1);
   }
   else
   {
     return (0);
   }
}

int FetchField(char *start, char *result)
{
   char *end;

   if(start == NULL)
      return 0;

   end = strstr( start, ",");
   // the end of sentence
   if(end == NULL)
      end = strstr(start, "*");

   if(end-start>0)
   {
     strncpy( result, start, end-start);
     result[end-start]='\0';
   }
   else   // no data
   {
     result[0]='\0';
     return 0;
   }

   return 1;
}
void GLL_Parse( char *head)
{
   // $GPGLL,2446.367638,N,12101.356226,E,144437.000,A,A*56
   char *start, result[20], tmp[20], *point;
   int len=0;
   char FixService;

   // check checksum
   if(CheckSum(head, strlen(head)))
   {
      // Position(Lat)
      start = strstr( head, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField(start, result))
      {
         point = strstr( result, ".");
         len = (point-2)-result;
         strncpy(tmp, result, len);
         tmp[len]='\0';
         g_gpsInfo.Lat = (float)(atof(tmp));
         strncpy(tmp, result+len, strlen(result)-len);
         tmp[strlen(result)-len]='\0';
         g_gpsInfo.Lat += (float)(atof(tmp)/60.0);
      }

      // N or S
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField(start, result))
      {
         if(*result=='S')
            g_gpsInfo.Lat = -g_gpsInfo.Lat;
      }

      // Position(Lon)
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField(start, result))
      {
         point = strstr( result, ".");
         len = (point-2)-result;
         strncpy(tmp, result, len);
         tmp[len]='\0';
         g_gpsInfo.Lon = (float)(atof(tmp));
         strncpy(tmp, result+len, strlen(result)-len);
         tmp[strlen(result)-len]='\0';
         g_gpsInfo.Lon += (float)(atof(tmp)/60.0);
      }

      // E or W
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
      {
         if(*result=='W')
            g_gpsInfo.Lon = -g_gpsInfo.Lon;
      }

      // UTC Time
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
      {
         // Hour
         strncpy( tmp, result, 2);
         tmp[2]='\0';
         g_gpsInfo.hour = atoi(tmp);
         // Min
         strncpy( tmp, result+2, 2);
         tmp[2]='\0';
         g_gpsInfo.min = atoi(tmp);
         // Sec
         strncpy( tmp, result+4, strlen(result)-4);
         tmp[strlen(result)-4]='\0';
         g_gpsInfo.sec = (float)(atof(tmp));
      }   

      // The positioning system Mode Indicator and Status fields shall not be null fields.
      // Data valid
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;

      if(!FetchField( start, result))
         return;
      
      if(*result=='A')
      {
         // Fix Type
         if(g_gpsInfo.FixType == 0)
            g_gpsInfo.FixType = 1;   // Assume 2D, if there's no other info.

         // Fix Service
         start = strstr( start, ",");
         if(start != NULL)
             start = start +1;
         else
            return;

         if(!FetchField( start, result))
            return;

         FixService = *result;

         switch(FixService)
         {
            case 'A':
            {
               g_gpsInfo.FixService = 1;
               break;
            }
            case 'D':
            {
               g_gpsInfo.FixService = 2;
               break;
            }
            case 'E':
            {
               g_gpsInfo.FixService = 6;
               break;
            }
         }
      }
      else // Data invalid
      {
         g_gpsInfo.FixType = 0;    // NoFix
         g_gpsInfo.FixService = 0; // NoFix
      }
   }
}
//---------------------------------------------------------------------------
void RMC_Parse( char *head)
{
   // $GPRMC,073446.000,A,2446.3752,N,12101.3708,E,0.002,22.08,121006,,,A*6C

   char *start, result[20], tmp[20], *point;
   int len=0;

   // check checksum
   if(CheckSum(head, strlen(head)))
   {
      // UTC time : 161229.487
      start = strstr( head, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, tmp))
      {
         // Hour
         strncpy( result, tmp, 2);
         result[2]='\0';
         g_gpsInfo.hour = atoi(result);
         // Min
         strncpy( result, tmp+2, 2);
         result[2]='\0';
         g_gpsInfo.min = atoi(result);
         // Sec
         strncpy( result, tmp+4, strlen(tmp)-4);
         result[strlen(tmp)-4]='\0';
         g_gpsInfo.sec = (float)(atof(result));
      }

      // valid
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(!FetchField( start, result))
         return;

      if(*result == 'A')
      {
         if(g_gpsInfo.FixType == 0)
            g_gpsInfo.FixType = 1;      // Assume 2D

         if(g_gpsInfo.FixService == 0)
            g_gpsInfo.FixService = 1;   // Assume SPS
      }
      else
      {
         g_gpsInfo.FixType = 0;    // NoFix
         g_gpsInfo.FixService = 0; // NoFix
      }

      // Position(Lat) : 3723.2475(N)
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
      {
         point = strstr( result, ".");
         len = (point-2)-result;
         strncpy(tmp, result, len);
         tmp[len]='\0';
         g_gpsInfo.Lat = (float)(atoi(tmp));
         strncpy(tmp, result+len, strlen(result)-len);
         tmp[strlen(result)-len]='\0';
         g_gpsInfo.Lat += (float)(atof(tmp)/60.0);
      }
	  else  //Can not fetch Lat field
	  {
	     g_gpsInfo.Lat = 0;
	  }

      // N or S
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result) && g_gpsInfo.Lat!=0)
      {
         if(*result=='S')
            g_gpsInfo.Lat = -g_gpsInfo.Lat;
      }

      // Position(Lon) : 12158.3416(W)
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
      {
         point = strstr( result, ".");
         len = (point-2)-result;
         strncpy(tmp, result, len);
         tmp[len]='\0';
         g_gpsInfo.Lon = (float)(atoi(tmp));
         strncpy(tmp, result+len, strlen(result)-len);
         tmp[strlen(result)-len]='\0';
         g_gpsInfo.Lon += (float)(atof(tmp)/60.0);
      }
	  else  //Can not fetch Lat field
	  {
	     g_gpsInfo.Lon = 0;
	  }

      // E or W
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result) && g_gpsInfo.Lat!=0)
      {
         if(*result=='W')
            g_gpsInfo.Lon = -g_gpsInfo.Lon;
      }

      // Speed : 0.13
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
      {
         g_gpsInfo.Speed = (float)(atof(result) * Knot2Kmhr);
      }

      // Track : 309.62
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
      {
         g_gpsInfo.Track = (float)(atof(result));
      }

      // Date : 120598
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
      {
         // Day
         strncpy(tmp, result, 2);
         tmp[2]='\0';
         g_gpsInfo.day=atoi(tmp);

         // Month
         strncpy(tmp, result+2, 2);
         tmp[2]='\0';
         g_gpsInfo.mon=atoi(tmp);

         // Year
         strncpy(tmp, result+4, 2);
         tmp[2]='\0';
         g_gpsInfo.year=atoi(tmp)+2000;
      }

      // skip Magnetic variation
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;

      // mode indicator
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;

      if(!FetchField( start, result))
         return;

      if(g_gpsInfo.FixType > 0)
      {
         switch(result[0])
         {
            case 'A':
            {
               g_gpsInfo.FixService = 1;
               break;
            }
            case 'D':
            {
               g_gpsInfo.FixService = 2;
               break;
            }
            case 'E':
            {
               g_gpsInfo.FixService = 6;
               break;
            }
         }
      }
   }
}
//---------------------------------------------------------------------------
void VTG_Parse( char *head)
{
   //$GPVTG,159.16,T,,M,0.013,N,0.023,K,A*34
   char *start, result[20];
   char FixService;

   // check checksum
   if(CheckSum(head, strlen(head)))
   {
      // Track
      start = strstr( head, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
      {
         g_gpsInfo.Track = (float)(atof(result));
      }

      // ignore
      start = strstr( start, ",");     // T
      if(start != NULL)
         start = start +1;
      else
         return;

      start = strstr( start, ",");     // NULL
      if(start != NULL)
         start = start +1;
      else
         return;

      start = strstr( start, ",");     // M
      if(start != NULL)
         start = start +1;
      else
         return;

      // Speed
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
      {
         g_gpsInfo.Speed = (float)(atof(result) * Knot2Kmhr);
      }

      // ignore
      start = strstr( start, ",");     // N
      if(start != NULL)
         start = start +1;
      else
         return;

      start = strstr( start, ",");     // 0.023
      if(start != NULL)
         start = start +1;
      else
         return;

      start = strstr( start, ",");     // K
      if(start != NULL)
         start = start +1;
      else
         return;

      // Fix Service
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;

      if(!FetchField( start, result))
         return;
      FixService = *result;

      if(FixService != 'N')
      {
         if(g_gpsInfo.FixType == 0)
            g_gpsInfo.FixType = 1;  //Assume 2D

         switch(FixService)
         {
            case 'A':
            {
               g_gpsInfo.FixService = 1;
               break;
            }
            case 'D':
            {
               g_gpsInfo.FixService = 2;
               break;
            }
            case 'E':
            {
               g_gpsInfo.FixService = 6;
               break;
            }
         }
      }
      else // NoFix
      {
         g_gpsInfo.FixType = 0;    // NoFix
         g_gpsInfo.FixService = 0; // NoFix
      }
   }
}
//---------------------------------------------------------------------------
void GSA_Parse( char *head)
{
   // $GPGSA,A,3,03,19,27,23,13,16,15,11,07,,,,1.63,0.95,1.32*03
   char *start, result[20];
   int sv_cnt=0, i;

   if(CheckSum(head, strlen(head)))
   {
      //Fix SV
      memset(&g_gpsInfo.fixSV, 0, sizeof(g_gpsInfo.fixSV));

      //Valid
      start = strstr( head, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(!FetchField( start, result))
         return;

      if((*result == 'A') || (*result == 'M'))
      {
         // Fix Type
         start = strstr( start, ",");
         if(start != NULL)
            start = start +1;
         else
            return;
         if(!FetchField( start, result))
            return;
         g_gpsInfo.FixType = atoi(result)-1;

         if(g_gpsInfo.FixType > 0)          // Fix
         {
            if(g_gpsInfo.FixService == 0)
               g_gpsInfo.FixService = 1;    //Assume SPS FixSerivce
         }
         else
         {
            g_gpsInfo.FixType = 0;    // NoFix
            g_gpsInfo.FixService = 0; // NoFix
         }
      }
      else
      {
         g_gpsInfo.FixType = 0;    // NoFix
         g_gpsInfo.FixService = 0; // NoFix
      }

      for(i=0 ; i<12 ; i++)
      {
         start = strstr( start, ",");
         if(start != NULL)
            start = start +1;
         else
            return;

         FetchField( start, result);

         if(strlen(result)>0)
            g_gpsInfo.fixSV[sv_cnt++] = atoi(result);
      }

      //PDOP
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(!FetchField( start, result))
         return;
      g_gpsInfo.PDOP = (float)(atof(result));

      //HDOP
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(!FetchField( start, result))
         return;
      g_gpsInfo.HDOP = (float)(atof(result));

      //VDOP
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(!FetchField( start, result))
         return;
      g_gpsInfo.VDOP = (float)(atof(result));
   }
}
//---------------------------------------------------------------------------
void GSV_Parse( char *head)
{
   // $GPGSV,3,1,09,03,63,020,43,19,76,257,37,27,14,320,30,23,39,228,37*79
   // $GPGSV,3,2,09,13,38,274,38,16,31,058,37,15,16,055,34,11,16,192,32*76
   // $GPGSV,3,3,09,07,15,043,26*40

   char *start, result[20];
   int sv_cnt=0, base, i;

   // check checksum
   if(CheckSum(head, strlen(head)))
   {
      // ignore
      start = strstr( head, ",");
      if(start != NULL)
         start = start +1;
      else
         return;

      //first Message
      if(*(start+2)=='1')
      {
         memset( &g_svInfo, 0, sizeof(g_svInfo));
         //g_fgSVUpdate = false;
      }

      // Last Message
      //if(*start == *(start+2))
      //   g_fgSVUpdate = true;

      //base  //sentence number.
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(!FetchField( start, result))
         return;
      base = (atoi(result)-1)*4;

      //total
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(!FetchField( start, result))
         return;
      g_gpsInfo.SV_cnt = atoi(result);
      if(g_gpsInfo.SV_cnt == 0)
      {
         return;
      }

      for( i=0 ; i<4 ; i++)
      {
         //SVid
         start = strstr( start, ",");
         if(start != NULL)
            start = start +1;
         else
            return;
         FetchField( start, result);
         if(strlen(result)>0)
            g_svInfo[base+sv_cnt].SVid = atoi(result);
         else
            g_svInfo[base+sv_cnt].SVid = 0;

         //elev
         start = strstr( start, ",");
         if(start != NULL)
            start = start +1;
         else
            return;
         FetchField( start, result);
         if(strlen(result)>0)
            g_svInfo[base+sv_cnt].elv = atoi(result);
         else
            g_svInfo[base+sv_cnt].elv = 0;

         //azimuth
         start = strstr( start, ",");
         if(start != NULL)
            start = start +1;
         else
            return;
         FetchField( start, result);
         if(strlen(result)>0)
            g_svInfo[base+sv_cnt].azimuth = atoi(result);
         else
            g_svInfo[base+sv_cnt].azimuth = 0;

         //SNR
         start = strstr( start, ",");
         if(start != NULL)
            start = start +1;
         else
            return;
         if(*start == '*')
            g_svInfo[base+sv_cnt].SNR = 0;
         else
         {
            FetchField( start, result);
            if(strlen(result)>0)
               g_svInfo[base+sv_cnt].SNR = atoi(result);
            else
               g_svInfo[base+sv_cnt].SNR = 0;
         }

         sv_cnt++;

         if(base+sv_cnt == g_gpsInfo.SV_cnt)
            break;
      }
   }
}
//---------------------------------------------------------------------------
void GGA_Parse( char *head)
{
   //$GPGGA,144437.000,2446.367638,N,12101.356226,E,1,9,0.95,155.696,M,15.057,M,,*58
   char *start, result[20], tmp[20], *point;
   int len=0;

   // check checksum
   if(CheckSum(head, strlen(head)))
   {
      // UTC time : 144437.000
      start = strstr( head, ",");
      if(start != NULL)
         start = start +1;
      else
         return;

      if(FetchField( start, result))
      {
         // Hour
         strncpy( tmp, result, 2);
         tmp[2]='\0';
         g_gpsInfo.hour = atoi(tmp);
         // Min
         strncpy( tmp, result+2, 2);
         tmp[2]='\0';
         g_gpsInfo.min = atoi(tmp);
         // Sec
         strncpy( tmp, result+4, strlen(result)-4);
         tmp[strlen(result)-4]='\0';
         g_gpsInfo.sec = (float)(atof(tmp));
      }

      // Position(Lat)
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
      {
         point = strstr( result, ".");
         len = (point-2)-result;
         strncpy(tmp, result, len);
         tmp[len]='\0';
         g_gpsInfo.Lat = (float)(atoi(tmp));
         strncpy(tmp, result+len, strlen(result)-len);
         tmp[strlen(result)-len]='\0';
         g_gpsInfo.Lat += (float)(atof(tmp)/60.0);
      }

      // N or S
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
      {
         if(*result=='S')
            g_gpsInfo.Lat = -g_gpsInfo.Lat;
      }

      // Position(Lon)
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
      {
         point = strstr( result, ".");
         len = (point-2)-result;
         strncpy(tmp, result, len);
         tmp[len]='\0';
         g_gpsInfo.Lon = (float)(atoi(tmp));
         strncpy(tmp, result+len, strlen(result)-len);
         tmp[strlen(result)-len]='\0';
         g_gpsInfo.Lon += (float)(atof(tmp)/60.0);
      }

      // E or W
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
      {
         if(*result=='W')
            g_gpsInfo.Lon = -g_gpsInfo.Lon;
      }

      //GPS Fix Type and Service
      // 0: NoFix, 1:SPS, 2:DGPS, 6:Estimate
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(!FetchField( start, result))
         return;
      g_gpsInfo.FixService = atoi(result);

      // Fix
      if(g_gpsInfo.FixService > 0)
      {
         if(g_gpsInfo.FixType == 0)
            g_gpsInfo.FixType = 1; // Assume 2D
      }

      start = strstr( start, ",");   // Number of SV in use , ex :9
      if(start != NULL)
         start = start +1;
      else
         return;

      // HDOP
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
      {
         g_gpsInfo.HDOP = (float)(atof(result));
      }

      //Altitude (mean sea level)
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
      {
         g_gpsInfo.Alt = (float)(atof(result));
      }

      //Altitude unit (bypass)
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;

      //Altitude (Geoidal separation)
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      
      if(FetchField( start, result))
      {
         g_gpsInfo.Alt += (float)(atof(result));
      }   
   }
}
//---------------------------------------------------------------------------
void ZDA_Parse( char *head)
{
   // $GPZDA,000007.123,06,01,2000,,*50
   char *start, result[20], tmp[20];

   // check checksum
   if(CheckSum(head, strlen(head)))
   {
      // UTC time : 000007.123
      start = strstr( head, ",");
      if(start != NULL)
         start = start +1;
      else
         return;

      if(FetchField( start, result))
      {
         // Hour
         strncpy( tmp, result, 2);
         tmp[2]='\0';
         g_gpsInfo.hour = atoi(tmp);

         // Min
         strncpy( tmp, result+2, 2);
         tmp[2]='\0';
         g_gpsInfo.min = atoi(tmp);

         // Sec
         strncpy( tmp, result+4, strlen(result)-4);
         tmp[strlen(result)-4]='\0';
         g_gpsInfo.sec = (float)(atof(tmp));
      }

      // Day
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
         g_gpsInfo.day = atoi(result);

      // Month
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
         g_gpsInfo.mon = atoi(result);

      // Year
      start = strstr( start, ",");
      if(start != NULL)
         start = start +1;
      else
         return;
      if(FetchField( start, result))
         g_gpsInfo.year = atoi(result);
   }
}


void NMEA_Parse(char *TempBuf)
{
    char *GP;
    char type[4];
    //char TempBuf[500];
    //strcpy(TempBuf,g_NMEAbuf);
    GP=strtok(TempBuf, "\r\n");

    memset(&g_gpsInfo, 0, sizeof(g_gpsInfo));
    memset(g_svInfo, 0, (sizeof(SVInfo)*NUM_CH));
    memset(g_chInfo, 0, (sizeof(ChInfo)*PSEUDO_CH));

    if(GP == NULL)
        return;

    do
    {
        // Channel Status
        if(strncmp(GP, "$PMTKCHN", 8) == 0)
        {
            //Channel_Parse(GP);
        }
        //Ack Parse
        else if(strncmp(GP, "$PMTK", 5) == 0)
        {
            //Ack_Parse(GP);
        }
        // NMEA Parse
        else if((strncmp(GP, "$GP", 3) == 0) && (strlen(GP) > 10))
        {
            // skip "$GP" char to fetch Message Type
            strncpy ( type, GP+3, 3);
            type[3]='\0';

            if(strcmp( type, "GLL")==0)
            {
                GLL_Parse( GP);
            }
            else if(strcmp( type, "RMC")==0)
            {
                RMC_Parse( GP);
            }
            else if(strcmp( type, "VTG")==0)
            {
                VTG_Parse( GP);
            }
            else if(strcmp( type, "GSA")==0)
            {
                GSA_Parse( GP);
            }
            else if(strcmp( type, "GSV")==0)
            {
                GSV_Parse( GP);
            }
            else if(strcmp( type, "GGA")==0)
            {
                GGA_Parse( GP);
            }
            else if(strcmp( type, "ZDA")==0)
            {
                ZDA_Parse( GP);
            }
        }
    }while( (GP = strtok( NULL, "\r\n")) != NULL );
}

void *GPS_EngThread(void *arg)
{
    char *info;
    ttff = 0;
    fixed = 0;   
    //for MNL version
    char buf[72];
    const char *delim = ",";
    char *p = NULL;
    int i;
    
    while(1){
        int i = 0;
        int read_leng = 0;
        int total_leng = 0;      
            	
        memset(ptr, '\n', INFO_SIZE);
        ptr[INFO_SIZE-1] = 0x0;	
        if(sockfd != C_INVALID_SOCKET)
        {
            memset(nmea_buf, 0, NMEA_SIZE);
            ALOGD(TAG"read from sockfd 1\n");
            read_leng = read(sockfd, &nmea_buf[total_leng], (NMEA_SIZE - total_leng));
            total_leng += read_leng;
            ALOGD(TAG"read_leng=%d, total_leng=%d\n", read_leng, total_leng);			
			
            if (read_leng <= 0) 
            {
                ALOGD("ERROR reading from socket: %s, %d\n", strerror(errno), read_leng);                                
                return (void *)0;
            }
            else if(total_leng > 0)
            {
                if(strncmp(nmea_buf, "$PMTK705", 8) == 0)
                {
                    memset(buf, 0, sizeof(buf));
					strcpy(buf, nmea_buf);                    
                    ALOGD("mnl_ver = %s\n", buf);
                    p = (char*)malloc(sizeof(char)*100);
                    p = strtok(buf, delim);
                    for (i = 0; i < 3; i++)
                        p = strtok(NULL, delim);                    
	                printf("GPS chip version: %s\nMNL version: %s\n", CHIP_VERS, p);
                }
				
                NMEA_Parse((char*)&nmea_buf[0]);
                info = ptr;
                if(((g_gpsInfo.FixType != 0) && (ttff != 0)/*avoid prev second's NMEA*/) || (fixed == 1)) // 2D or 3D fixed
                {
                    info += sprintf(info, "Fixed: Lat = %f, Lon = %f, TTFF = %d\n", g_gpsInfo.Lat, g_gpsInfo.Lon, ttff);
                    fixed = 1;
                }
                else if((g_gpsInfo.FixType != 0) && (ttff == 0)) //skip prev second's NMEA, clear data
                {
                    info += printf(info, "GPS TTFF %d sec\n", ttff++);
					          memset(&g_gpsInfo, 0, sizeof(g_gpsInfo));
					          memset(g_svInfo, 0, (sizeof(SVInfo)*NUM_CH));
                    memset(g_chInfo, 0, (sizeof(ChInfo)*PSEUDO_CH));
                }
                else	// no fix
                {
                    info += sprintf(info, "GPS TTFF %d sec\n", ttff++);
                }
                for(i = 0; i < g_gpsInfo.SV_cnt; i++)
                {
                    info += sprintf(info, "SVid[%d] : %d\n", g_svInfo[i].SVid, g_svInfo[i].SNR);
                    //printf("SVid[%d] : %d\n", g_svInfo[i].SVid, g_svInfo[i].SNR);                    
                }
                printf("%s\n", ptr);
            }
        }
    }
    
    return (void *)0;    
}
int ENG_GPS_Open()
{
    int err;
    pid_t pid;
    int portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    ALOGD("ENG_GPS_Open() start\n");
    // power on GPS chip
    if (mnl_pid != C_INVALID_PID)
    {
        ALOGD("GPS driver is running\n");
        return 0;
    }
    err = mnl_write_attr("/sys/class/gpsdrv/gps/pwrctl", 4);
    if(err != 0)
    {
        ALOGD("ENG_GPS_Open: GPS power-on error: %d\n", err);
        return (-1);
    }

    // run gps driver (libmnlp)
    if ((pid = fork()) < 0) 
    {
        ALOGD("ENG_GPS_Open: fork fails: %d (%s)\n", errno, strerror(errno));
        return (-2);
    } 
    else if (pid == 0)  /*child process*/
    {
        int err;

        ALOGD("ENG_GPS_Open: execute: %s\n", "/system/xbin/libmnlp");
        err = execl("/system/xbin/libmnlp", "libmnlp", NULL);
        if (err == -1)
        {
            ALOGD("ENG_GPS_Open: execl error: %s\n", strerror(errno));
            return (-3);
        }
        return 0;
    } 
    else  /*parent process*/
    {
        mnl_pid = pid;
        ALOGD("ENG_GPS_Open: mnl_pid = %d\n", pid);
    }

    // create socket connection to gps driver
    portno = 7000;
    /* Create a socket point */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        ALOGD("ENG_GPS_Open: ERROR opening socket");
        return (-4);
    }
    server = gethostbyname("127.0.0.1");
    if (server == NULL) {
        ALOGD("ENG_GPS_Open: ERROR, no such host\n");
        return (-5);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    sleep(3);  // sleep 5sec for libmnlp to finish initialization

    /* Now connect to the server */
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    {
         ALOGD("ENG_GPS_Open: ERROR connecting");
         return (-6);
    }	

    if (pthread_create(&gps_eng_thread_handle, NULL, GPS_EngThread, NULL)) 
    {
        ALOGD("ENG_GPS_Open: error creating dsp thread \n");
        return (-7);
    }
    ALOGD("ENG_GPS_Open() stop\n");

    return 0;
}

int ENG_GPS_Close()
{    
    int err;
    int status;
    int child_pid;
     
    ALOGD("ENG_GPS_Close() 1\n");

#if 0
    if ((err = pthread_kill(gps_eng_thread_handle, SIGUSR1))) 
    {
        ALOGD("ENG_GPS_Close: pthread_kill 1 error(%d) \n", err);
    }
    ALOGD("ENG_GPS_Close() 2\n");
    if ((err = pthread_join(gps_eng_thread_handle, NULL)))
    {
        ALOGD("ENG_GPS_Close: pthread_kill 2 error(%d) \n", err);
    }
    ALOGD("ENG_GPS_Close() 3\n");
#endif

    // disconnect to gps driver
    if(sockfd != C_INVALID_SOCKET)
    {
        close(sockfd);
        sockfd = C_INVALID_SOCKET;
    }
    ALOGD("ENG_GPS_Close() 4\n");

    // kill gps driver (libmnlp)
    if(mnl_pid != C_INVALID_PID)
    {
        kill(mnl_pid, SIGKILL);
    }
    ALOGD("ENG_GPS_Close() 5\n");
    
    child_pid = waitpid(mnl_pid, &status, 0);

    ALOGD("Kill child_pid = %d", child_pid);
    if (child_pid == -1)
        ALOGD("wait error: %s\n",strerror(errno));
                
    if (WIFEXITED(status))
        ALOGD("Normal termination with exit status = %d\n", WEXITSTATUS(status));
    if (WIFSIGNALED(status))
        ALOGD("Killed by signal %d%s\n", WTERMSIG(status), WCOREDUMP(status) ? "dump core" : "");
    if (WIFSTOPPED(status))
        ALOGD("Stopped by signal = %d\n", WSTOPSIG(status)); 
    
    // power off GPS chip
    err = mnl_write_attr("/sys/class/gpsdrv/gps/pwrctl", 0);
    if(err != 0)
    {
        ALOGD("GPS power-off error: %d\n", err);
    }
    mnl_pid = C_INVALID_PID;
    ALOGD("ENG_GPS_Close() 6\n");

    ALOGD("ENG_GPS_Close() 7\n");
    return 0;
}

int ENG_GPS_Restart(GPS_RESTART restart) 
{
    int en_start;
    en_start = restart;
    int res = 0;
    int ret = 0;
    switch(en_start)
    {
        case GPS_HOT_RESTART:           
            res = write(sockfd, "$PMTK101*32\r\n", sizeof("$PMTK101*32\r\n")); 
            ttff = 0;
            fixed = 0;
            memset(ptr, '\n', INFO_SIZE);
            ptr[INFO_SIZE-1] = 0x0;            
            if (res > 0){
            	  ret = 0;
                printf("Command send success 1!\n");                
            } else {
                ret = -1;
                printf("Command send fail 1!\n");
            }			
            break;
        case GPS_WARM_RESTART:            
            res = write(sockfd, "$PMTK102*31\r\n", sizeof("$PMTK102*31\r\n") );
            ttff = 0;
            fixed = 0;
            memset(ptr, '\n', INFO_SIZE);
            ptr[INFO_SIZE-1] = 0x0; 
            if (res > 0){
            	  ret = 0;
                printf("Command send success 2!\n");
            } else {
            	  ret = -1;
                printf("Command send fail 2!\n");
            }            
            break;
        case GPS_COLD_RESTART:            
            res = write(sockfd, "$PMTK103*30\r\n", sizeof("$PMTK103*30\r\n") );
            ttff = 0;
            fixed = 0;
            memset(ptr, '\n', INFO_SIZE);
            ptr[INFO_SIZE-1] = 0x0;
            if (res > 0){
            	  ret = 0;
                printf("Command send success 3!\n");
            } else {
            	  ret = -1;
                printf("Command send fail 3!\n");
            }
            break;
        case GPS_FULL_RESTART:
            ttff = 0;
            fixed = 0;
            memset(ptr, '\n', INFO_SIZE);
            ptr[INFO_SIZE-1] = 0x0;
            res = write(sockfd, "$PMTK104*37\r\n", sizeof("$PMTK104*37\r\n") );
            if (res > 0){
            	  ret = 0;
                printf("Command send success 4!\n");
            } else {
            	  ret = -1;
                printf("Command send fail 4!\n");
            }           
            break;
        default:
        	  ret = -1;
            printf("No command\n");
            break;
    }
    
    return ret;
}

int ENG_GPS_Getver()
{
    int res = 0;	
    char cmd[] = "$PMTK605*31\r\n";
    	
	//get chip version
    memset(CHIP_VERS, 0, sizeof(CHIP_VERS));
    #ifdef MTK_GPS_MT3326
    strcpy(CHIP_VERS, "MTK_GPS_MT3326\r\n");
    #endif
    #ifdef MTK_GPS_MT6620
    strcpy(CHIP_VERS, "MTK_GPS_MT6620\r\n");
    #endif
    #ifdef MTK_GPS_MT3336
    strcpy(CHIP_VERS, "MTK_GPS_MT3336\r\n");
    #endif
    #ifdef MTK_GPS_MT6628
    strcpy(CHIP_VERS, "MTK_GPS_MT6628\r\n");
    #endif
	#ifdef MTK_GPS_MT3332
    strcpy(CHIP_VERS, "MTK_GPS_MT3332\r\n");
    #endif


    //Query MNL version        
    int len = write(sockfd, cmd, sizeof(cmd));
    ALOGD("len = %d, cmd = %s", len, cmd);
    if(len < 0){
        ALOGE("Send cmd fail!!");
		return -1;
    } else {
        ALOGD("Send command PMTK605*31 to MNL success!");		
    }

	return 0;
    
}

