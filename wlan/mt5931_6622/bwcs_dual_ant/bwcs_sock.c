#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <pthread.h>
#include "bwcs.h"
#include "bwcs_sock.h"

extern void * bwcs_sock_thread(void *arg);
bwcs_sock_cb_t	bwcs_sock_cb;
volatile UCHAR	sock_running = 0;// may not need, because of always running

/* print function */
void print_payload(BWCS_BT* bb)
{
    printf("from bt event    : 0X%04X\n", bb->event);
    printf("from bt action   : 0X%04X\n", bb->action);
    printf("from bt parameter: %02X %02X %02X %02X %02X %02X %02X %02X\n", bb->para[0], bb->para[1], bb->para[2], bb->para[3], bb->para[4], bb->para[5], bb->para[6], bb->para[7]);
}


void action_to_bt( USHORT action, char * para)
{
    printf("action_to_bt: 0X%04X\n", action);
    //to be done
    
    BWCS_BT bwcs_send;
	struct sockaddr_un local;
	int local_len;
	int sock_fd2;
    sock_fd2=socket(PF_UNIX, SOCK_DGRAM, 0);	
	memset(&local, 0, sizeof(local));
    local.sun_family = AF_LOCAL;
    strcpy(local.sun_path, UNIX_DOMAIN_BT);
	
    bwcs_send.event = 0;
    bwcs_send.action = action;
    memcpy(bwcs_send.para, para, 8);
    local_len = sizeof(local);
    sendto(sock_fd2, &bwcs_send, sizeof(BWCS_BT), 0, (struct sockaddr*)&local, local_len);
	usleep(500);
	close(sock_fd2);
    //todo: add some judgement on write return value 
}

/* think about readn & writen in p74*/
int bwcs_sock_setup( )
{
    socklen_t clt_addr_len;
	
    int ret, len;
	
    struct sockaddr_un clt_addr;
	
    struct sockaddr_un srv_addr;
	
    //bwcs_sock_cb.listen_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    //if(bwcs_sock_cb.listen_fd<0){
    //    perror("cannot create listening socket");
    //    return 1;
    //}
	
	bwcs_sock_cb.com_fd = socket(PF_UNIX, SOCK_DGRAM, 0);
	if(bwcs_sock_cb.com_fd < 0){
        perror("cannot create listening socket");
        return 1;
    }
    printf("create listen socket success\n");
	
    srv_addr.sun_family=AF_UNIX;
    strncpy(srv_addr.sun_path, UNIX_DOMAIN_BWCS, sizeof(srv_addr.sun_path)-1);	
	unlink(UNIX_DOMAIN_BWCS);
    ret=bind(bwcs_sock_cb.com_fd, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
    if(ret==-1){
        perror("cannot bind server socket");
        close(bwcs_sock_cb.com_fd);
        unlink(UNIX_DOMAIN_BWCS);
        return 1;
    }
    printf("server addr bind to listen socket success\n");

	/*
    ret=listen(bwcs_sock_cb.listen_fd,1);
    if(ret==-1){
        perror("cannot listen the client connect request");
        close(bwcs_sock_cb.listen_fd);
        unlink(UNIX_DOMAIN);
        return 1;
    }
    printf("server is listening\n");
	
    len=sizeof(clt_addr);
    bwcs_sock_cb.com_fd = accept(bwcs_sock_cb.listen_fd, (struct sockaddr*)&clt_addr, &len);
    if(bwcs_sock_cb.com_fd<0){
		perror("cannot accept client connect request");
        close(bwcs_sock_cb.listen_fd);
        unlink(UNIX_DOMAIN);
        return 1;
    }
    printf("unix socket server get connected\n");
    */
    
    return 0;
}

/* setup socket connection & setup sock_thread to communicate with bwcs */
int bwcs_setup_sock_thread()
{
    pthread_attr_t thread_attr;
	
    bwcs_sock_setup();
	
    printf("bwcs_sock_cb.com_fd = %d\n", bwcs_sock_cb.com_fd);
    pthread_attr_init(&thread_attr);
    if (pthread_create(&(bwcs_sock_cb.sock_thread), &thread_attr, \
		bwcs_sock_thread, NULL) != 0 )
    {
        printf("bt sock_thread create failed!\n");
        return FALSE;
    }// may this usage cause memory leakage, should have pthread_join/exit
	
    return 0;
}



/* used to parse event from bwcs */
void parse_bt_event(BWCS_BT* bb)
{
    USHORT event;
    event = bb->event;
    switch (event){
	case BT_EVENT_ON:
		bt_event_notify(BT_EVENT_ON);
		break;
	case BT_EVENT_OFF:
		bt_event_notify(BT_EVENT_OFF);
		break;
	default:
		break;
    }
}

void destroy_sock_thread()
{
    close(bwcs_sock_cb.com_fd);
    //close(bwcs_sock_cb.listen_fd);
    unlink(UNIX_DOMAIN_BWCS);
}
