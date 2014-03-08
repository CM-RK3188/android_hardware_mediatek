#include "bwcs.h"
#include "bt_sock.h"

static void * bt_sock_thread(void *arg);
static bt_sock_cb_t	bt_sock_cb;
static volatile UCHAR	sock_running = 0;

enum {
    BT_SOCK_THREAD_EXIT
};


/*****************************************************************************
**   Socket signal functions to wake up bt_sock_thread for termination
**
**   creating an unnamed pair of connected sockets
**      - signal_fds[0]: join fd_set in select call of bt_sock_thread
**      - signal_fds[1]: trigger from bt_status_thread in destroy_sock_conn
*****************************************************************************/
static int signal_fds[2]={0,1};
static inline int create_signal_fds(fd_set* set)
{
    if(signal_fds[0]==0 && socketpair(AF_UNIX, SOCK_STREAM, 0, signal_fds)<0)
    {
        ALOGE("create_signal_sockets:socketpair failed, errno: %d", errno);
        return -1;
    }
    FD_SET(signal_fds[0], set);
    return signal_fds[0];
}
static inline int send_wakeup_signal(char sig_cmd)
{
    return send(signal_fds[1], &sig_cmd, sizeof(sig_cmd), 0);
}
static inline char reset_signal()
{
    char sig_recv = -1;
    recv(signal_fds[0], &sig_recv, sizeof(sig_recv), MSG_WAITALL);
    return sig_recv;
}
static inline int is_signaled(fd_set* set)
{
    return FD_ISSET(signal_fds[0], set);
}


/* print function */
void print_payload(BWCS_BT* bb)
{
    BWCS_DBG(BWCS_LOG_TAG "from bwcs event    : 0X%04X", bb->event);
    BWCS_DBG(BWCS_LOG_TAG "from bwcs action   : 0X%04X", bb->action);
    BWCS_DBG(BWCS_LOG_TAG "from bwcs parameter: %02X %02X %02X %02X %02X %02X %02X %02X", bb->para[0], bb->para[1], bb->para[2], bb->para[3], bb->para[4], bb->para[5], bb->para[6], bb->para[7]);
}

void event_to_bwcs( USHORT event)
{
    BWCS_DBG(BWCS_LOG_TAG "event_to_bwcs 1: 0X%04X, %s", event,bt_sock_cb.bwcs_addr.sun_path);
    //to be done
    
    BWCS_BT bt_send;
    bt_send.event = event;
    int temp_fd;
   
    memset(&(bt_sock_cb.bwcs_addr), 0, sizeof(struct sockaddr_un));
    bt_sock_cb.bwcs_addr.sun_family = AF_UNIX;
    strcpy(bt_sock_cb.bwcs_addr.sun_path, BWCS_DOMAIN);

	
    temp_fd=socket(PF_UNIX, SOCK_DGRAM, 0);

    sendto(temp_fd, &bt_send, sizeof(BWCS_BT), 0, (struct sockaddr*)(&(bt_sock_cb.bwcs_addr)), sizeof(struct sockaddr_un));
    BWCS_DBG(BWCS_LOG_TAG "usleep(500)\n");
    usleep(500);
    close(temp_fd);

//    write(bt_sock_cb.fd, &bt_send, sizeof(BWCS_BT));
    //todo: add some judgement on write return value 
}

/* think about readn & writen in p74*/
int sock_setup( )
{
    int ret, sock_fd;

    sock_fd=socket(PF_UNIX, SOCK_DGRAM, 0);
    if(sock_fd<0){
	BWCS_ERR(BWCS_LOG_TAG "cannot create communication socket, errno:%d, err string:%s", errno, strerror(errno));
	return 1;
   } 
   bt_sock_cb.fd = sock_fd;
   BWCS_DBG(BWCS_LOG_TAG "bt_sock_cb.fd = %d\n", bt_sock_cb.fd);

   bt_sock_cb.bt_addr.sun_family = AF_UNIX;
   strcpy(bt_sock_cb.bt_addr.sun_path, MT6622_DOMAIN);

    unlink(MT6622_DOMAIN);
   ret = bind(bt_sock_cb.fd, (struct sockaddr*)(&(bt_sock_cb.bt_addr)), sizeof(struct sockaddr_un));
   if(ret==-1){
	BWCS_ERR(BWCS_LOG_TAG "cannot bind to bt_sock_cb.fd, errno: %d strerr: %s", errno, strerror(errno));
	close(bt_sock_cb.fd);
	unlink(MT6622_DOMAIN);
	return 1;
   }
   BWCS_DBG(BWCS_LOG_TAG "bind socket success\n");

/*
    ret=connect(sock_fd, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
    if(ret==-1){
        BWCS_ERR(BWCS_LOG_TAG "cannot connect to the server, errno:%d, err string:%s", errno, strerror(errno));
        close(sock_fd);
        return 1;
    }
    BWCS_DBG(BWCS_LOG_TAG "get coonnect to server");
*/
    return ret;
}

/* setup socket connection & setup sock_thread to communicate with bwcs */
int setup_sock_conn()
{
    pthread_attr_t thread_attr;

//    bt_sock_cb.fd = sock_setup();
    sock_setup();

//    BWCS_DBG(BWCS_LOG_TAG "bt_sock_cb.fd = %d", bt_sock_cb.fd);
    pthread_attr_init(&thread_attr);
    if (pthread_create(&(bt_sock_cb.sock_thread), &thread_attr, \
                       bt_sock_thread, NULL) != 0 )
    {
        BWCS_ERR(BWCS_LOG_TAG "bt sock_thread create failed!, errno:%d, err string:%s", errno, strerror(errno));
        return FALSE;
    }// may this usage cause memory leakage

//temp no use
/*
    if(pthread_getschedparam(userial_cb.read_thread, &policy, &param)==0)
    {
        policy = BTHC_LINUX_BASE_POLICY;
#if (BTHC_LINUX_BASE_POLICY!=SCHED_NORMAL)
        param.sched_priority = BTHC_USERIAL_READ_THREAD_PRIORITY;
#endif
        result = pthread_setschedparam(userial_cb.read_thread, policy, &param);
        if (result != 0)
        {
            ALOGW("userial_open: pthread_setschedparam failed (%s)", \
                  strerror(result));
        }
    }
*/
    return 0;
}

void destroy_sock_conn()
{
    BWCS_DBG(BWCS_LOG_TAG "destroy_sock_conn, sock_running: 0X%02X", sock_running);
    if(sock_running) {
//	send_wakeup_signal(BT_SOCK_THREAD_EXIT);

        sock_running = 0;
//    usleep(1000);
        close(bt_sock_cb.fd);
    }
    unlink(MT6622_DOMAIN);
}

static void * bt_sock_thread(void *arg)
{
    sock_running = 1;
    int n =0, ret = -1;
    char reason = 0;

    fd_set input;
    BWCS_BT bt_recv;  //will change to BWCS_BT* and use bt_hc_cbacks->alloc to alloc memory for it

    struct sockaddr_un any;
    int any_len;
    any_len = sizeof(any);

    BWCS_DBG(BWCS_LOG_TAG "bt_sock_thread start");
    while(sock_running){
/*        
	if (bt_hc_e)
        {
            p_buf = (HC_BT_HDR *) bt_hc_cbacks->alloc( \
                                                BTHC_USERIAL_READ_MEM_SIZE);
        }
*/	//in my libbt-vendor.so should use this function to alloc memory 

        /* Initialize the input fd set */
//        int fd_max = create_signal_fds(&input);
//        fd_max = fd_max > bt_sock_cb.fd ? fd_max : bt_sock_cb.fd;


        FD_ZERO(&input);
	FD_SET(bt_sock_cb.fd, &input);

        /* Do select */
        n = select(bt_sock_cb.fd+1, &input, NULL, NULL, NULL);
/*	if(is_signaled(&input))
        {
            reason = reset_signal();
            if (reason == BT_SOCK_THREAD_EXIT)
            {
                BWCS_DBG(BWCS_LOG_TAG "RX termination");
                return NULL;
            }
        }
*/
        if (n > 0){
            /* We might have input */
            if (FD_ISSET(bt_sock_cb.fd, &input)) {

		ret = recvfrom(bt_sock_cb.fd, &bt_recv, sizeof(BWCS_BT), 0, (struct sockaddr*)&any, &any_len);

//                ret = read(bt_sock_cb.fd, &bt_recv, sizeof(BWCS_BT));
                if (0 == ret)
                    BWCS_DBG(BWCS_LOG_TAG "recvfrom() returned 0!" );
		BWCS_DBG(BWCS_LOG_TAG "read ret = %d", ret);
		parse_bwcs_action(bt_recv);
	    }
        }
        else if (n < 0)
            BWCS_DBG(BWCS_LOG_TAG "select() Failed:errno:%d, errstr:%s\n", errno, strerror(errno));
        else if (n == 0)
            BWCS_DBG(BWCS_LOG_TAG "Got a select() TIMEOUT");
    }

    BWCS_DBG(BWCS_LOG_TAG "bt_sock_thread exit");
    pthread_exit(NULL);
    return NULL;//
}

/* used to parse event from bwcs */
void parse_bwcs_action(BWCS_BT bb)
{
    USHORT action;

    action = bb.action;
    print_payload(&bb);
    switch (action){

	case BT_ACTION_SCATTER_INQUIRY:
	    bwcs_action_notify(BT_ACTION_SCATTER_INQUIRY, bb);
	    break;

	case BT_ACTION_WIFI_CHL:
	    bwcs_action_notify(BT_ACTION_WIFI_CHL, bb);
	    break;

	case BT_ACTION_POWER_CTRL:
	    bwcs_action_notify(BT_ACTION_POWER_CTRL, bb);
	    break;

	default:
	    BWCS_DBG(BWCS_LOG_TAG "no such action: 0X%04X", action);
	    break;
    }
}
