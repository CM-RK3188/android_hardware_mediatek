/*
*	Wireless Tools
*
*		Jean II - HPL 99->04
*
* Main code for "iwevent". This listent for wireless events on rtnetlink.
* You need to link this code against "iwcommon.c" and "-lm".
*
* Part of this code is from Alexey Kuznetsov, part is from Casey Carter,
* I've just put the pieces together...
* By the way, if you know a way to remove the root restrictions, tell me
* about it...
*
* This file is released under the GPL license.
*     Copyright (c) 1997-2004 Jean Tourrilhes <jt@hpl.hp.com>
*/

/***************************** INCLUDES *****************************/

#include "iwlib.h"		/* Header */

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include <getopt.h>
#include <time.h>
#include <sys/time.h>
#include "bwcs.h"
#include "iwevent.h"
/* Ugly backward compatibility :-( */


/**************************** VARIABLES ****************************/

/* Cache of wireless interfaces */
struct wireless_iface *	interface_cache = NULL;


struct rtnl_handle	rthandle;

char wifi_channel_info[WIFI_EVENT_PARAM_MAX];


/* used to parse event from bwcs */
void parse_wifi_event(BWCS_WIFI* bw)
{
    USHORT event;
    event = bw->event;
    switch (event){
	case WIFI_EVENT_WIFI_OPEN:
		wifi_event_notify(WIFI_EVENT_WIFI_OPEN);
		break;
	case WIFI_EVENT_WIFI_CLOSE:
		wifi_event_notify(WIFI_EVENT_WIFI_CLOSE);
		break;
	case WIFI_EVENT_CONN_NEW:
		wifi_event_notify(WIFI_EVENT_CONN_NEW);
		break;
	case WIFI_EVENT_CONN_DEL:
		wifi_event_notify(WIFI_EVENT_CONN_DEL);
		break;
	default:
		break;
    }
}

static inline void rtnl_close(struct rtnl_handle *rth)
{
	close(rth->fd);
}

static inline int rtnl_open(struct rtnl_handle *rth, unsigned subscriptions)
{
	int addr_len;
	
	memset(rth, 0, sizeof(rth));
	
	rth->fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if (rth->fd < 0) {
		perror("Cannot open netlink socket");
		return -1;
	}
	
	memset(&rth->local, 0, sizeof(rth->local));
	rth->local.nl_family = AF_NETLINK;
	rth->local.nl_groups = subscriptions;
	
	if (bind(rth->fd, (struct sockaddr*)&rth->local, sizeof(rth->local)) < 0) {
		perror("Cannot bind netlink socket");
		return -1;
	}
	addr_len = sizeof(rth->local);
	if (getsockname(rth->fd, (struct sockaddr*)&rth->local,
		(socklen_t *) &addr_len) < 0) {
		perror("Cannot getsockname");
		return -1;
	}
	if (addr_len != sizeof(rth->local)) {
		fprintf(stderr, "Wrong address length %d\n", addr_len);
		return -1;
	}
	if (rth->local.nl_family != AF_NETLINK) {
		fprintf(stderr, "Wrong address family %d\n", rth->local.nl_family);
		return -1;
	}
	rth->seq = time(NULL);
	return 0;
}

/******************* WIRELESS INTERFACE DATABASE *******************/
/*
* We keep a few information about each wireless interface on the
* system. This avoid to query this info at each event, therefore
* reducing overhead.
*
* Each interface is indexed by the 'ifindex'. As opposed to interface
* names, 'ifindex' are never reused (even if you reactivate the same
* hardware), so the data we cache will never apply to the wrong
* interface.
* Because of that, we are pretty lazy when it come to purging the
* cache...
*/

/*------------------------------------------------------------------*/
/*
* Get name of interface based on interface index...
*/
static inline int
index2name(int		skfd,
		   int		ifindex,
		   char *	name)
{
	struct ifreq	irq;
	int		ret = 0;
	
	memset(name, 0, IFNAMSIZ + 1);
	
	/* Get interface name */
	irq.ifr_ifindex = ifindex;
	if(ioctl(skfd, SIOCGIFNAME, &irq) < 0)
		ret = -1;
	else
		strncpy(name, irq.ifr_name, IFNAMSIZ);
	
	return(ret);
}

/*------------------------------------------------------------------*/
/*
* Get interface data from cache or live interface
*/
static struct wireless_iface *
iw_get_interface_data(int	ifindex)
{
	struct wireless_iface *	curr;
	int				skfd = -1;	/* ioctl socket */
	
	/* Search for it in the database */
	curr = interface_cache;
	while(curr != NULL)
    {
		/* Match ? */
		if(curr->ifindex == ifindex)
		{
			//printf("Cache : found %d-%s\n", curr->ifindex, curr->ifname);
			
			/* Return */
			return(curr);
		}
		/* Next entry */
		curr = curr->next;
    }
	
	/* Create a channel to the NET kernel. Doesn't happen too often, so
	* socket creation overhead is minimal... */
	if((skfd = iw_sockets_open()) < 0)
    {
		perror("iw_sockets_open");
		return(NULL);
    }
	
	/* Create new entry, zero, init */
	curr = calloc(1, sizeof(struct wireless_iface));
	if(!curr)
    {
		fprintf(stderr, "Malloc failed\n");
		return(NULL);
    }
	curr->ifindex = ifindex;
	
	/* Extract static data */
	if(index2name(skfd, ifindex, curr->ifname) < 0)
    {
		perror("index2name");
		free(curr);
		return(NULL);
    }
	curr->has_range = (iw_get_range_info(skfd, curr->ifname, &curr->range) >= 0);
	//printf("Cache : create %d-%s\n", curr->ifindex, curr->ifname);
	
	/* Done */
	iw_sockets_close(skfd);
	
	/* Link it */
	curr->next = interface_cache;
	interface_cache = curr;
	
	return(curr);
}

/*------------------------------------------------------------------*/
/*
* Remove interface data from cache (if it exist)
*/
static void
iw_del_interface_data(int	ifindex)
{
	struct wireless_iface *	curr;
	struct wireless_iface *	prev = NULL;
	struct wireless_iface *	next;
	
	/* Go through the list, find the interface, kills it */
	curr = interface_cache;
	while(curr)
    {
		next = curr->next;
		
		/* Got a match ? */
		if(curr->ifindex == ifindex)
		{
			/* Unlink. Root ? */
			if(!prev)
				interface_cache = next;
			else
				prev->next = next;
			//printf("Cache : purge %d-%s\n", curr->ifindex, curr->ifname);
			
			/* Destroy */
			free(curr);
		}
		else
		{
			/* Keep as previous */
			prev = curr;
		}
		
		/* Next entry */
		curr = next;
    }
}

/********************* WIRELESS EVENT DECODING *********************/
/*
* Parse the Wireless Event and print it out
*/

/*------------------------------------------------------------------*/
/*
* Dump a buffer as a serie of hex
* Maybe should go in iwlib...
* Maybe we should have better formatting like iw_print_key...
*/
static char *
iw_hexdump(char *		buf,
		   size_t		buflen,
		   const unsigned char *data,
		   size_t		datalen)
{
	size_t	i;
	char *	pos = buf;
	
	for(i = 0; i < datalen; i++)
		pos += snprintf(pos, buf + buflen - pos, "%02X", data[i]);
	return buf;
}

/*------------------------------------------------------------------*/
/*
* Print one element from the scanning results
*/
static inline int
print_event_token(struct iw_event *	event,		/* Extracted token */
				  struct iw_range *	iw_range,	/* Range info */
				  int			has_range)
{
	char		buffer[128];	/* Temporary buffer */
	char		buffer2[30];	/* Temporary buffer */
	char *	prefix = (IW_IS_GET(event->cmd) ? "New" : "Set");
	
	/* Now, let's decode the event */
	switch(event->cmd)
    {
		/* ----- set events ----- */
    case IWEVCUSTOM:
		{
			char custom[IW_CUSTOM_MAX+1];
			memset(custom, '\0', sizeof(custom));
			
			printf("[JCB]Custom driver event start \n");
			
#if 1  //temp
			if((event->u.data.pointer) && (event->u.data.length))
				memcpy(custom, event->u.data.pointer, event->u.data.length);
			
			//printf("Custom driver event:%s \n", custom);
			printf("Custom driver event:%s , flags=%d\n", custom,  event->u.data.flags );
			
			
			//JCB add for test
			RT_BWCS_EVENT *pBwcsEvent;
			if((event->u.data.pointer) && (event->u.data.length))
			{
				pBwcsEvent = (RT_BWCS_EVENT *)event->u.data.pointer;
				
				if(!strcmp(pBwcsEvent->name, "BWCS2"))
				{
					printf("[JCB]pBwcsEvent->name:%s , pBwcsEvent->Channel=%d ,pBwcsEvent->Freq=%d,  flags=%d\n", 
						pBwcsEvent->name,  pBwcsEvent->Channel, pBwcsEvent->Freq ,event->u.data.flags);
				}
				else
				{
					printf("Custom driver event: not for BWCS\n");
				}
			}
#endif
			
			
			BWCS_WIFI *pBwcsEvent2;
			int i;
			if((event->u.data.pointer) && (event->u.data.length))
			{
				pBwcsEvent2 = (BWCS_WIFI *)event->u.data.pointer;
				
				if(!strcmp(pBwcsEvent2->name, BWCS_NAME))
				{
					printf("[JCB]pBwcsEvent2->name:%s , event=%d ,  flags=%d\n", 
						pBwcsEvent2->name,  pBwcsEvent2->event , event->u.data.flags);
					
					if( pBwcsEvent2->event == WIFI_EVENT_CONN_NEW)
						{
						memcpy(wifi_channel_info,pBwcsEvent2->para,WIFI_EVENT_PARAM_MAX);
						}
					
					parse_wifi_event(pBwcsEvent2);

					
					for ( i=0; i<WIFI_EVENT_PARAM_MAX; i++)
					{
						printf("[JCB] pBwcsEvent2->para[%d]=%d \n", i, pBwcsEvent2->para[i]);
	
					}
				}
				else
				{
					printf("Custom driver event: not for BWCS2\n");
				}	
			}
		}
		break;
		
    default:
		printf("(Unknown Wireless event 0x%04X)\n", event->cmd);
    }	/* switch(event->cmd) */
	
	return(0);
}

/*------------------------------------------------------------------*/
/*
* Print out all Wireless Events part of the RTNetlink message
* Most often, there will be only one event per message, but
* just make sure we read everything...
*/
static inline int
print_event_stream(int		ifindex,
				   char *	data,
				   int		len)
{
	struct iw_event	iwe;
	struct stream_descr	stream;
	int			i = 0;
	int			ret;
	char			buffer[64];
	struct timeval	recv_time;
	struct timezone	tz;
	struct wireless_iface *	wireless_data;
	
	/* Get data from cache */
	wireless_data = iw_get_interface_data(ifindex);
	if(wireless_data == NULL)
		return(-1);
	
	/* Print received time in readable form */
	gettimeofday(&recv_time, &tz);
	iw_print_timeval(buffer, sizeof(buffer), &recv_time, &tz);
	
	iw_init_event_stream(&stream, data, len);
	do
    {
		/* Extract an event and print it */
		ret = iw_extract_event_stream(&stream, &iwe,
			wireless_data->range.we_version_compiled);
		if(ret != 0)
		{
			if(i++ == 0)
				printf("%s   %-8.16s ", buffer, wireless_data->ifname);
			else
				printf("                           ");
			if(ret > 0)
				print_event_token(&iwe,
				&wireless_data->range, wireless_data->has_range);
			else
				printf("(Invalid event)\n");
			/* Push data out *now*, in case we are redirected to a pipe */
			fflush(stdout);
		}
    }
	while(ret > 0);
	
	return(0);
}

/*********************** RTNETLINK EVENT DUMP***********************/
/*
* Dump the events we receive from rtnetlink
* This code is mostly from Casey
*/

/*------------------------------------------------------------------*/
/*
* Respond to a single RTM_NEWLINK event from the rtnetlink socket.
*/
static int
LinkCatcher(struct nlmsghdr *nlh)
{
	struct ifinfomsg* ifi;
	
#if 1
	fprintf(stderr, "%s: nlmsg_type %#x.\n", __PRETTY_FUNCTION__, nlh->nlmsg_type);
	//fprintf(stderr, "nlmsg_type = %d.\n", nlh->nlmsg_type);
#endif
	
	ifi = NLMSG_DATA(nlh);
	
	/* Code is ugly, but sort of works - Jean II */
	
	/* If interface is getting destoyed */
	if(nlh->nlmsg_type == RTM_DELLINK)
    {
		/* Remove from cache (if in cache) */
		iw_del_interface_data(ifi->ifi_index);
		return 0;
    }
	
	//fprintf(stderr, "%s: interface[ifi_family=0x%x][ifi_idex=0x%x][ifi_type=0x%x][ifi_flags=0x%x][ifi_change=0x%x] continues... %#x.\n",
	//			  __PRETTY_FUNCTION__, ifi->ifi_family, ifi->ifi_index,ifi->ifi_type,ifi->ifi_flags,ifi->ifi_change);
	
	/* Only keep add/change events */
	if(nlh->nlmsg_type != RTM_NEWLINK)
		return 0;
	
	/* Check for attributes */
	if (nlh->nlmsg_len > NLMSG_ALIGN(sizeof(struct ifinfomsg)))
    {
		int attrlen = nlh->nlmsg_len - NLMSG_ALIGN(sizeof(struct ifinfomsg));
		struct rtattr *attr = (void *) ((char *) ifi +
			NLMSG_ALIGN(sizeof(struct ifinfomsg)));
		
		while (RTA_OK(attr, attrlen))
		{
			/* Check if the Wireless kind */
			if(attr->rta_type == IFLA_WIRELESS)
			{
				/* Go to display it */
				print_event_stream(ifi->ifi_index,
					(char *) attr + RTA_ALIGN(sizeof(struct rtattr)),
					attr->rta_len - RTA_ALIGN(sizeof(struct rtattr)));
			}
			attr = RTA_NEXT(attr, attrlen);
		}
    }
	
	return 0;
}

/* ---------------------------------------------------------------- */
/*
* We must watch the rtnelink socket for events.
* This routine handles those events (i.e., call this when rthandle.fd
* is ready to read).
*/
inline void
handle_netlink_events(struct rtnl_handle *	rth)
{
	while(1)
    {
		struct sockaddr_nl sanl;
		socklen_t sanllen = sizeof(struct sockaddr_nl);
		
		struct nlmsghdr *h;
		int amt;
		char buf[8192];
		
		amt = recvfrom(rth->fd, buf, sizeof(buf), MSG_DONTWAIT, (struct sockaddr*)&sanl, &sanllen);
		if(amt < 0)
		{
			if(errno != EINTR && errno != EAGAIN)
			{
				fprintf(stderr, "%s: error reading netlink: %s.\n",
					__PRETTY_FUNCTION__, strerror(errno));
			}
			return;
		}
		
		if(amt == 0)
		{
			fprintf(stderr, "%s: EOF on netlink??\n", __PRETTY_FUNCTION__);
			return;
		}
		
		h = (struct nlmsghdr*)buf;
		while(amt >= (int)sizeof(*h))
		{
			int len = h->nlmsg_len;
			int l = len - sizeof(*h);
			
			if(l < 0 || len > amt)
			{
				fprintf(stderr, "%s: malformed netlink message: len=%d\n", __PRETTY_FUNCTION__, len);
				break;
			}
			
			switch(h->nlmsg_type)
			{
			case RTM_NEWLINK:
			case RTM_DELLINK:
				LinkCatcher(h);
				break;
			default:
#if 1
				fprintf(stderr, "%s: got nlmsg of type %#x.\n", __PRETTY_FUNCTION__, h->nlmsg_type);
#endif
				break;
			}
			
			len = NLMSG_ALIGN(len);
			amt -= len;
			h = (struct nlmsghdr*)((char*)h + len);
		}
		
		if(amt > 0)
			fprintf(stderr, "%s: remnant of size %d on netlink\n", __PRETTY_FUNCTION__, amt);
    }
}



int rtnl_setup(void)
{
	
	int opt;
	/* Open netlink channel */
	if(rtnl_open(&rthandle, RTMGRP_LINK) < 0)
    {
		perror("Can't initialize rtnetlink socket");
		return(1);
    }
    return 0;
}

int rtnl_cleanup(void)
{
    rtnl_close(&rthandle);
    return 0;
}