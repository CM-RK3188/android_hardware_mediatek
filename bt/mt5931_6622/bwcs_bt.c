#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>


#include "bt_hci_bdroid.h"
#include "bt_vendor_lib.h"
#include "utils.h"

#include "bwcs.h"
#include "bt_sock.h"

/* bt status thread control block */

typedef struct
{
    pthread_t       bt_status_thread;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
} bt_status_cb_t;

/******************************************************************************
**  Static Variables
******************************************************************************/


bt_vendor_callbacks_t *bwcs_bt_vnd_cbacks = NULL;

static bt_status_cb_t	bt_status_cb;
static volatile UCHAR	bt_running   = 0;
static volatile USHORT	ready_events = 0;
static BWCS_BT bwcs_action;

/* WiFi Channel Info, BW is 20M */
static char AFH_Mask_Ch_Info[15][11] =
{
    {0x00, 0x00, 0xF0, 0xFF, 0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF},/* WiFi Channel 1: 2402~2421, BT Chl 1~20  */
    {0x1F, 0x00, 0x00, 0xFE, 0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF},/* WiFi Channel 2: 2407~2426, BT Chl 6~25  */
    {0xFF, 0x03, 0x00, 0xC0, 0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF},/* WiFi Channel 3: 2412~2431, BT Chl 11~30 */
    {0xFF, 0x7F, 0x00, 0x00, 0xF8, 0xFF,0xFF,0xFF,0xFF,0xFF},/* WiFi Channel 4: 2417~2436, BT Chl 16~35 */
    {0xFF, 0xFF, 0x0F, 0x00, 0x00, 0xFF,0xFF,0xFF,0xFF,0xFF},/* WiFi Channel 5: 2422~2441, BT Chl 21~40 */
    {0xFF, 0xFF, 0xFF, 0x01, 0x00, 0xE0,0xFF,0xFF,0xFF,0xFF},/* WiFi Channel 6: 2427~2446, BT Chl 26~45 */
    {0xFF, 0xFF, 0xFF, 0x3F, 0x00, 0x00,0xFC,0xFF,0xFF,0xFF},/* WiFi Channel 7: 2432~2451, BT Chl 31~50 */
    {0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x00,0x80,0xFF,0xFF,0xFF},/* WiFi Channel 8: 2437~2456, BT Chl 36~55 */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,0x00,0xF0,0xFF,0xFF},/* WiFi Channel 9: 2442~2461, BT Chl 41~60 */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F,0x00,0x00,0xFC,0xFF},/* WiFi Channel 10:2447~2466, BT Chl 46~65 */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,0x03,0x00,0xC0,0xFF},/* WiFi Channel 11:2452~2471, BT Chl 51~70 */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,0x7F,0x00,0x00,0xF8},/* WiFi Channel 12:2457~2476, BT Chl 56~75 */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,0xFF,0x0F,0x00,0x00},/* WiFi Channel 13:2462~2481, BT Chl 61~80 */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,0xFF,0xFF,0xFF,0x00},/* WiFi Channel 14:2474~2493, BT Chl 73~79 */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF} /* If No WiFi Channel, Set to All 0xFF, means do not mask */
};
/******************************************************************************
**  Functions
******************************************************************************/
static void * bt_status_thread(void *arg);
void bt_status_event(USHORT event);

void bwcs_bt_init(const bt_vendor_callbacks_t* p_cb)
{
    pthread_attr_t thread_attr;
    struct sched_param param;//todo: need it or not
    
    bwcs_bt_vnd_cbacks = p_cb;
    pthread_mutex_init(&bt_status_cb.mutex, NULL);
    pthread_cond_init(&bt_status_cb.cond, NULL);
    pthread_attr_init(&thread_attr);

    if (pthread_create(&bt_status_cb.bt_status_thread, &thread_attr, \
                       bt_status_thread, NULL) != 0)
    {
        BWCS_ERR(BWCS_LOG_TAG"pthread_create failed! errno:%d, err string:%s", errno, strerror(errno));
        bt_running = 0;
//        return 0;
    }

/*  //todo: not really know about this
    if(pthread_getschedparam(hc_cb.worker_thread, &policy, &param)==0)
    {
        policy = BTHC_LINUX_BASE_POLICY;
#if (BTHC_LINUX_BASE_POLICY!=SCHED_NORMAL)
        param.sched_priority = BTHC_MAIN_THREAD_PRIORITY;
#endif
        result = pthread_setschedparam(hc_cb.worker_thread, policy, &param);
        if (result != 0)
        {
            ALOGW("libbt-hci init: pthread_setschedparam failed (%s)", \
                  strerror(result));
        }
    }
*/
//    return TRUE;
}

void bwcs_bt_cleanup(void)
{
    //do cleanup things including<socket release, two thread join, & so on>
    bwcs_bt_vnd_cbacks = NULL;
    memset(&bwcs_action, 0, sizeof(BWCS_BT));
    destroy_sock_conn();
}

/* deliver action from bwcs to main thread */
void bwcs_action_notify(USHORT action, BWCS_BT bb)
{
    bwcs_action = bb;  
    pthread_mutex_lock(&bt_status_cb.mutex);
    ready_events |= action;

    pthread_cond_signal(&bt_status_cb.cond);
    pthread_mutex_unlock(&bt_status_cb.mutex);
}

void bt_status_event(USHORT event)
{
    pthread_mutex_lock(&bt_status_cb.mutex);
    ready_events |= event;
    pthread_cond_signal(&bt_status_cb.cond);
    pthread_mutex_unlock(&bt_status_cb.mutex);
}


/* BWCS related Command */
static void GORMcmd_HCC_Set_PTA(BOOL on);   	/*0xFC74*/
static void GORMcmd_HCC_Enable_PTA(BOOL on);	/*0xFCD2*/
static void GORMcmd_HCC_Set_WiFi_Ch(void);	/*0xFCD3*/
static void GORMcmd_HCC_Set_AFH_Mask(void);	/*0x0C3F*/
static void GORMcmd_HCC_Set_Radio(UCHAR init_power, UCHAR max_power);		/*0xFC79*/
static void GORMevt_HCE_Common_Complete(void *p_evt);   /*event*/


static void GORMevt_HCE_Common_Complete(void *p_evt)
{
    HC_BT_HDR *p_buf = (HC_BT_HDR *)p_evt;
    UCHAR *p;
    UCHAR event, status;
    USHORT opcode;
    BOOL success;
    
    BWCS_DBG(BWCS_LOG_TAG "GORMevt_HCE_Common_Complete\n");
    
    p = (uint8_t *)(p_buf + 1);
    event = *p;
    p += 3;
    STREAM_TO_UINT16(opcode, p);
    status = *p;


    BWCS_DBG(BWCS_LOG_TAG "event =0X%02X, opcode=:0X%04X, status=%d\n", event, opcode, status);    
    if (bwcs_bt_vnd_cbacks){
	bwcs_bt_vnd_cbacks->dealloc(p_buf);
    }
}

/* this cmd function should be modified */	    
static void GORMcmd_HCC_Set_PTA( BOOL on )
{
    HC_BT_HDR *p_cmd = NULL;
    UCHAR *p, ret;
    USHORT wOpCode = 0xFC74;

    if (bwcs_bt_vnd_cbacks){
	p_cmd = (HC_BT_HDR *)bwcs_bt_vnd_cbacks->alloc(BT_HC_HDR_SIZE + \
                                                          HCI_CMD_MAX_SIZE);
    } else {
	BWCS_ERR(BWCS_LOG_TAG "No libbt-hci callbacks!\n");
    }
        
    if (p_cmd)
    {
	p_cmd->event = MSG_STACK_TO_HC_HCI_CMD;
	p_cmd->offset = 0;
	p_cmd->layer_specific = 0;
            
	p_cmd->len = 13;
	p = (uint8_t *)(p_cmd + 1);
	UINT16_TO_STREAM(p, wOpCode);
	*p++ = 10;

	//params
	*p++ = 0xCF;
	*p++ = 0x8B;
	*p++ = 0x1F;
	if(on)
	    *p++ = 0x02;	    /* 02:for MT5921; 04:for MT5931 *///wenhui change to 02 for testing bwcs
	else
	    *p++ = 0x04;
	*p++ = 0x08;
	*p++ = 0xA2;
	*p++ = 0x62;
	*p++ = 0x56;
	*p++ = 0x07;
	*p++ = 0x1B;

	BWCS_DBG(BWCS_LOG_TAG "GORMcmd_HCC_Set_PTA, para:%d\n", on);
	if (bwcs_bt_vnd_cbacks){
	    ret = bwcs_bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
	}
	else{
	    BWCS_ERR(BWCS_LOG_TAG "No HCI packet transmit callback\n");
            ret = FALSE;
	}

	if(FALSE == ret) {
	    BWCS_ERR(BWCS_LOG_TAG "Send Set PTA command fails\n");
	    if (bwcs_bt_vnd_cbacks){
		bwcs_bt_vnd_cbacks->dealloc(p_cmd);
	    }
	}

    } else {
	BWCS_ERR(BWCS_LOG_TAG "Alloc command buffer fails\n");
    }
}

static void GORMcmd_HCC_Enable_PTA( BOOL on)
{
    HC_BT_HDR *p_cmd = NULL;
    UCHAR *p, ret;
    USHORT wOpCode = 0xFCD2;

    if (bwcs_bt_vnd_cbacks){
	p_cmd = (HC_BT_HDR *)bwcs_bt_vnd_cbacks->alloc(BT_HC_HDR_SIZE + \
                                                          HCI_CMD_MAX_SIZE);
    } else {
	BWCS_ERR(BWCS_LOG_TAG "No libbt-hci callbacks!\n");
    }
        
    if (p_cmd)
    {
	p_cmd->event = MSG_STACK_TO_HC_HCI_CMD;
	p_cmd->offset = 0;
	p_cmd->layer_specific = 0;

	p_cmd->len = 4;
	p = (uint8_t *)(p_cmd + 1);
	UINT16_TO_STREAM(p, wOpCode);
	*p++ = 1;

	//params
	if(on)
	    *p++ = 0x01;	    /* 01 open, 00 close *///wenhui change to 02 for testing bwcs
	else
	    *p++ = 0x00;

	BWCS_DBG(BWCS_LOG_TAG "GORMcmd_HCC_Enable_PTA, para:%d\n", on);
	if (bwcs_bt_vnd_cbacks){
	    ret = bwcs_bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
	}
	else{
	    BWCS_ERR(BWCS_LOG_TAG "No HCI packet transmit callback\n");
            ret = FALSE;
	}

	if(FALSE == ret) {
	    BWCS_ERR(BWCS_LOG_TAG "Send Set PTA command fails\n");
	    if (bwcs_bt_vnd_cbacks){
		bwcs_bt_vnd_cbacks->dealloc(p_cmd);
	    }
	}

    } else {
	BWCS_ERR(BWCS_LOG_TAG "Alloc command buffer fails\n");
    }
}

//todo /* need to think aboutn chl & bandwidth */
static void GORMcmd_HCC_Set_WiFi_Ch(void)
{
    HC_BT_HDR *p_cmd = NULL;
    UCHAR *p, ret;
    USHORT wOpCode = 0xFCD3;

    UCHAR  chl_count = bwcs_action.para[0];
    UCHAR  chl_no    = bwcs_action.para[1];
    UCHAR  chl_half_bw = 0x0A;

    if( WIFI_BW_20 == bwcs_action.para[2])
    	chl_half_bw    = 0x0A;
    else if( WIFI_BW_40 == bwcs_action.para[2])
    	chl_half_bw    = 0x14;

    BWCS_DBG(BWCS_LOG_TAG "GORMcmd_HCC_Set_WiFi_Ch, para[0~7]: %02X %02X %02X %02X %02X %02X %02X %02X", bwcs_action.para[0], bwcs_action.para[1], bwcs_action.para[2], bwcs_action.para[3], bwcs_action.para[4], bwcs_action.para[5], bwcs_action.para[6], bwcs_action.para[7]);

    if (bwcs_bt_vnd_cbacks){
	p_cmd = (HC_BT_HDR *)bwcs_bt_vnd_cbacks->alloc(BT_HC_HDR_SIZE + \
                                                          HCI_CMD_MAX_SIZE);
    } else {
	BWCS_ERR(BWCS_LOG_TAG "No libbt-hci callbacks!\n");
    }
        
    if (p_cmd)
    {
	p_cmd->event = MSG_STACK_TO_HC_HCI_CMD;
	p_cmd->offset = 0;
	p_cmd->layer_specific = 0;

	p_cmd->len = 5;
	p = (uint8_t *)(p_cmd + 1);
	UINT16_TO_STREAM(p, wOpCode);
	*p++ = 2;

	if(chl_count == 0) {//todo: chl_count = 0, how to set the channel?
	    //params
            *p++ = 0X0E;  /* wifi channel no */
	    *p++ = 0; //wifi channel half bandwidth
	}
	else if(chl_count == 1) {
	    //params
            *p++ = chl_no;  /* wifi channel no */
	    *p++ = chl_half_bw; //wifi channel half bandwidth

	} else if(chl_count == 2) {//todo: if more than 1 channel
	    //params
            *p++ = chl_no;  /* wifi channel no */
	    *p++ = chl_half_bw; //wifi channel half bandwidth

	}

	BWCS_DBG(BWCS_LOG_TAG "GORMcmd_HCC_Set_WiFi_Ch, channel count:%d\n", chl_count);
	if (bwcs_bt_vnd_cbacks){
	    ret = bwcs_bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
	}
	else{
	    BWCS_ERR(BWCS_LOG_TAG "No HCI packet transmit callback\n");
            ret = FALSE;
	}

	if(FALSE == ret) {
	    BWCS_ERR(BWCS_LOG_TAG "Send Set PTA command fails\n");
	    if (bwcs_bt_vnd_cbacks){
		bwcs_bt_vnd_cbacks->dealloc(p_cmd);
	    }
	}

    } else {
	BWCS_ERR(BWCS_LOG_TAG "Alloc command buffer fails\n");
    }
}

//todo: need to think about this command
static void GORMcmd_HCC_Set_AFH_Mask(void)
{
    HC_BT_HDR *p_cmd = NULL;
    UCHAR *p, ret;
    USHORT wOpCode = 0x0C3F;

//    UCHAR  chl_count = bwcs_action.para[0];
    UCHAR  chl_no    = bwcs_action.para[1];
    UCHAR  chl_half_bw = 0x0A;

//use 20M Bandwidth as default
//    if( WIFI_BW_20 == bwcs_action.para[2])
//    	chl_half_bw    = 0x0A;
//    else if( WIFI_BW_40 == bwcs_action.para[2])
//    	chl_half_bw    = 0x14;

    if (bwcs_bt_vnd_cbacks){
	p_cmd = (HC_BT_HDR *)bwcs_bt_vnd_cbacks->alloc(BT_HC_HDR_SIZE + \
                                                          HCI_CMD_MAX_SIZE);
    } else {
	BWCS_ERR(BWCS_LOG_TAG "No libbt-hci callbacks!\n");
    }
        
    if (p_cmd)
    {
	p_cmd->event = MSG_STACK_TO_HC_HCI_CMD;
	p_cmd->offset = 0;
	p_cmd->layer_specific = 0;

	p_cmd->len = 13;
	p = (uint8_t *)(p_cmd + 1);
	UINT16_TO_STREAM(p, wOpCode);
	*p++ = 10;

    //params
    *p++ = AFH_Mask_Ch_Info[chl_no][0];
    *p++ = AFH_Mask_Ch_Info[chl_no][1];
    *p++ = AFH_Mask_Ch_Info[chl_no][2];
    *p++ = AFH_Mask_Ch_Info[chl_no][3];
    *p++ = AFH_Mask_Ch_Info[chl_no][4];
    *p++ = AFH_Mask_Ch_Info[chl_no][5];
    *p++ = AFH_Mask_Ch_Info[chl_no][6];
    *p++ = AFH_Mask_Ch_Info[chl_no][7];
    *p++ = AFH_Mask_Ch_Info[chl_no][8];
    *p++ = AFH_Mask_Ch_Info[chl_no][9];

	BWCS_DBG(BWCS_LOG_TAG "GORMcmd_HCC_Set_AFH_Mask, wenhui, chl_no:%d\n", chl_no);
	if (bwcs_bt_vnd_cbacks){
	    ret = bwcs_bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
	}
	else{
	    BWCS_ERR(BWCS_LOG_TAG "No HCI packet transmit callback\n");
            ret = FALSE;
	}

	if(FALSE == ret) {
	    BWCS_ERR(BWCS_LOG_TAG "Send Set PTA command fails\n");
	    if (bwcs_bt_vnd_cbacks){
		bwcs_bt_vnd_cbacks->dealloc(p_cmd);
	    }
	}

    } else {
	BWCS_ERR(BWCS_LOG_TAG "Alloc command buffer fails\n");
    }

}

static void GORMcmd_HCC_Set_Radio(UCHAR init_power, UCHAR max_power)
{
//{0x07, 0x80, 0x00, 0x06, 0x03, 0x07},
/* todo: first judge if power is in [1~7] */

    HC_BT_HDR *p_cmd = NULL;
    UCHAR *p, ret;
    USHORT wOpCode = 0xFC79;

    if (bwcs_bt_vnd_cbacks){
	p_cmd = (HC_BT_HDR *)bwcs_bt_vnd_cbacks->alloc(BT_HC_HDR_SIZE + \
                                                          HCI_CMD_MAX_SIZE);
    } else {
	BWCS_ERR(BWCS_LOG_TAG "No libbt-hci callbacks!\n");
    }
        
    if (p_cmd)
    {
	p_cmd->event = MSG_STACK_TO_HC_HCI_CMD;
	p_cmd->offset = 0;
	p_cmd->layer_specific = 0;

	p_cmd->len = 9;
	p = (uint8_t *)(p_cmd + 1);
	UINT16_TO_STREAM(p, wOpCode);
	*p++ = 6;

    //params
    *p++ = init_power;  // default power level
    *p++ = 0x80;
    *p++ = 0x00;
    *p++ = 0x06;
    *p++ = 0x03;
    *p++ = max_power;  // max power level

	BWCS_DBG(BWCS_LOG_TAG "GORMcmd_HCC_Set_Radio, init_power: %d, max_power: %d\n", init_power, max_power);
	if (bwcs_bt_vnd_cbacks){
	    ret = bwcs_bt_vnd_cbacks->xmit_cb(wOpCode, p_cmd, GORMevt_HCE_Common_Complete);
	}
	else{
	    BWCS_ERR(BWCS_LOG_TAG "No HCI packet transmit callback\n");
            ret = FALSE;
	}

	if(FALSE == ret) {
	    BWCS_ERR(BWCS_LOG_TAG "Send Set PTA command fails\n");
	    if (bwcs_bt_vnd_cbacks){
		bwcs_bt_vnd_cbacks->dealloc(p_cmd);
	    }
	}

    } else {
	BWCS_ERR(BWCS_LOG_TAG "Alloc command buffer fails\n");
    }

}

void bt_action( USHORT action)
{
    BWCS_DBG(BWCS_LOG_TAG "bt_action: 0X%04X", action);

    switch (action){//todo: modify it for bluedroid & mt6622

	case BT_ACTION_SCATTER_INQUIRY://todo: enqueue cmd to tx_q
	    BWCS_DBG(BWCS_LOG_TAG "action is BT_ACTION_SCATTER_INQUIRY: 0X%04X, parameter:0X%02X", action, bwcs_action.para[0]);
	        GORMcmd_HCC_Set_PTA(bwcs_action.para[0]);
		GORMcmd_HCC_Enable_PTA(bwcs_action.para[0]);
	    break;

	case BT_ACTION_WIFI_CHL://todo: enqueue cmd to tx_q
	    BWCS_DBG(BWCS_LOG_TAG "action is BT_ACTION_WIFI_CHL:AFH_MASK 0X%04X, parameter:0X%02X", action, bwcs_action.para[0]);
	    GORMcmd_HCC_Set_AFH_Mask();
	    //GORMcmd_HCC_Set_WiFi_Ch();
	    break;

	case BT_ACTION_POWER_CTRL:
	    BWCS_DBG(BWCS_LOG_TAG "action is BT_ACTION_POWER_CTRL: 0X%04X, parameter:0X%02X", action, bwcs_action.para[0]);
	    GORMcmd_HCC_Set_Radio(bwcs_action.para[0], bwcs_action.para[1]);
	    break;

	default:
	    BWCS_DBG(BWCS_LOG_TAG "unknown action: 0X%04X", action);
	    break;
    }
    //todo:
}
static void init()
{
    /* first setup socket */
    setup_sock_conn();
}

static void * bt_status_thread(void *arg)
{
    BWCS_DBG("bwcs client start");
    int event;
    bt_running = 1;

    init();//setup staffs should add to init function

    while(bt_running){

	pthread_mutex_lock(&bt_status_cb.mutex);
        while (ready_events == 0)//wait for event to come
        {
            pthread_cond_wait(&bt_status_cb.cond, &bt_status_cb.mutex);
        }
        event = ready_events;
        ready_events = 0;
        pthread_mutex_unlock(&bt_status_cb.mutex);

	if (event & BT_EVENT_BT_OPEN){

	    BWCS_DBG(BWCS_LOG_TAG "receive event: BT_EVENT_BT_OPEN");
	    event_to_bwcs(BT_EVENT_BT_OPEN);
	}
	if (event & BT_EVENT_BT_CLOSE){

	    BWCS_DBG(BWCS_LOG_TAG"receive event: BT_EVENT_BT_CLOSE");
	    event_to_bwcs(BT_EVENT_BT_CLOSE);
	    bt_running = 0;// when bt is off, means this function should be closed
	}
	if (event & BT_ACTION_SCATTER_INQUIRY){

	    BWCS_DBG(BWCS_LOG_TAG "receive action: BT_ACTION_SCATTER_INQUIRY");
	    bt_action(BT_ACTION_SCATTER_INQUIRY);//bt_action function should be modified
	}
	if (event & BT_ACTION_WIFI_CHL){

	    BWCS_DBG(BWCS_LOG_TAG "receive action: BT_ACTION_WIFI_CHL, channel is:%d", bwcs_action.para[0]);
	    bt_action(BT_ACTION_WIFI_CHL);
	}
	if (event & BT_ACTION_POWER_CTRL){

	    BWCS_DBG(BWCS_LOG_TAG "receive action: BT_ACTION_POWER_CTRL");
	    bt_action(BT_ACTION_POWER_CTRL);// if is wifi chl, how to carry this parameter
	}
    }
    BWCS_DBG(BWCS_LOG_TAG "bwcs client exit");
    pthread_exit(NULL);

    return 0;
}
