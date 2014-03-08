#ifndef IWEVENT_H
#define IWEVENT_H

#ifndef IFLA_WIRELESS
#define IFLA_WIRELESS	(IFLA_MASTER + 1)
#endif /* IFLA_WIRELESS */

/****************************** TYPES ******************************/

/*
 * Static information about wireless interface.
 * We cache this info for performance reason.
 */
typedef struct wireless_iface
{
  /* Linked list */
  struct wireless_iface *	next;

  /* Interface identification */
  int		ifindex;		/* Interface index == black magic */

  /* Interface data */
  char			ifname[IFNAMSIZ + 1];	/* Interface name */
  struct iw_range	range;			/* Wireless static data */
  int			has_range;
} wireless_iface;


//JCB
typedef struct __RT_BWCS_EVENT {
	char			name[32];
	unsigned char	Channel;
	unsigned char	Freq;
} RT_BWCS_EVENT;

struct rtnl_handle
{
	int			fd;
	struct sockaddr_nl	local;
	struct sockaddr_nl	peer;
	__u32			seq;
	__u32			dump;
};

#endif
