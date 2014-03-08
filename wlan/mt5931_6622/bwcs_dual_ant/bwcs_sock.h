#ifndef BT_SOCK_H
#define BT_SOCK_H

/* bt sock thread control block */
typedef struct {
    pthread_t	sock_thread;
    //int		listen_fd;
    int		com_fd;
} bwcs_sock_cb_t;

int bwcs_sock_setup(void);
int bwcs_setup_sock_conn(void);

//void event_to_bwcs( USHORT event );
void print_payload(BWCS_BT* bb);
void parse_bt_event(BWCS_BT* bb);

#endif
