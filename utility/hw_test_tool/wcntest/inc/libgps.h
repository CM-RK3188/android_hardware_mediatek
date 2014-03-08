
#ifndef __LIBGPS_H__
#define __LIBGPS_H__

/*****************************************************************************
 * FUNCTION
 *     ENG_GPS_Open
 * DESCRIPTION
 *     Open GPS driver
 * PARAMETERS
 *     None  
 * RETURNS
 *     success: 0; failure: -1
 *****************************************************************************/
int
ENG_GPS_Open ();


/*****************************************************************************
 * FUNCTION
 *     ENG_GPS_Restart
 * DESCRIPTION
 *     Restart GPS driver: H/W/C/F start 
 * PARAMETERS
 *     GPS_HOT_RESTART
 *     GPS_WARM_RESTART
 *     GPS_COLD_RESTART
 *     GPS_FULL_RESTART
 * RETURNS
 *     success: 0; failure: -1
 *****************************************************************************/
int
ENG_GPS_Restart(int GPS_RESTART);


/*****************************************************************************
 * FUNCTION
 *     ENG_GPS_Close
 * DESCRIPTION
 *     Close GPS driver
 * PARAMETERS
 *     None  
 * RETURNS
 *     success: 0; failure: -1
 *****************************************************************************/
int
ENG_GPS_Close ();

/*****************************************************************************
 * FUNCTION
 *     ENG_GPS_Getver
 * DESCRIPTION
 *     Get gps device and gps driver version
 * PARAMETERS
 *     None  
 * RETURNS
 *     success: 0; failure: -1
 *****************************************************************************/
int
ENG_GPS_Getver();

#endif //__LIBGPS_H__