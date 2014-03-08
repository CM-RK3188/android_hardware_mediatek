#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include "iwlib.h"		/* Header */

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include <getopt.h>
#include <time.h>
#include <sys/time.h>
#include "bwcs.h"
#include "bwcs_sock.h"
#include "iwevent.h"
#include "iwlib.h"

/* bt status thread control block */

typedef struct
{
    pthread_t		bt_status_thread;
    pthread_mutex_t	mutex;
    pthread_cond_t	cond;
    int			bt_status;//todo: should it be expanded
    int			wifi_status;//todo: should it be expanded
} bwcs_status_cb_t;

/******************************************************************************
**  Static Variables
******************************************************************************/
extern volatile UCHAR	sock_running;
extern struct rtnl_handle	rthandle;
extern bwcs_sock_cb_t	bwcs_sock_cb;
static bwcs_status_cb_t	bwcs_status_cb;
static volatile USHORT	bt_ready_events = 0;
static volatile USHORT	wifi_ready_events = 0;

extern char wifi_channel_info[WIFI_EVENT_PARAM_MAX];



/******************************************************************************
**  Functions
******************************************************************************/

static void * bwcs_status_thread(void *arg);

void bt_event_notify(USHORT event)
{
	printf("bt_event_notify:%d\n", event);
    pthread_mutex_lock(&bwcs_status_cb.mutex);
    bt_ready_events |= event;
    pthread_cond_signal(&bwcs_status_cb.cond);
    pthread_mutex_unlock(&bwcs_status_cb.mutex);
}

void wifi_event_notify(USHORT event)
{
	printf("wifi_event_notify:%d\n", event);
	pthread_mutex_lock(&bwcs_status_cb.mutex);
    wifi_ready_events |= event;
    pthread_cond_signal(&bwcs_status_cb.cond);
    pthread_mutex_unlock(&bwcs_status_cb.mutex);
}


void cleanup()
{
    destroy_sock_thread();
    //and do some cleaning up things
}

int main()
{
    printf("bwcs Server start\n");
    int bt_event,wifi_event;
	
    int i;
    char para[8];
	/*
    memset(&bwcs_status_cb, 0, sizeof(bwcs_status_cb_t));
    pthread_attr_t thread_attr;
    pthread_mutex_init(&bwcs_status_cb.mutex, NULL);
    pthread_cond_init(&bwcs_status_cb.cond, NULL);
    pthread_attr_init(&thread_attr);
	*/
	//bwcs should store bt&wifi current status
	
	if(mkdir("/data/bwcs", 0755)<0) {
		printf("mkdir error:%d, %s\n",errno, strerror(errno));
	while(1);
	}
	rtnl_setup();
	
    bwcs_setup_sock_thread();//todo: this should all in a init function ; 
	
	
    while(1){
	/*
	for(i=0;i<100;i++){
	sleep(1);
	event_to_bt(i);
	}
		*/
		
		pthread_mutex_lock(&bwcs_status_cb.mutex);
		
        while (wifi_ready_events == 0 && bt_ready_events == 0)
        {
			pthread_cond_wait(&bwcs_status_cb.cond, &bwcs_status_cb.mutex);
        }
        wifi_event = wifi_ready_events;
        bt_event = bt_ready_events;
        bt_ready_events = 0;
        wifi_ready_events = 0;
        pthread_mutex_unlock(&bwcs_status_cb.mutex);
		
		
		if (bt_event & BT_EVENT_ON){
			bwcs_status_cb.bt_status = BT_EVENT_ON;
			printf("receive event: BT_EVENT_ON\n");
			if(bwcs_status_cb.wifi_status == WIFI_EVENT_CONN_NEW)
			{
				para[0]=1;
				action_to_bt(BT_ACTION_SCATTER_INQUIRY, para);
				usleep(500);
				action_to_bt(BT_ACTION_WIFI_CHANNEL, wifi_channel_info);
			}

			//event_to_bt(BT_EVENT_ON);
		}
		if (bt_event & BT_EVENT_OFF){
			bwcs_status_cb.bt_status = BT_EVENT_OFF;
			printf("receive event: BT_EVENT_OFF\n");
			//event_to_bt(BT_EVENT_OFF);
		}
		if (wifi_event & WIFI_EVENT_CONN_NEW){
			bwcs_status_cb.wifi_status = WIFI_EVENT_CONN_NEW;
			printf("receive event: WIFI_EVENT_CONN_NEW\n");
			if(bwcs_status_cb.bt_status == BT_EVENT_ON)
			{
				para[0]=1;
				action_to_bt(BT_ACTION_SCATTER_INQUIRY, para);
				usleep(500);
				action_to_bt(BT_ACTION_WIFI_CHANNEL, wifi_channel_info);
				
			}

		}
		if (wifi_event & WIFI_EVENT_CONN_DEL){
			bwcs_status_cb.wifi_status = WIFI_EVENT_CONN_DEL;
			printf("receive event: WIFI_EVENT_CONN_DEL\n");
			if(bwcs_status_cb.bt_status == BT_EVENT_ON)
			{
				para[0]=0;
				action_to_bt(BT_ACTION_SCATTER_INQUIRY, para);
			}
		
		}
		
    }
	
    cleanup();
    /* Cleanup - only if you are pedantic */
	rtnl_cleanup();
    return 0;
}

void * bwcs_sock_thread(void *arg)
{
    sock_running = 1;
    int n =0, ret = -1;
	//   rx_length;
    fd_set input;
    int		last_fd;	
    BWCS_BT bwcs_recv;  //will change to BWCS_BT* and use bt_hc_cbacks->alloc to alloc memory for it
    struct sockaddr_un any;
    int any_len;
	
    printf("bt_sock_thread start\n");
    
	fprintf(stderr, "Waiting for Wireless Events from interfaces...\n");
	/* Do what we have to do */
	
    while(sock_running){
	/*        
	if (bt_hc_cbacks)
	{
	p_buf = (HC_BT_HDR *) bt_hc_cbacks->alloc( \
	BTHC_USERIAL_READ_MEM_SIZE);
	}
		*/	//in my libbt-vendor.so should use this function to alloc memory 
		
		
        /* Initialize the input fd set */
		FD_ZERO(&input);
		FD_SET(bwcs_sock_cb.com_fd, &input);
		FD_SET(rthandle.fd, &input);
		last_fd = bwcs_sock_cb.com_fd > rthandle.fd ? bwcs_sock_cb.com_fd : rthandle.fd;
		
		n=0;
		memset(&bwcs_recv, 0, sizeof(BWCS_BT));
        /* Do select */
        n = select(last_fd+1, &input, NULL, NULL, NULL);
		
        if (n > 0)
        {
            /* We might have input */
            if (FD_ISSET(bwcs_sock_cb.com_fd, &input))
            {

				any_len = sizeof(any);
				ret = recvfrom(bwcs_sock_cb.com_fd, &bwcs_recv, sizeof(BWCS_BT), 0, (struct sockaddr*)&any, &any_len);
                if (0 == ret)
                    printf( "read() returned 0!\n" );
				printf("read ret = %d\n", ret);
				
				print_payload( &bwcs_recv );
				parse_bt_event(&bwcs_recv);
				//		if (bwcs_recv.event == 0x0050)  // receive event == 0x0004, means to exit; will be replaced by signal(how)....
				//		    sock_running = 0;
            }
            if(FD_ISSET(rthandle.fd, &input))
			{
				
				handle_netlink_events(&rthandle);
			}
			
        }
        else if (n < 0) {
            printf( "select() Failed, %d, %s\n", errno, strerror(errno));
		return 0;
	}
        else if (n == 0)
            printf( "Got a select() TIMEOUT\n");
    }
	
    printf("bt_sock_thread exit\n");
    pthread_exit(NULL);
    return NULL;
	
}
