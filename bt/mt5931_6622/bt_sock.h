#ifndef BT_SOCK_H
#define BT_SOCK_H

/* bt_sock headers */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "bwcs.h"

/* bt sock thread control block */
typedef struct {
    pthread_t       sock_thread;
    struct sockaddr_un bwcs_addr;
    struct sockaddr_un bt_addr;
    int             fd;
} bt_sock_cb_t;

int  sock_setup(void);
int  setup_sock_conn(void);
void destroy_sock_conn(void);

void event_to_bwcs( USHORT event );
void print_payload( BWCS_BT* bb );
void parse_bwcs_action(BWCS_BT bb);

#endif
