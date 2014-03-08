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


#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <sys/ioctl.h>
#include <sys/time.h>
//#include <linux/mtgpio.h>
//for EPO file
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define EPO_FILE "/data/misc/EPO.DAT"
#define EPO_FILE_NEW "/data/misc/EPOTMP.DAT"

#define MTK_EPO_SET_PER_DAY  4
#define MTK_EPO_MAX_DAY      30
#define MTK_EPO_ONE_SV_SIZE  72
#define MTK_EPO_SET_MAX_SIZE 2304  //72*32, One SET
#define MTK_EPO_MAX_SET (MTK_EPO_MAX_DAY * MTK_EPO_SET_PER_DAY)
#define BUF_SIZE MTK_EPO_SET_MAX_SIZE

#define  LOG_TAG  "gps_mtk_gps"
#include <cutils/log.h>
#include <cutils/sockets.h>
#include <cutils/properties.h>
#ifdef HAVE_LIBC_SYSTEM_PROPERTIES
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#endif 

#include <hardware/gps.h>

/* the name of the controlled socket */
#define  GPS_CHANNEL_NAME       "/dev/gps"
#define  GPS_MNL_DAEMON_NAME    "mnld"
#define  GPS_MNL_DAEMON_PROP    "init.svc.mnld"

#define  GPS_DEBUG  1
#define  NEMA_DEBUG 0   /*the flag works if GPS_DEBUG is defined*/

#define  TRC(f)       ALOGD("%s", __func__)
#define  ERR(f, ...)  ALOGE("%s: " f, __func__, ##__VA_ARGS__)
#define  WAN(f, ...)  ALOGW("%s: " f, __func__, ##__VA_ARGS__)
#if GPS_DEBUG
#  define DBG(f, ...) ALOGD("%s: " f, __func__, ##__VA_ARGS__)
#  define VER(f, ...) ((void)0) //((void)0)//
#else
#  define DBG(...)    ((void)0)
#  define VER(...)    ((void)0)
#endif

/*****************************************************************************/
/*    MTK gps device control                                                  */   
/*****************************************************************************/
enum { 
    MNL_CMD_UNKNOWN = -1,
    /*command send from GPS HAL*/    
    MNL_CMD_INIT            = 0x00,
    MNL_CMD_CLEANUP         = 0x01,
    MNL_CMD_STOP            = 0x02,
    MNL_CMD_START           = 0x03,
    MNL_CMD_RESTART         = 0x04,    /*restart MNL process*/
    MNL_CMD_RESTART_HOT     = 0x05,    /*restart MNL by PMTK command: hot start*/
    MNL_CMD_RESTART_WARM    = 0x06,    /*restart MNL by PMTK command: warm start*/
    MNL_CMD_RESTART_COLD    = 0x07,    /*restart MNL by PMTK command: cold start*/
    MNL_CMD_RESTART_FULL    = 0x08,    /*restart MNL by PMTK command: full start*/
    MNL_CMD_RESTART_AGPS    = 0x09,    /*restart MNL by PMTK command: AGPS start*/

   MNL_CMD_READ_EPO_TIME = 0x33,
   MNL_CMD_UPDATE_EPO_FILE = 0x34,
   
    /*please see mnld.c for other definition*/
};

static const char* gps_native_thread = "GPS NATIVE THREAD";

/*---------------------------------------------------------------------------*/
typedef struct {    
    int sock; 
} MTK_GPS;
/*****************************************************************************/
static MTK_GPS mtk_gps_gps = {
    .sock = -1,
};
static int gps_nmea_end_tag = 0;
/*****************************************************************************/
int mtk_gps_start_daemon() { /*gps driver must exist before running the function*/
    int start = 0;
    char status[PROPERTY_VALUE_MAX] = {0};
    int count = 100, len;
#ifdef HAVE_LIBC_SYSTEM_PROPERTIES    
    const prop_info *pi = NULL;
    unsigned serial = 0;
#endif     
    TRC();

#ifdef HAVE_LIBC_SYSTEM_PROPERTIES
    pi = __system_property_find(GPS_MNL_DAEMON_PROP);
    if (pi != NULL)
        serial = __system_property_serial(pi);
#endif

	property_set("ctl.start", GPS_MNL_DAEMON_NAME);
	sched_yield();

    while (count-- > 0) {
 #ifdef HAVE_LIBC_SYSTEM_PROPERTIES
        if (pi == NULL) {
            pi = __system_property_find(GPS_MNL_DAEMON_PROP);
        }
        if (pi != NULL) {
            __system_property_read(pi, NULL, status);
            if (strcmp(status, "running") == 0) {
                DBG("running\n");
                return 0;
            } else if (__system_property_serial(pi) != serial &&
                    strcmp(status, "stopped") == 0) {
                return -1;
            }
        }
#else
        if (property_get(GPS_MNL_DAEMON_PROP, status, NULL)) {
            if (strcmp(status, "running") == 0)
                return 0;
        }
#endif
        WAN("[%02d] '%s'\n", count, status);
        usleep(100000);
    }
    ERR("start daemon timeout!!\n");
    return -1;
}
/*****************************************************************************/
int mtk_gps_stop_daemon() {
    char status[PROPERTY_VALUE_MAX] = {0};
    int count = 50;

    TRC();
    if (property_get(GPS_MNL_DAEMON_PROP, status, NULL) && 
        strcmp (status, "stopped") == 0) {
        DBG("daemon is already stopped!!");
        return 0;
    }

    property_set("ctl.stop", GPS_MNL_DAEMON_NAME);
    sched_yield();

    while (count-- > 0) {
        if (property_get(GPS_MNL_DAEMON_PROP, status, NULL)) {
            if (strcmp(status, "stopped") == 0) {
                DBG("daemon is stopped");
                return 0;
            }
        }
        usleep(100000);
    }
    ERR("stop daemon timeout!!\n");
    return -1;
}
/*****************************************************************************/
int mtk_gps_daemon_init() {
    int err = -1, lsocket;
    int count = 10;
    
    if (mtk_gps_start_daemon() < 0) {
        ERR("start daemon fail: %s\n", strerror(errno));
        return -1;
    }

    lsocket = socket(PF_UNIX, SOCK_STREAM, 0);
    if (lsocket < 0) {
        ERR("fail to create socket: %s\n", strerror(errno));
        return -1;
    }
    
    while (count-- > 0) {
        err = socket_local_client_connect(lsocket, GPS_MNL_DAEMON_NAME, 
                                    ANDROID_SOCKET_NAMESPACE_RESERVED, SOCK_STREAM);
        /*connect success*/
        if (err >= 0)   
            break;
        /*connect retry*/
        WAN("[%02d] retry\n", count);
        err = mtk_gps_start_daemon();
        if (err < 0)
            break;
        usleep(100000);    
    }
    if (err < 0) {
        ERR("socket_local_client_connect fail: %s\n", strerror(errno));
        return -1;
    }    
    mtk_gps_gps.sock = lsocket;
    DBG("socket : %d\n", lsocket);
    return 0;
}
/*****************************************************************************/
int mtk_gps_daemon_cleanup() {
    TRC();
    if (mtk_gps_gps.sock != -1) 
        close(mtk_gps_gps.sock);    
    return mtk_gps_stop_daemon();
}
/*****************************************************************************/
int mtk_gps_daemon_send(char* cmd, int len) {
    DBG("send: %d,%d\n", (int)(*cmd), len);
    int ret;
    if (mtk_gps_gps.sock != -1) {
        ret = send(mtk_gps_gps.sock, cmd, len, 0);
        if (ret != len)
            ERR("send fails: %d(%s)\n", errno, strerror(errno));
        return (ret != len) ? (-1) : (0);        
    }
    errno = -EINVAL;        
    return -1;
}
/*****************************************************************************/
int mtk_gps_init() 
{
    int err;
    char buf[] = {MNL_CMD_INIT};
    TRC();    
    if ((err = mtk_gps_daemon_init()))
        return err;
    return mtk_gps_daemon_send(buf, sizeof(buf));    
}
/*****************************************************************************/
int mtk_gps_cleanup()
{
    int err, res;
    char buf[] = {MNL_CMD_CLEANUP};
    TRC();    
    if ((err = mtk_gps_daemon_send(buf, sizeof(buf))))
        ERR("mtk_gps_cleanup send cleanup fails\n");
    res = mtk_gps_daemon_cleanup();
    //DBG("cleanup: %d, %d\n", err, res);
    return 0;
}
/*****************************************************************************/
int mtk_gps_start()
{
    char buf[] = {MNL_CMD_START};
    int err;
    int idx = 0, max = 5;
    TRC();
    err = mtk_gps_daemon_send(buf, sizeof(buf));
    /*sometimes, the mnld is restarted, so */
    while (err && (errno == EPIPE) && (idx++ < max)) {
        sleep(1);    
        if ((err = mtk_gps_daemon_init()))
            err = mtk_gps_daemon_send(buf, sizeof(buf));
    }
    return err;
}
/*****************************************************************************/
int mtk_gps_stop()
{
    char buf[] = {MNL_CMD_STOP};
    TRC();
    return mtk_gps_daemon_send(buf, sizeof(buf));
}
/*****************************************************************************/
int mtk_gps_restart(unsigned char cmd)
{
    char buf[] = {cmd};
    TRC();
    return mtk_gps_daemon_send(buf, sizeof(buf));
}
/*****************************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****                                                       *****/
/*****       N M E A   T O K E N I Z E R                     *****/
/*****                                                       *****/
/*****************************************************************/
/*****************************************************************/

typedef struct {
    const char*  p;
    const char*  end;
} Token;

#define  MAX_NMEA_TOKENS  24

typedef struct {
    int     count;
    Token   tokens[ MAX_NMEA_TOKENS ];
} NmeaTokenizer;

static int
nmea_tokenizer_init( NmeaTokenizer*  t, const char*  p, const char*  end )
{
    int    count = 0;
    char*  q;

    // the initial '$' is optional
    if (p < end && p[0] == '$')
        p += 1;

    // remove trailing newline
    if (end > p && end[-1] == '\n') {
        end -= 1;
        if (end > p && end[-1] == '\r')
            end -= 1;
    }

    // get rid of checksum at the end of the sentecne
    if (end >= p+3 && end[-3] == '*') {
        end -= 3;
    }

    while (p < end) {
        const char*  q = p;

        q = memchr(p, ',', end-p);
        if (q == NULL)
            q = end;

        if (q >= p) {
            if (count < MAX_NMEA_TOKENS) {
                t->tokens[count].p   = p;
                t->tokens[count].end = q;
                count += 1;
            }
        }
        if (q < end)
            q += 1;

        p = q;
    }

    t->count = count;
    return count;
}

static Token
nmea_tokenizer_get( NmeaTokenizer*  t, int  index )
{
    Token  tok;
    static const char*  dummy = "";

    if (index < 0 || index >= t->count) {
        tok.p = tok.end = dummy;
    } else
        tok = t->tokens[index];

    return tok;
}


static int
str2int( const char*  p, const char*  end )
{
    int   result = 0;
    int   len    = end - p;
    int   sign = 1;

    if (*p == '-')
    {
        sign = -1;
        p++;
        len = end - p;
    }

    for ( ; len > 0; len--, p++ )
    {
        int  c;

        if (p >= end)
            goto Fail;

        c = *p - '0';
        if ((unsigned)c >= 10)
            goto Fail;

        result = result*10 + c;
    }
    return  sign*result;

Fail:
    return -1;
}

static double
str2float( const char*  p, const char*  end )
{
    int   result = 0;
    int   len    = end - p;
    char  temp[16];

    if (len >= (int)sizeof(temp))
        return 0.;

    memcpy( temp, p, len );
    temp[len] = 0;
    return strtod( temp, NULL );
}

/*****************************************************************/
/*****************************************************************/
/*****                                                       *****/
/*****       N M E A   P A R S E R                           *****/
/*****                                                       *****/
/*****************************************************************/
/*****************************************************************/

#define  NMEA_MAX_SIZE  83
/*maximum number of SV information in GPGSV*/
#define  NMEA_MAX_SV_INFO 4 
#define  LOC_FIXED(pNmeaReader) ((pNmeaReader->fix_mode == 2) || (pNmeaReader->fix_mode ==3))
typedef struct {
    int     pos;
    int     overflow;
    int     utc_year;
    int     utc_mon;
    int     utc_day;
    int     utc_diff;
    GpsLocation  fix;

    /*
     * The fix flag extracted from GPGSA setence: 1: No fix; 2 = 2D; 3 = 3D
     * if the fix mode is 0, no location will be reported via callback
     * otherwise, the location will be reported via callback
     */
    int     fix_mode;           
    /*
     * Indicate that the status of callback handling.
     * The flag is used to report GPS_STATUS_SESSION_BEGIN or GPS_STATUS_SESSION_END:
     * (0) The flag will be set as true when callback setting is changed via nmea_reader_set_callback
     * (1) GPS_STATUS_SESSION_BEGIN: receive location fix + flag set + callback is set
     * (2) GPS_STATUS_SESSION_END:   receive location fix + flag set + callback is null
     */
    int     cb_status_changed;  
    int     sv_count;           /*used to count the number of received SV information*/    
    GpsSvStatus  sv_status;  
    GpsCallbacks callbacks;
    char    in[ NMEA_MAX_SIZE+1 ];
} NmeaReader;


static void
nmea_reader_update_utc_diff( NmeaReader*  r )
{
    time_t         now = time(NULL);
    struct tm      tm_local;
    struct tm      tm_utc;
    long           time_local, time_utc;

    gmtime_r( &now, &tm_utc );
    localtime_r( &now, &tm_local );

    time_local = tm_local.tm_sec +
                 60*(tm_local.tm_min +
                 60*(tm_local.tm_hour +
                 24*(tm_local.tm_yday +
                 365*tm_local.tm_year)));

    time_utc = tm_utc.tm_sec +
               60*(tm_utc.tm_min +
               60*(tm_utc.tm_hour +
               24*(tm_utc.tm_yday +
               365*tm_utc.tm_year)));

    r->utc_diff = time_utc - time_local;
}


static void
nmea_reader_init( NmeaReader*  r )
{
    memset( r, 0, sizeof(*r) );

    r->pos      = 0;
    r->overflow = 0;
    r->utc_year = -1;
    r->utc_mon  = -1;
    r->utc_day  = -1;
    r->utc_diff = 0;
    r->callbacks.location_cb= NULL;
    r->callbacks.status_cb= NULL;
    r->callbacks.sv_status_cb= NULL;  
    r->sv_count = 0;
    r->fix_mode = 0;    /*no fix*/
    r->cb_status_changed = 0;
    memset((void*)&r->sv_status, 0x00, sizeof(r->sv_status));
    memset((void*)&r->in, 0x00, sizeof(r->in));
    
    nmea_reader_update_utc_diff( r );
}

GpsStatus sta;
static void
nmea_reader_set_callback( NmeaReader*  r, GpsCallbacks* cbs)
{
    if (!r) {           /*this should not happen*/
        return; 
    } else if (!cbs) {  /*unregister the callback */
        if (r->callbacks.location_cb != NULL) {
            //GpsStatus sta;                                                                                                   //Modify to global
            sta.status = GPS_STATUS_SESSION_END;
            r->callbacks.status_cb(&sta);
            DBG("sta.status = GPS_STATUS_SESSION_END\n");
            sta.status = GPS_STATUS_ENGINE_OFF;
            DBG("sta.status = GPS_STATUS_ENGINE_OFF\n");
            r->callbacks.status_cb(&sta);

        }
        r->callbacks.location_cb = NULL;
        r->callbacks.status_cb = NULL;
        r->callbacks.sv_status_cb = NULL;
        r->callbacks.nmea_cb = NULL;
        return ;
    } else {            /*register the callback*/    
        if (r->callbacks.location_cb == NULL) {
            //GpsStatus sta;                                                                                                 //Modify to global
            sta.status = GPS_STATUS_ENGINE_ON;
            DBG("sta.status = GPS_STATUS_ENGINE_ON\n");
            cbs->status_cb(&sta);
            sta.status = GPS_STATUS_SESSION_BEGIN;
            DBG("sta.status = GPS_STATUS_SESSION_BEGIN\n");
            cbs->status_cb(&sta);
        }
        r->callbacks = *cbs;
        /*don't report any thing until the data is reported*/
        if (r->callbacks.location_cb != NULL && r->fix.flags != 0) {
            DBG("%s: sending latest fix to new callback", __FUNCTION__);
            //r->callbacks.location_cb( &r->fix );
            r->fix.flags = 0;
        }
        if (r->callbacks.sv_status_cb != NULL && r->sv_status.num_svs != 0) {
            //r->callbacks.sv_status_cb(&r->sv_status);
            r->sv_count = r->sv_status.num_svs = 0;
            r->sv_status.used_in_fix_mask = 0;
        }
    }
}


static int
nmea_reader_update_time( NmeaReader*  r, Token  tok )
{
    int        hour, minute;
    double     seconds;
    struct tm  tm;
    time_t     fix_time;

    if (tok.p + 6 > tok.end)
        return -1;
        
    memset((void*)&tm, 0x00, sizeof(tm));
    if (r->utc_year < 0) {
        // no date yet, get current one
        time_t  now = time(NULL);
        gmtime_r( &now, &tm );
        r->utc_year = tm.tm_year + 1900;
        r->utc_mon  = tm.tm_mon + 1;
        r->utc_day  = tm.tm_mday;
    }

    hour    = str2int(tok.p,   tok.p+2);
    minute  = str2int(tok.p+2, tok.p+4);
    seconds = str2float(tok.p+4, tok.end);

    tm.tm_hour = hour;
    tm.tm_min  = minute;
    tm.tm_sec  = (int) seconds;
    tm.tm_year = r->utc_year - 1900;
    tm.tm_mon  = r->utc_mon - 1;
    tm.tm_mday = r->utc_day;
    tm.tm_isdst = -1;

    if (mktime(&tm) == (time_t)-1)
        ERR("mktime error: %d %s\n", errno, strerror(errno));
        
    //Add by ZQH to recalculate the utc_diff when the time zone is reset     
    nmea_reader_update_utc_diff( r );
        
    fix_time = mktime( &tm ) - r->utc_diff;
    r->fix.timestamp = (long long)fix_time * 1000;
    return 0;
}

static int
nmea_reader_update_date( NmeaReader*  r, Token  date, Token  time )
{
    Token  tok = date;
    int    day, mon, year;

    if (tok.p + 6 != tok.end) {
        ERR("date not properly formatted: '%.*s'", tok.end-tok.p, tok.p);
        return -1;
    }
    day  = str2int(tok.p, tok.p+2);
    mon  = str2int(tok.p+2, tok.p+4);
    year = str2int(tok.p+4, tok.p+6) + 2000;

    if ((day|mon|year) < 0) {
        ERR("date not properly formatted: '%.*s'", tok.end-tok.p, tok.p);
        return -1;
    }

    r->utc_year  = year;
    r->utc_mon   = mon;
    r->utc_day   = day;

    return nmea_reader_update_time( r, time );
}


static double
convert_from_hhmm( Token  tok )
{
    double  val     = str2float(tok.p, tok.end);
    int     degrees = (int)(floor(val) / 100);
    double  minutes = val - degrees*100.;
    double  dcoord  = degrees + minutes / 60.0;
    return dcoord;
}


static int
nmea_reader_update_latlong( NmeaReader*  r,
                            Token        latitude,
                            char         latitudeHemi,
                            Token        longitude,
                            char         longitudeHemi )
{
    double   lat, lon;
    Token    tok;

    tok = latitude;
    if (tok.p + 6 > tok.end) {
        ERR("latitude is too short: '%.*s'", tok.end-tok.p, tok.p);
        return -1;
    }
    lat = convert_from_hhmm(tok);
    if (latitudeHemi == 'S')
        lat = -lat;

    tok = longitude;
    if (tok.p + 6 > tok.end) {
        ERR("longitude is too short: '%.*s'", tok.end-tok.p, tok.p);
        return -1;
    }
    lon = convert_from_hhmm(tok);
    if (longitudeHemi == 'W')
        lon = -lon;

    r->fix.flags    |= GPS_LOCATION_HAS_LAT_LONG;
    r->fix.latitude  = lat;
    r->fix.longitude = lon;
    return 0;
}


static int
nmea_reader_update_altitude( NmeaReader*  r,
                             Token        altitude,
                             Token        units )
{
    double  alt;
    Token   tok = altitude;

    if (tok.p >= tok.end)
        return -1;

    r->fix.flags   |= GPS_LOCATION_HAS_ALTITUDE;
    r->fix.altitude = str2float(tok.p, tok.end);
    return 0;
}


static int
nmea_reader_update_bearing( NmeaReader*  r,
                            Token        bearing )
{
    double  alt;
    Token   tok = bearing;

    if (tok.p >= tok.end)
        return -1;

    r->fix.flags   |= GPS_LOCATION_HAS_BEARING;
    r->fix.bearing  = str2float(tok.p, tok.end);
    return 0;
}


static int
nmea_reader_update_speed( NmeaReader*  r,
                          Token        speed )
{
    double  alt;
    Token   tok = speed;

    if (tok.p >= tok.end)
        return -1;

    r->fix.flags   |= GPS_LOCATION_HAS_SPEED;

    //Modify by ZQH to convert the speed unit from knot to m/s
    //r->fix.speed    = str2float(tok.p, tok.end);
    r->fix.speed = str2float(tok.p, tok.end) / 1.942795467;
    return 0;
}

//Add by LCH for accuracy
static int
nmea_reader_update_accuracy( NmeaReader*  r,
                             Token accuracy )
{
    double  alt;
    Token   tok = accuracy;

    if (tok.p >= tok.end)
        return -1;

    r->fix.flags   |= GPS_LOCATION_HAS_ACCURACY;
    r->fix.accuracy = str2float(tok.p, tok.end);
    return 0;
}

static int
nmea_reader_update_sv_status( NmeaReader* r, int sv_index,
                              Token id, Token elevation,
                              Token azimuth, Token snr) 
{   
    int prn = str2int(id.p, id.end);    
    if ((prn <= 0) || (prn > GPS_MAX_SVS) || (r->sv_count >= GPS_MAX_SVS)) {
        VER("sv_status: ignore (%d)", prn);
        return 0;
    }
    sv_index = r->sv_count;
    r->sv_status.sv_list[sv_index].prn = prn;
    r->sv_status.sv_list[sv_index].snr = str2int(snr.p, snr.end);
    r->sv_status.sv_list[sv_index].elevation = str2int(elevation.p, elevation.end);
    r->sv_status.sv_list[sv_index].azimuth = str2int(azimuth.p, azimuth.end);
    r->sv_count++;
    VER("sv_status(%2d): %2d, %2f, %3f, %2f", sv_index, r->sv_status.sv_list[sv_index].prn, r->sv_status.sv_list[sv_index].elevation,
                                         r->sv_status.sv_list[sv_index].azimuth, r->sv_status.sv_list[sv_index].snr);        
    return 0;
}

static void
nmea_reader_parse( NmeaReader*  r )
{
   /* we received a complete sentence, now parse it to generate
    * a new GPS fix...
    */
    NmeaTokenizer  tzer[1];
    Token          tok;

#if NEMA_DEBUG    
    DBG("Received: '%.*s'", r->pos, r->in);
#endif     
    if (r->pos < 9) {
        ERR("Too short. discarded. '%.*s'", r->pos, r->in);
        return;
    }

    nmea_tokenizer_init(tzer, r->in, r->in + r->pos);
#if NEMA_DEBUG
    {
        int  n;
        DBG("Found %d tokens", tzer->count);
        for (n = 0; n < tzer->count; n++) {
            Token  tok = nmea_tokenizer_get(tzer,n);
            DBG("%2d: '%.*s'", n, tok.end-tok.p, tok.p);
        }
    }
#endif

    tok = nmea_tokenizer_get(tzer, 0);
    if (tok.p + 5 > tok.end) {
        ERR("sentence id '%.*s' too short, ignored.", tok.end-tok.p, tok.p);
        return;
    }

    // ignore first two characters.
    tok.p += 2;
    if ( !memcmp(tok.p, "GGA", 3) ) {
        // GPS fix
        Token  tok_time          = nmea_tokenizer_get(tzer,1);
        Token  tok_latitude      = nmea_tokenizer_get(tzer,2);
        Token  tok_latitudeHemi  = nmea_tokenizer_get(tzer,3);
        Token  tok_longitude     = nmea_tokenizer_get(tzer,4);
        Token  tok_longitudeHemi = nmea_tokenizer_get(tzer,5);
        Token  tok_altitude      = nmea_tokenizer_get(tzer,9);
        Token  tok_altitudeUnits = nmea_tokenizer_get(tzer,10);

        nmea_reader_update_time(r, tok_time);
        nmea_reader_update_latlong(r, tok_latitude,
                                      tok_latitudeHemi.p[0],
                                      tok_longitude,
                                      tok_longitudeHemi.p[0]);
        nmea_reader_update_altitude(r, tok_altitude, tok_altitudeUnits);

    } else if ( !memcmp(tok.p, "GSA", 3) ) {
        Token tok_fix = nmea_tokenizer_get(tzer, 2);    
        int idx, max = 12; /*the number of satellites in GPGSA*/

        r->fix_mode = str2int(tok_fix.p, tok_fix.end);    
        r->sv_status.used_in_fix_mask = 0;
        if (LOC_FIXED(r)) { /* 1: No fix; 2: 2D; 3: 3D*/
            for (idx = 0; idx < max; idx++) {
                Token tok_satellite = nmea_tokenizer_get(tzer, idx+3);
                if (tok_satellite.p == tok_satellite.end) {                    
                    VER("GPGSA: found %d active satellites", idx);
                    break;
                }
                int sate_id = str2int(tok_satellite.p, tok_satellite.end);
                if (sate_id >= 1 && sate_id <= 32) {
                    r->sv_status.used_in_fix_mask |= ( 1 << (sate_id-1) );
                } else {
                    //r->sv_status.used_in_fix_mask = 0;
                    //VER("GPGSA: invalid sentence, ignore!!");
                    //break;
                    // Mask the above code to avoid the display issue caused by QZSS-193 Satellite
                    DBG("GPGSA: unsupported satellite ID %d", sate_id);
                }
            }
        }
        VER("GPGSA: mask 0x%x", r->sv_status.used_in_fix_mask);
    } else if ( !memcmp(tok.p, "RMC", 3) ) {
        Token  tok_time          = nmea_tokenizer_get(tzer,1);
        Token  tok_fixStatus     = nmea_tokenizer_get(tzer,2);
        Token  tok_latitude      = nmea_tokenizer_get(tzer,3);
        Token  tok_latitudeHemi  = nmea_tokenizer_get(tzer,4);
        Token  tok_longitude     = nmea_tokenizer_get(tzer,5);
        Token  tok_longitudeHemi = nmea_tokenizer_get(tzer,6);
        Token  tok_speed         = nmea_tokenizer_get(tzer,7);
        Token  tok_bearing       = nmea_tokenizer_get(tzer,8);
        Token  tok_date          = nmea_tokenizer_get(tzer,9);

        VER("in RMC, fixStatus=%c", tok_fixStatus.p[0]);
        if (tok_fixStatus.p[0] == 'A')
        {
            nmea_reader_update_date( r, tok_date, tok_time );

            nmea_reader_update_latlong( r, tok_latitude,
                                           tok_latitudeHemi.p[0],
                                           tok_longitude,
                                           tok_longitudeHemi.p[0] );

            nmea_reader_update_bearing( r, tok_bearing );
            nmea_reader_update_speed  ( r, tok_speed );
        }
    } else if ( !memcmp(tok.p, "GSV", 3) ) {
        Token tok_num = nmea_tokenizer_get(tzer,1); //number of messages
        Token tok_seq = nmea_tokenizer_get(tzer,2); //sequence number
        Token tok_cnt = nmea_tokenizer_get(tzer,3); //Satellites in view
        int num = str2int(tok_num.p, tok_num.end);
        int seq = str2int(tok_seq.p, tok_seq.end);
        int cnt = str2int(tok_cnt.p, tok_cnt.end);
        int sv_base = (seq - 1)*NMEA_MAX_SV_INFO;
        int sv_num = cnt - sv_base;
        int idx, base = 4, base_idx;
        if (sv_num > NMEA_MAX_SV_INFO) 
            sv_num = NMEA_MAX_SV_INFO;
        if (seq == 1)   /*if sequence number is 1, a new set of GPGSV will be parsed*/
            r->sv_count = 0;
        for (idx = 0; idx < sv_num; idx++) {            
            base_idx = base*(idx+1);
            Token tok_id  = nmea_tokenizer_get(tzer, base_idx+0);
            Token tok_ele = nmea_tokenizer_get(tzer, base_idx+1);
            Token tok_azi = nmea_tokenizer_get(tzer, base_idx+2);
            Token tok_snr = nmea_tokenizer_get(tzer, base_idx+3);
            nmea_reader_update_sv_status(r, sv_base+idx, tok_id, tok_ele, tok_azi, tok_snr);
        }
        if (seq == num) {
            if (r->sv_count <= cnt) { 
                r->sv_status.num_svs = r->sv_count;
                r->sv_status.almanac_mask = 0;
                r->sv_status.ephemeris_mask = 0;
            } else {
                ERR("GPGSV incomplete (%d/%d), ignored!", r->sv_count, cnt);
                r->sv_count = r->sv_status.num_svs = 0;                
            }
        }        
    }
    //Add for Accuracy, LCH
    else if ( !memcmp(tok.p, "ACCURACY", 8)) {
        if((r->fix_mode == 3)) {
        //if(LOC_FIXED(r)) {
        Token  tok_accuracy = nmea_tokenizer_get(tzer,1);
        nmea_reader_update_accuracy(r, tok_accuracy);
            DBG("GPS get accuracy from driver:%f\n", r->fix.accuracy);
        }
        else {
            DBG("GPS get accuracy failed, fix mode:%d\n", r->fix_mode);
        } 
    }
    else {
        tok.p -= 2;
        VER("unknown sentence '%.*s", tok.end-tok.p, tok.p);
    }
    if (!LOC_FIXED(r)) {
        VER("Location is not fixed, ignored callback\n");
    } else if (r->fix.flags != 0 && gps_nmea_end_tag) {
#if NEMA_DEBUG
        char   temp[256];
        char*  p   = temp;
        char*  end = p + sizeof(temp);
        struct tm   utc;

        p += snprintf( p, end-p, "sending fix" );
        if (r->fix.flags & GPS_LOCATION_HAS_LAT_LONG) {
            p += snprintf(p, end-p, " lat=%g lon=%g", r->fix.latitude, r->fix.longitude);
        }
        if (r->fix.flags & GPS_LOCATION_HAS_ALTITUDE) {
            p += snprintf(p, end-p, " altitude=%g", r->fix.altitude);
        }
        if (r->fix.flags & GPS_LOCATION_HAS_SPEED) {
            p += snprintf(p, end-p, " speed=%g", r->fix.speed);
        }
        if (r->fix.flags & GPS_LOCATION_HAS_BEARING) {
            p += snprintf(p, end-p, " bearing=%g", r->fix.bearing);
        }
        if (r->fix.flags & GPS_LOCATION_HAS_ACCURACY) {
            p += snprintf(p,end-p, " accuracy=%g", r->fix.accuracy);
            DBG("GPS accuracy=%g\n", r->fix.accuracy);
        }
        gmtime_r( (time_t*) &r->fix.timestamp, &utc );
        p += snprintf(p, end-p, " time=%s", asctime( &utc ) );
        VER(temp);
#endif
        if (r->callbacks.location_cb) {
            r->callbacks.location_cb( &r->fix );
            r->fix.flags = 0;
        } else {
            VER("no location callback, keeping data until needed !");
        }
    }
    if (r->sv_status.num_svs != 0 && gps_nmea_end_tag) {
        if (r->callbacks.sv_status_cb) {
            int idx;
            DBG("Found %d Satellites, mask = 0x%x", r->sv_status.num_svs, r->sv_status.used_in_fix_mask);
            for (idx = 0; idx < r->sv_status.num_svs; idx++) {
                int prn = r->sv_status.sv_list[idx].prn;
                if (prn < 1 || prn > 32) {
                    ERR("Satellite invalid id: %d", prn);
                } else {
                    VER("Satellite (%2d) = %2f, %3f, %2f", r->sv_status.sv_list[idx].prn, 
                                                       r->sv_status.sv_list[idx].elevation, r->sv_status.sv_list[idx].azimuth,
                                                       r->sv_status.sv_list[idx].snr);
                }
            }                                    
            r->callbacks.sv_status_cb(&r->sv_status);
            r->sv_count = r->sv_status.num_svs = 0;
            r->sv_status.used_in_fix_mask = 0;
        } 
    }
}


static void
nmea_reader_addc( NmeaReader*  r, int  c )
{
    if (r->overflow) {
        r->overflow = (c != '\n');
        return;
    }

    if (r->pos >= (int) sizeof(r->in)-1 ) {
        r->overflow = 1;
        r->pos      = 0;
        return;
    }

    r->in[r->pos] = (char)c;
    r->pos       += 1;

    if (c == '\n') {
        nmea_reader_parse( r );
        if(r->callbacks.nmea_cb != NULL)
        {
            r->callbacks.nmea_cb( r->fix.timestamp, r->in, r->pos );
        }
        r->pos = 0;
    }
}


/*****************************************************************/
/*****************************************************************/
/*****                                                       *****/
/*****       C O N N E C T I O N   S T A T E                 *****/
/*****                                                       *****/
/*****************************************************************/
/*****************************************************************/

/* commands sent to the gps thread */
enum {
    CMD_QUIT  = 0,
    CMD_START = 1,
    CMD_STOP  = 2,
    CMD_RESTART = 3,
};


/* this is the state of our connection to the daemon */
typedef struct {
    int                     init;
    int                     fd;// fd = open (/dev/gps_
    GpsCallbacks            callbacks;
    pthread_t               thread;
    int                     control[2];
} GpsState;

static GpsState  _gps_state[1];


static void
gps_state_done( GpsState*  s )
{
    // tell the thread to quit, and wait for it
    char   cmd = CMD_QUIT;
    void*  dummy;
    write( s->control[0], &cmd, 1 );
    pthread_join(s->thread, &dummy);

    // close the control socket pair
    close( s->control[0] ); s->control[0] = -1;
    close( s->control[1] ); s->control[1] = -1;

    // close connection to the GPS daemon
    close( s->fd ); s->fd = -1;
    s->init = 0;
    if (mtk_gps_cleanup() != 0) 
        ERR("mtk_gps cleanup error!!");
}


static void
gps_state_start( GpsState*  s )
{
    char  cmd = CMD_START;
    int   ret;

    do { ret=write( s->control[0], &cmd, 1 ); }
    while (ret < 0 && errno == EINTR);

    if (ret != 1)
        ERR("%s: could not send CMD_START command: ret=%d: %s",
          __FUNCTION__, ret, strerror(errno));
}


static void
gps_state_stop( GpsState*  s )
{
    char  cmd = CMD_STOP;
    int   ret;

    do { ret=write( s->control[0], &cmd, 1 ); }
    while (ret < 0 && errno == EINTR);

    if (ret != 1)
        ERR("%s: could not send CMD_STOP command: ret=%d: %s",
          __FUNCTION__, ret, strerror(errno));
}

static void
gps_state_restart( GpsState*  s )
{
    char  cmd = CMD_RESTART;
    int   ret;

    do { ret=write( s->control[0], &cmd, 1 ); }
    while (ret < 0 && errno == EINTR);

    if (ret != 1)
        ERR("%s: could not send CMD_RESTART command: ret=%d: %s",
          __FUNCTION__, ret, strerror(errno));
}


static int
epoll_register( int  epoll_fd, int  fd )
{
    struct epoll_event  ev;
    int                 ret, flags;

    /* important: make the fd non-blocking */
    flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    ev.events  = EPOLLIN;
    ev.data.fd = fd;
    do {
        ret = epoll_ctl( epoll_fd, EPOLL_CTL_ADD, fd, &ev );
    } while (ret < 0 && errno == EINTR);
    return ret;
}


static int
epoll_deregister( int  epoll_fd, int  fd )
{
    int  ret;
    do {
        ret = epoll_ctl( epoll_fd, EPOLL_CTL_DEL, fd, NULL );
    } while (ret < 0 && errno == EINTR);
    return ret;
}

/*for reducing the function call to get data from kernel*/
static char buff[1024];
/* this is the main thread, it waits for commands from gps_state_start/stop and,
 * when started, messages from the GPS daemon. these are simple NMEA sentences
 * that must be parsed to be converted into GPS fixes sent to the framework
 */
static void
gps_state_thread( void*  arg )
{
    GpsState*   state = (GpsState*) arg;
    NmeaReader  reader[1];
    int         epoll_fd   = epoll_create(2);
    int         started    = 0;
    int         gps_fd     = state->fd;
    int         control_fd = state->control[1];

    nmea_reader_init( reader );

    // register control file descriptors for polling
    epoll_register( epoll_fd, control_fd );
    epoll_register( epoll_fd, gps_fd );

    DBG("gps thread running: PPID[%d], PID[%d]\n", getppid(), getpid());

    // now loop
    for (;;) {
        struct epoll_event   events[2];
        int                  ne, nevents;

        nevents = epoll_wait( epoll_fd, events, 2, -1 );
        if (nevents < 0) {
            if (errno != EINTR)
                ERR("epoll_wait() unexpected error: %s", strerror(errno));
            continue;
        }
        VER("gps thread received %d events", nevents);
        for (ne = 0; ne < nevents; ne++) {
            if ((events[ne].events & (EPOLLERR|EPOLLHUP)) != 0) {
                ERR("EPOLLERR or EPOLLHUP after epoll_wait() !?");
                goto Exit;
            }
            if ((events[ne].events & EPOLLIN) != 0) {
                int  fd = events[ne].data.fd;

                if (fd == control_fd)
                {
                    char  cmd = 255;
                    int   ret;
                    DBG("gps control fd event");
                    do {
                        ret = read( fd, &cmd, 1 );
                    } while (ret < 0 && errno == EINTR);

                    if (cmd == CMD_QUIT) {
                        DBG("gps thread quitting on demand");
                        goto Exit;
                    }
                    else if (cmd == CMD_START) {
                        if (!started) {
                            DBG("gps thread starting  location_cb=%p", state->callbacks.location_cb);
                            started = 1;
                            nmea_reader_set_callback( reader, &state->callbacks);
                        }
                    }
                    else if (cmd == CMD_STOP) {
                        if (started) {
                            DBG("gps thread stopping");
                            started = 0;
                            nmea_reader_set_callback( reader, NULL );
                        }
                    }
                    else if (cmd == CMD_RESTART) {
                        reader->fix_mode = 0;
                    }
                }
                else if (fd == gps_fd)
                {
                    VER("gps fd event");
                    for (;;) {
                        int  nn, ret;

                        ret = read( fd, buff, sizeof(buff) );
                        if (ret < 0) {
                            if (errno == EINTR)
                                continue;
                            if (errno != EWOULDBLOCK)
                                ERR("error while reading from gps daemon socket: %s: %p", strerror(errno), buff);
                            break;
                        }
                        VER("received %d bytes: %.*s", ret, ret, buff);
                        gps_nmea_end_tag = 0;
                        for (nn = 0; nn < ret; nn++)
                        {
                            if(nn == (ret-1))
                                gps_nmea_end_tag = 1;
                                
                            nmea_reader_addc( reader, buff[nn] );
                        }
                    }
                    VER("gps fd event end");
                }
                else
                {
                    ERR("epoll_wait() returned unkown fd %d ?", fd);
                }
            }
        }
    }
Exit:
    return;
}


static void
gps_state_init( GpsState*  state )
{
    state->init       = 1;
    state->control[0] = -1;
    state->control[1] = -1;
    state->fd         = -1;

    state->fd = open( GPS_CHANNEL_NAME, O_RDONLY); //support poll behavior

    if (state->fd < 0) {
        ERR("no gps hardware detected: %s:%d, %s", GPS_CHANNEL_NAME, state->fd, strerror(errno));
        return;
    }

    if ( mtk_gps_init() != 0 ) {
        ERR("could not initiaize mtk_gps !!");
        goto Fail;
    }
    
    if ( socketpair( AF_LOCAL, SOCK_STREAM, 0, state->control ) < 0 ) {
        ERR("could not create thread control socket pair: %s", strerror(errno));
        goto Fail;
    }

    //if ( pthread_create( &state->thread, NULL, gps_state_thread, state ) != 0 ) {
    if ( !(state->callbacks.create_thread_cb(gps_native_thread, gps_state_thread, state))) {
        ERR("could not create gps thread: %s", strerror(errno));
        goto Fail;
    }

    DBG("gps state initialized");
    return;

Fail:
    gps_state_done( state );
}


/*****************************************************************/
/*****************************************************************/
/*****                                                       *****/
/*****       I N T E R F A C E                               *****/
/*****                                                       *****/
/*****************************************************************/
/*****************************************************************/


static int
mtk_gps_gps_init(GpsCallbacks* callbacks)
{
    GpsState*  s = _gps_state;
    s->callbacks = *callbacks;

    if (!s->init)
        gps_state_init(s);

    if (s->fd < 0)
        return -1;

    //s->callbacks = *callbacks;

    return 0;
}

static void
mtk_gps_gps_cleanup(void)
{
    GpsState*  s = _gps_state;

	TRC();
    if (s->init)
        gps_state_done(s);
}


static int
mtk_gps_gps_start()
{
    GpsState*  s = _gps_state;
    int err;

    if (!s->init) {
        ERR("%s: called with uninitialized state !!", __FUNCTION__);
        return -1;
    }
    if ((err = mtk_gps_start())) {
        ERR("mtk_gps_start err = %d", err);
        return -1;
    }
        
    TRC();
    gps_state_start(s);
    return 0;
}


static int
mtk_gps_gps_stop()
{
    GpsState*  s = _gps_state;
    int err;

    if (!s->init) {
        ERR("%s: called with uninitialized state !!", __FUNCTION__);
        return -1;
    }
    if ((err = mtk_gps_stop())) {
        ERR("mtk_gps_stop err = %d", err);
        return -1;
    }

    TRC();
    gps_state_stop(s);
    return 0;
}



static int
mtk_gps_gps_inject_time(GpsUtcTime time, int64_t timeReference, int uncertainty)
{
    TRC();
    return 0;
}

static int
mtk_gps_gps_inject_location(double latitude, double longitude, float accuracy)
{
    return 0;
}

static void
mtk_gps_gps_delete_aiding_data(GpsAidingData flags)
{    
    /*workaround to trigger hot/warm/cold/full start*/
    #define FLAG_HOT_START  GPS_DELETE_RTI
    #define FLAG_WARM_START GPS_DELETE_EPHEMERIS     
    #define FLAG_COLD_START (GPS_DELETE_EPHEMERIS | GPS_DELETE_POSITION | GPS_DELETE_TIME | GPS_DELETE_IONO | GPS_DELETE_UTC | GPS_DELETE_HEALTH)
    #define FLAG_FULL_START (GPS_DELETE_ALL)
    #define FLAG_AGPS_START (GPS_DELETE_EPHEMERIS | GPS_DELETE_ALMANAC | GPS_DELETE_POSITION | GPS_DELETE_TIME | GPS_DELETE_IONO | GPS_DELETE_UTC)
    GpsState*  s = _gps_state;

    DBG("%s:0x%X\n", __FUNCTION__, flags);

    gps_state_restart(s);

    if (flags == FLAG_HOT_START)
        mtk_gps_restart(MNL_CMD_RESTART_HOT);
    else if (flags == FLAG_WARM_START)
        mtk_gps_restart(MNL_CMD_RESTART_WARM);
    else if (flags == FLAG_COLD_START)
        mtk_gps_restart(MNL_CMD_RESTART_COLD);
    else if (flags == FLAG_FULL_START)
        mtk_gps_restart(MNL_CMD_RESTART_FULL);
    else if(flags == FLAG_AGPS_START)
    {
    	ERR("Send MNL_CMD_RESTART_AGPS in HAL\n");	
        mtk_gps_restart(MNL_CMD_RESTART_AGPS);
    }
}

static int mtk_gps_gps_set_position_mode(GpsPositionMode mode, GpsPositionRecurrence recurrence,
            uint32_t min_interval, uint32_t preferred_accuracy, uint32_t preferred_time)
{
    // FIXME - support fix_frequency
    // only standalone supported for now.
    TRC();
    return 0;
}

static const void*
mtk_gps_gps_get_extension(const char* name)
{
    TRC();
    return NULL;
}

int
mtk_gps_gps_sys_read_lock(int fd, off_t offset, int whence, off_t len){   

    struct flock lock;
    	
    lock.l_type = F_RDLCK;                                                       
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;
	
    if (fcntl(fd, F_SETLK, &lock) < 0 ){
		return -1;
    }
	
    return 0;	
}

int
mtk_gps_gps_sys_write_lock(int fd, off_t offset, int whence, off_t len){
	struct flock lock;
	
	lock.l_type = F_WRLCK;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;
	if (fcntl(fd, F_SETLK, &lock) < 0){
		return -1;
	}
	
	return 0;
}

static unsigned int
mtk_gps_gps_sys_get_file_size() {
    unsigned int fileSize;
    struct stat st;

    if(stat(EPO_FILE, &st) < 0) {
		ERR("Get file size error, return\n");
		return 0;
    	}
	
    fileSize = st.st_size;	
    DBG("EPO file size: %d\n", fileSize);
    return fileSize;
       	
    //fseek(pFile, 0L, SEEK_END); //reset the current pointer to the end of file
    //fileSize = ftell(pFile); 
    //fseek(pFile, 0, SEEK_SET);

    //return fileSize;	
}
void GpsToUtcTime(int i2Wn, double dfTow, time_t* uSecond)
{
    struct tm target_time;
    int iYearsElapsed;     // Years since 1980.
    unsigned int iDaysElapsed;      // Days elapsed since Jan 1, 1980.
    double dfSecElapsed;
    unsigned int fgLeapYear; 
    int pi2Yr;
    int pi2Mo;
    int pi2Day;
    int pi2Hr;
    int pi2Min;
    double pdfSec; 	
    int i;  
  

  // Number of days into the year at the start of each month (ignoring leap
  // years).
  unsigned int doy[12] = {0,31,59,90,120,151,181,212,243,273,304,334};

  // Convert time to GPS weeks and seconds
  iDaysElapsed = i2Wn * 7 + ((int)dfTow / 86400) + 5;
  dfSecElapsed = dfTow - ((int)dfTow / 86400) * 86400;


  // decide year
  iYearsElapsed = 0;    // from 1980
  while(iDaysElapsed >= 365)
  {
    if((iYearsElapsed % 100) == 20) // if year % 100 == 0
    {
      if((iYearsElapsed % 400) == 20) // if year % 400 == 0
      {
        if (iDaysElapsed >= 366)
        {
          iDaysElapsed -= 366;
        }
        else
        {
          break;
        }
      }
      else
      {
        iDaysElapsed -= 365;
      }
    }
    else if((iYearsElapsed % 4) == 0) // if year % 4 == 0
    {
        if (iDaysElapsed >= 366)
        {
          iDaysElapsed -= 366;
        }
        else
        {
          break;
        }
    }
    else
    {
        iDaysElapsed -= 365;
    }
    iYearsElapsed++;
  }
  pi2Yr = 1980 + iYearsElapsed;


  // decide month, day
  fgLeapYear = 0;
  if((iYearsElapsed % 100) == 20) // if year % 100 == 0
  {
    if((iYearsElapsed % 400) == 20) // if year % 400 == 0
    {
      fgLeapYear = 1;
    }
  }
  else if((iYearsElapsed % 4) == 0) // if year % 4 == 0
  {
    fgLeapYear = 1;
  }

  if(fgLeapYear)
  {
    for(i = 2; i < 12; i++)
    {
      doy[i] += 1;
    }
  }
  for(i = 0; i < 12; i++)
  {
    if(iDaysElapsed < doy[i])
    {
      break;
    }
  }
  pi2Mo = i;
  pi2Day =iDaysElapsed - doy[i-1] + 1;


  // decide hour, min, sec
  pi2Hr = dfSecElapsed / 3600;
  pi2Min = ((int)dfSecElapsed % 3600) / 60;
  pdfSec = dfSecElapsed - ((int)dfSecElapsed / 60) * 60;

    //change the UTC time to seconds
    memset(&target_time, 0, sizeof(target_time));
    target_time.tm_year = pi2Yr - 1900;
    target_time.tm_mon = pi2Mo - 1;
    target_time.tm_mday = pi2Day;
    target_time.tm_hour = pi2Hr;
    target_time.tm_min = pi2Min;
    target_time.tm_sec = pdfSec;
    target_time.tm_isdst = -1;
    DBG("target_time.tm_year = %d, month = %d, day = %d, hour = %d, min = %d, sec = %d, tm_isdst = %d\n", 
        target_time.tm_year, target_time.tm_mon, target_time.tm_mday, target_time.tm_hour, target_time.tm_min, target_time.tm_sec, target_time.tm_isdst);
    *uSecond = mktime(&target_time);
    if (*uSecond < 0){
		ERR("Convert UTC time to seconds fail, return\n");
    }
    
}


static int
mtk_gps_gps_sys_epo_period_start(int fd, unsigned int* u4GpsSecs, time_t* uSecond){      //no file lock                  
    char szBuf[MTK_EPO_ONE_SV_SIZE];    
    int pi2WeekNo; 
    unsigned int pu4Tow;
    	
	
    //if (fread(szBuf, 1, MTK_EPO_ONE_SV_SIZE, pFile) != MTK_EPO_ONE_SV_SIZE) {
    if(read(fd, szBuf, MTK_EPO_ONE_SV_SIZE) != MTK_EPO_ONE_SV_SIZE){
        return -1;
    }

    *u4GpsSecs = (((*(unsigned int*)(&szBuf[0])) & 0x00FFFFFF) *3600);                                         
    pi2WeekNo = (*u4GpsSecs) / 604800;
    pu4Tow = (*u4GpsSecs) % 604800;
    
    TRC();
    DBG("pi2WeekNo = %d, pu4Tow = %d\n", pi2WeekNo, pu4Tow);
    GpsToUtcTime(pi2WeekNo, pu4Tow, uSecond);//to get UTC second	
    return 0;	
}


static int
mtk_gps_gps_sys_epo_period_end(int fd, unsigned int *u4GpsSecs, time_t* uSecond) {        //no file lock   
    int fileSize;
    char szBuf[MTK_EPO_ONE_SV_SIZE]; 
    int pi2WeekNo; 
    unsigned int pu4Tow;	

    fileSize = mtk_gps_gps_sys_get_file_size();
    if(fileSize < MTK_EPO_ONE_SV_SIZE) {
		return -1;
    }

    lseek(fd, (fileSize - MTK_EPO_ONE_SV_SIZE), SEEK_SET);

    if(read(fd, szBuf, MTK_EPO_ONE_SV_SIZE) != MTK_EPO_ONE_SV_SIZE) {
   	return -1;
   }

    *u4GpsSecs = (((*(unsigned int*)(&szBuf[0])) & 0x00FFFFFF) *3600);                                     
    (*u4GpsSecs) += 21600;

    pi2WeekNo = (*u4GpsSecs) / 604800;
    pu4Tow = (*u4GpsSecs) % 604800;
    
    TRC();
    DBG("pi2WeekNo = %d, pu4Tow = %d\n", pi2WeekNo, pu4Tow);
    GpsToUtcTime(pi2WeekNo, pu4Tow, uSecond);		

    return 0;	 
}

int
mtk_gps_gps_epo_file_time_hal(long long uTime[]) {
    
    TRC();
    struct stat filestat;
    int fd = 0;
    int addLock;	
    unsigned int u4GpsSecs_start; //GPS seconds
    unsigned int u4GpsSecs_expire;
    time_t uSecond_start;   //UTC seconds
    time_t uSecond_expire;	

    //open file
    fd = open(EPO_FILE, O_RDONLY);
    if(-1 == fd) {
		DBG("Open /data/misc/EPO.data fail, return");
		return -1;
    	}

   //Add file lock 
   if(mtk_gps_gps_sys_read_lock(fd, 0, SEEK_SET, 0) < 0){
   	ERR("Add read lock failed, return\n");
       close(fd);
	return -1;
   }
       
    //EPO start time
    if(mtk_gps_gps_sys_epo_period_start(fd, &u4GpsSecs_start, &uSecond_start)) {
		ERR("Get EPO file start time error, return\n");
		close(fd);
		return -1;                                                                                  
    	}else{
              uTime[0] = (long long)uSecond_start;   	    
    	       DBG("The Start time of EPO file is %lld", uTime[0]);
               DBG("The start time of EPO file is %s", ctime(&uTime[0]));
    	}
			   
    //download time	
    stat(EPO_FILE, &filestat);
    uTime[1] = (long long)(filestat.st_mtime);
    //uTime[1] = uTime[1] - 8 * 3600;    
    DBG("Download time of EPO file is %lld", uTime[1]);
    DBG("Download time of EPO file is %s\n", ctime(&uTime[1]));		
    
    //EPO file expire time    
    if(mtk_gps_gps_sys_epo_period_end(fd, &u4GpsSecs_expire, &uSecond_expire)){
            ERR("Get EPO file expire time error, return\n");
	     close(fd);	
	     return -1;
	}else {
	     uTime[2] = (long long)uSecond_expire;
	     DBG("The expire time of EPO file is %lld", uTime[2]);
             DBG("The expire time of EPO file is %s", ctime(&uTime[2]));
	}

    close(fd);
    return 0;
}


int
mtk_gps_gps_epo_file_update_hal(){
    TRC();
    int fd_write, fd_read;
    char buf[BUF_SIZE];
    ssize_t bytes_read;
    ssize_t bytes_write;
    int result;
  
    DBG("Update EPO file...\n");

    if(access(EPO_FILE, 0) == -1){
        DBG("EPO file does not exist, rename it directly\n");
        result = rename(EPO_FILE_NEW, EPO_FILE);
        if (!result){
            DBG("Rename files success");
            return 0;
        }
     }

    //Add write lock to "/data/misc/EPO.dat" 
    fd_write = open(EPO_FILE, O_WRONLY | O_TRUNC);
    if(fd_write < 0){
	ERR("open /data/misc/EPO.dat error!\n");		
	return -1;
    }
 
   if(mtk_gps_gps_sys_write_lock(fd_write, 0, SEEK_SET, 0) < 0){
        ERR("Add read lock failed, return\n");
        close(fd_write);	   
	 return -1;
   }


    //Add read lock to "EPO30.dat"	
    fd_read = open(EPO_FILE_NEW, O_RDONLY);
    if(fd_read < 0){
	DBG("open /data/misc/EPO30.dat error!\n");		
	return -1;
    }
	
    if(mtk_gps_gps_sys_read_lock(fd_read, 0, SEEK_SET, 0) < 0){
   	ERR("Add read lock failed, return\n");
	close(fd_read);
	return -1;		
    }

    //start copy new data from EPO30.dat to EPO.dat	
    while((bytes_read = read(fd_read, buf, BUF_SIZE)) > 0){                                              
		bytes_write = write(fd_write, buf, bytes_read);
		if(bytes_write != bytes_read) {
                ERR("Copy file error\n");
		  close(fd_read);
		  close(fd_write);
		  return -1;
		}
    }

    //release write and read lock
    close(fd_read);
    close(fd_write);	
    DBG("Updates EPO file done\n");
    return 0;
 }

enum {
    HAL_CMD_STOP_UNKNOWN = -1,
    HAL_CMD_READ_EPO_TIME_DONE = 0x35,
    HAL_CMD_UPDATE_EPO_FILE_DONE = 0x36,
    
    HAL_CMD_READ_EPO_TIME_FAIL = 0x37,
    HAL_CMD_UPDATE_EPO_FILE_FAIL = 0x38,
};

int mtk_gps_gps_epo_file_time(long long uTime[])
{
    TRC();
    char cmd = HAL_CMD_STOP_UNKNOWN;
    int ret;
    struct stat filestat; 
    DBG("sta.status = %d\n", sta.status);	
    if(sta.status == GPS_STATUS_ENGINE_ON || sta.status == GPS_STATUS_SESSION_BEGIN) {    
	 DBG("GPS driver is running, read epo time via GPS driver\n");         
        //send cmd to MNLD        
        char buf[] = {MNL_CMD_READ_EPO_TIME};   
        ret = mtk_gps_daemon_send(buf, sizeof(buf));   
        if (-1 == ret){
            ERR("Request read epo time fail\n");
	     return ret;		
        }else{
	     //send cmd success, wait to read from socket
	     DBG("Request read epo time successfully\n");
	     //usleep(100000);                                                          
	     
            ret = read(mtk_gps_gps.sock, &cmd, sizeof(cmd));
            if(cmd == HAL_CMD_READ_EPO_TIME_DONE){
                DBG("HAL_CMD_READ_EPO_TIME_DONE\n");
	         time_t msg[2];

	         do { 
	             ret = read(mtk_gps_gps.sock, msg, sizeof(msg));
                    DBG("Read EPO time from mtk_gps_gps.sock, ret = %d\n", ret);
	         } while (ret < 0 && errno == EINTR);

	         DBG("msg[0] = %ld, msg[1] = %ld", msg[0], msg[1]);
            
	         if(ret == 0) {
                    ERR("Remote socket closed\n");
	             return -1;	   
	         }
            
	         if(ret == sizeof(msg)) {		
                    //start time
	             uTime[0] = msg[0];
	             DBG("Start time of EPO file is %lld", uTime[0]);
	             DBG("Start time of EPO file is %s\n", ctime(&uTime[0]));
		  
                    //download time
                    stat(EPO_FILE, &filestat);
                    uTime[1] = (long long)(filestat.st_mtime);
                    //uTime[1] = uTime[1] - 8 * 3600;    
                    DBG("Download time of EPO file is %lld", uTime[1]);
                    DBG("Download time of EPO file is %s\n", ctime(&uTime[1]));	

	             //expire time	
	             uTime[2] = msg[1];
	             DBG("Download time of EPO file is %lld", uTime[2]);
                    DBG("Download time of EPO file is %s\n", ctime(&uTime[2]));
	             return 0;
	         } else {
                    ERR("Read time fail\n");
	             return -1;		   
	         }
          	  //return 0;		
             }else if(cmd == HAL_CMD_READ_EPO_TIME_FAIL){
                ERR("Read EPO time fail\n");
	         return -1;			 
	     }	
          }        
      }else{
          //GPS driver is not running, read epo file time in HAL.
          DBG("GPS driver is not running, read epo file time in HAL\n");
          ret = mtk_gps_gps_epo_file_time_hal(uTime);
      }
    return ret;	
}
	
int mtk_gps_gps_epo_file_update(){
    int ret;
    DBG("sta.status = %d\n", sta.status);	
    if(sta.status == GPS_STATUS_ENGINE_ON || sta.status == GPS_STATUS_SESSION_BEGIN) {
        //send cmd to MNLD        
        char cmd = HAL_CMD_STOP_UNKNOWN;
 
        DBG("GPS driver is running, update epo file via GPS driver\n");
	char buf[] = {MNL_CMD_UPDATE_EPO_FILE};
	ret = mtk_gps_daemon_send(buf, sizeof(buf));
	      
        if(-1 == ret){
            ERR("Request update epo file fail\n");
	 }else{
	     //send cmd success, wait to read from socket
            DBG("Request update epo file successfully\n");
            //usleep(100000);                                                         
			
	     ret = read(mtk_gps_gps.sock, &cmd, sizeof(cmd));
	     if(cmd == HAL_CMD_UPDATE_EPO_FILE_DONE){
                DBG("Update EPO file successfully\n");
		  return 0;		
            }else if (cmd == HAL_CMD_UPDATE_EPO_FILE_FAIL){
                ERR("Update EPO file fail\n");
	         return -1;			
	     }     	 	
	 }	 	
    } else {
        //GPS driver is not running, update epo file in HAL
        DBG("GPS driver is not running, update epo file in HAL\n");
        ret = mtk_gps_gps_epo_file_update_hal();
    }
    return ret;
}

static const GpsInterface  mtk_gpsGpsInterface = {
    sizeof(GpsInterface),
    mtk_gps_gps_init,
    mtk_gps_gps_start,
    mtk_gps_gps_stop,
    mtk_gps_gps_cleanup,
    mtk_gps_gps_inject_time,
    mtk_gps_gps_inject_location,
    mtk_gps_gps_delete_aiding_data,
    mtk_gps_gps_set_position_mode,
    mtk_gps_gps_get_extension,
    mtk_gps_gps_epo_file_time,
    mtk_gps_gps_epo_file_update,    
};

const GpsInterface* gps__get_gps_interface(struct gps_device_t* dev)
{
	  DBG("gps__get_gps_interface HAL\n");
    return &mtk_gpsGpsInterface;
}

static int open_gps(const struct hw_module_t* module, char const* name,
        struct hw_device_t** device)
{
	  DBG("open_gps HAL 1\n");
    struct gps_device_t *dev = malloc(sizeof(struct gps_device_t));
    memset(dev, 0, sizeof(*dev));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t*)module;
//    dev->common.close = (int (*)(struct hw_device_t*))close_lights;
    DBG("open_gps HAL 2\n");
    dev->get_gps_interface = gps__get_gps_interface;
    DBG("open_gps HAL 3\n");
    *device = (struct hw_device_t*)dev;
    return 0;
}


static struct hw_module_methods_t gps_module_methods = {
    .open = open_gps
};


struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .version_major = 1,
    .version_minor = 0,
    .id = GPS_HARDWARE_MODULE_ID,
    .name = "Hardware GPS Module",
    .author = "The MTK GPS Source Project",
    .methods = &gps_module_methods,
};

/*
const GpsInterface* gps_get_hardware_interface()
{
    TRC();	
    return &mtk_gpsGpsInterface;
}
*/

