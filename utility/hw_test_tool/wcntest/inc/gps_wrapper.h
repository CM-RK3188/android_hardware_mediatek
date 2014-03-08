#ifndef __GPS_WRAPPER_H__
#define __GPS_WRAPPER_H__
#include "wcn_cmd.h"

int gps_open();
int gps_hstart();
int gps_wstart();
int gps_cstart();
int gps_fstart();
int gps_close();
int gps_getver();

#define GPS_TEST_TBL \
    {"gopen",  0,  gps_open,   {NULL, NULL, NULL}, "eg: open, desc: open gps driver"}, \
    {"ghttff", 0,  gps_hstart, {NULL, NULL, NULL}, "eg: restart, desc: Hot restart gps"}, \
    {"gwttff", 0,  gps_wstart, {NULL, NULL, NULL}, "eg: restart, desc: Warm restart gps"}, \
    {"gcttff", 0,  gps_cstart, {NULL, NULL, NULL}, "eg: restart, desc: Cold restart gps"}, \
    {"gfttff", 0,  gps_fstart, {NULL, NULL, NULL}, "eg: restart, desc: Full restart gps"}, \
    {"ggetver",0,  gps_getver, {NULL, NULL, NULL}, "eg: get version, desc: Get gps chip and gps driver version"}, \
    {"gclose", 0,  gps_close,  {NULL, NULL, NULL}, "eg: close, desc: close gps driver"}


#endif //__GPS_WRAPPER_H__