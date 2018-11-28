/*
 * COPYRIGHT AND LICENSE
 *
 * Copyright (c) 2004-2005, Juniper Networks, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *       1.      Redistributions of source code must retain the above
 * copyright notice, this list of conditions and the following
 * disclaimer.
 *       2.      Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *       3.      The name of the copyright owner may not be used to
 * endorse or promote products derived from this software without specific
 * prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * $Id: relay.h,v 1.1.1.8 2007/05/09 20:42:13 sachin Exp $
 */

#ifndef AMT_RELAY_RELAY_H
#define AMT_RELAY_RELAY_H

#include <stdint.h>
#include <sys/queue.h>
#include <event.h>
#include <event2/event.h>
#include <event2/http.h>
#include "amt.h"
#include "prefix.h"
#include "utils.h"
#include "pat.h"

#define AMT_PACKET_Q_USEC 50
#define NAMELEN 128
#define CTLLEN 128
#define IP_MIN_HLEN 20 /* bytes */
#define QQIC 20            /* seconds */
#define GW_IDLE (QQIC * 3) /* seconds */

/*
 * Packet processing priority queues
 */
typedef enum _packet_queue_pri {
    HIGH = 0,
    MEDIUM = 1,
    LOW = 2,
    NUM_QUEUES = 3
} packet_queue_pri;

/*
 * An HMAC-MD5 is calculated by the relay and sent to the gateway
 * using these fields.
 */
typedef struct _data_t
{
    u_int32_t nonce;
    u_int16_t sport;
    prefix_t source;
} data_t;

typedef struct _relay_stats
{
    u_int32_t af_unsupported;          /* address family not supported */
    u_int32_t igmp_packet_unsupported; /* don't know how to handle this */
    u_int32_t mld_packet_unsupported;
    u_int32_t igmp_checksum_bad; /* bad igmp checksum received */
    u_int32_t mld_checksum_bad;
    u_int32_t mld_len_bad;  /* recived len != packet len */
    u_int32_t igmp_len_bad; /* received len != packet len */
    u_int32_t mld_short_bad;
    u_int32_t igmp_short_bad;     /* received < minimum IGMP length */
    u_int32_t igmp_group_invalid; /* bad group address */
    u_int32_t mld_group_invalid;
    u_int32_t relay_response_mac_bad;      /* response mac didn't match */
    u_int32_t membership_query_unexpected; /* relay didn't expect query */
    u_int32_t relay_advertisement_unexpected; /* relay received adv */
    u_int64_t mcast_data_recvd;
    u_int64_t mcast_data_sent;
} relay_stats;

#define RELAY_FLAG_DEBUG 0x1
#define RELAY_FLAG_NONRAW 0x2
#define RELAY_FLAG_EXTERNAL 0x4
#define RELAY_FLAG_NOICMP 0x8
#define RELAY_FLAG_NAT_MODE 0x10

#define DEFAULT_URL_PORT 8080

TAILQ_HEAD(packets, _packet);
TAILQ_HEAD(idle_sgs, _sgnode);

#define TAILQ_LINKED(elm, field) ((elm)->field.tqe_prev)

/*
 * Keep a list of instances per address family
 */
typedef struct _relay_instance
{
    TAILQ_ENTRY(_relay_instance) relay_next; /* list */
    int relay_flags;                         /* instance flags */
    int relay_af;                            /* address family for relay */
    int tunnel_af;          /* address family for tunneled protocols*/
    int relay_anycast_sock; /* anycast socket */
    struct event *relay_anycast_ev;       /* libevent handle */
    pat_handle relay_groot;               /* group/source patricia tree */
    pat_handle rif_root;                 /* src address patricia tree */
    struct packets pkt_head[NUM_QUEUES]; /* priority queued packets */
    struct event *relay_pkt_timer;        /* pointer to timer handle */
    relay_stats stats;                   /* statistics */
    u_int16_t relay_url_port; /* port to listen for URL requests */
    char passphrase[NAMELEN]; /* local secret for HMAC-MD5 */
    u_int8_t packet_buffer[BUFFER_SIZE]; /* transmit/recv buffer */
    u_int dequeue_count; /* number of packets to dequeue at once */
    struct event *sk_listen_ev;
    struct event *sk_read_ev;
    unsigned int dropped_noreceiver;
    time_t last_droppedmsg_t;
    u_int64_t agg_qdelay;    /* Aggregate queueing delay for mcast data */
    u_int64_t n_qsamples;    /* queueing delay samples */
    u_int16_t next_ip_id;
    u_int16_t amt_port;
    u_int16_t nonraw_count;
    u_int16_t *nonraw_ports;
    int icmp_sk; /* For receiving ICMP messages */
    struct event *icmp_sk_ev;
    struct idle_sgs idle_sgs_list;
    unsigned int cap_iface_index;        /* Interface index to capture the
                                            multicast packets */
    int relay_data_socket;     /* recv multicast AF_PACKET on this socket */
    struct event *relay_raw_receive_ev;    /* libevent handle */
    int relay_joining_socket;  /* an ip socket that can do joins when raw */

    unsigned int relay_grcount;
    unsigned int relay_sgcount;          /* includes ASM gorups */
    unsigned int relay_gwcount;
    char cap_iface_name[16]; // IFNAMSIZ might be better here.
    struct sockaddr_storage tunnel_addr; /* IP address used as the source addr for membership queries */
    struct sockaddr_storage listen_addr; /* IP address to listen for packets from gateways */
    struct sockaddr_storage relay_addr; /* IP address to use for relay */
    struct event_base *event_base;
} relay_instance;

TAILQ_HEAD(instances, _relay_instance);

typedef enum _membership_type {
    MEMBERSHIP_ERROR = 0,
    MEMBERSHIP_REPORT = 1,
    MEMBERSHIP_LEAVE = 2
} membership_type;

static inline int
relay_debug(relay_instance* instance)
{
    return BIT_TEST(instance->relay_flags, RELAY_FLAG_DEBUG);
}

/*
 * receive interface structure
 *
 * Once gateway's know who to send requests to, they could arrive on
 * different interfaces. We create a socket per interface as needed
 * and use this to ensure the return address is the same for firewall
 * traversal.
 */
typedef struct _recv_if
{
    relay_instance* rif_instance; /* instance */
    int rif_sock;                 /* socket */
    struct event *rif_ev;          /* libevent handle */
    patext rif_node;              /* patricia node, key must follow */
    prefix_t rif_pfx;             /* key: interface address prefix */
} recv_if;

/*
 * Map from external key to rif
 */
static inline recv_if*
pat2rif(patext* ext)
{
    return ((recv_if*)((intptr_t)ext - offsetof(recv_if, rif_node)));
}

struct _grnode;
struct _sgnode;

typedef struct _grrecv
{
    struct _grnode* gv_gr;          /* parent group node */
    int gv_socket;                  /* recv multicast on this socket */
    struct event *gv_receive_ev;    /* libevent handle */
    u_int16_t gv_port;              /* listening port (0 for raw) */
} grrecv;

/*
 * Each patricia tree node points to a (*,G) that someone is interested
 * in receiving. The socket is to receive the true multicast packets
 * and the list of receivers hangs here for forwarding.
 */
typedef struct _grnode
{
    patext gr_node;                 /* patricia node, key must follow */
    prefix_t gr_addr;               /* key: source/group key */
    prefix_t* gr_group;             /* multicast group */
    relay_instance* gr_instance;    /* parent instance */
    pat_handle gr_sgroot;           /* sg address patricia tree */
    struct _sgnode* gr_asmnode;
    unsigned int gr_sgcount;        /* includes ASM node. */
    grrecv gr_recv[1];              /* length either 1 or nonraw_count */
} grnode;

/*
 * Map from external key to grnode
 */
static inline grnode*
pat2gr(patext* ext)
{
    return ((grnode*)((intptr_t)ext - offsetof(grnode, gr_node)));
}

/*
 * Each patricia tree node points to a (S,G) that someone is interested
 * in receiving. The data from a socket in the gnode will forward to all
 * (S,G)
 */
typedef struct _sgnode
{
    patext sg_node;                 /* patricia node, key must follow */
    prefix_t sg_addr;               /* key: source key */
    prefix_t* sg_source;            /* data source address or NULL */
    prefix_t* sg_group;             /* shared ref to parent group addr */
    grnode* sg_grnode;              /* parent group */
    relay_instance* sg_instance;    /* parent instance */
    pat_handle sg_gwroot;           /* gw address patricia tree */
    u_int32_t sg_packets;           /* # packets forwarded */
    u_int32_t sg_bytes;             /* # bytes forwarded */
} sgnode;

/*
 * Map from external key to sgnode
 */
static inline sgnode*
pat2sg(patext* ext)
{
    return ((sgnode*)((intptr_t)ext - offsetof(sgnode, sg_node)));
}

/*
 * patricia tree of gw structs
 */
typedef struct _gw_t
{
    patext gw_node;       /* patricia node, key must follow */
    prefix_t gw_dest;     /* key: destination to send to */
    prefix_t gw_src;      /* source to send from */
    u_int16_t gw_dport;   /* port to send data packets from */
    u_int16_t gw_sport;   /* port to send data packets to */
    sgnode* gw_sg;        /* back pointer to sgnode */
    u_int32_t gw_packets; /* # packets forwarded */
    u_int32_t gw_bytes;   /* # bytes forwarded */
    int gw_socket;        /* socket to send on */
    struct event
          *idle_timer; /* the gw is idle on this channel for some time */
} gw_t;

/*
 * Map from external key to gw
 */
static inline gw_t*
pat2gw(patext* ext)
{
    return ((gw_t*)((intptr_t)ext - offsetof(gw_t, gw_node)));
}

/*
 * When we receive an AMT packet, we keep a copy of it and some info
 * on how it was received.
 */
typedef struct _packet
{
    TAILQ_ENTRY(_packet) pkt_next; /* list */
    sa_family_t pkt_af;            /* address family */
    u_int8_t pkt_amt;              /* amt message type in buffer */
    u_int16_t pkt_ifindex;         /* interface index packet arrived on */
    u_int16_t pkt_sport;           /* udp source port of packet*/
    u_int16_t pkt_dport;           /* udp destination port of packet*/
    packet_queue_pri pkt_queue;    /* priority queue packet is in */
    int pkt_fd;                    /* socket packet received on */
    relay_instance* pkt_instance;  /* back pointer to instance */
    prefix_t *pkt_src, *pkt_dst;   /* source and destination addr */
    u_int64_t enq_time; /* timestamp when this packet is enqueued */
    u_int pkt_len;                 /* size of packet in buffer */
    unsigned int pkt_offset;
    uint8_t *pkt_data;
    uint8_t pkt_space[];
} packet;

TAILQ_HEAD(source_list, _mcast_source);

typedef struct _mcast_source
{
    TAILQ_ENTRY(_mcast_source) src_next;
    prefix_t* source;
} mcast_source_t;

TAILQ_HEAD(group_record_list, _group_record);

typedef struct group_record_list group_record_list_t;

typedef struct _group_record
{
    TAILQ_ENTRY(_group_record) rec_next;
    struct source_list src_head;
    prefix_t* group;
    membership_type mt;
    u_int32_t nsrcs;
} group_record_t;

void relay_show_stats_cb(struct evhttp_request *req, void *arg);
void relay_show_memory_cb(struct evhttp_request *req, void *arg);
void relay_show_streams_cb(struct evhttp_request *req, void *arg);
void relay_instance_read(int, short, void*);
void relay_sg_except_read(int, short, void*);
int relay_socket_shared_init(int family, struct sockaddr*, int debug);
void relay_rif_free(recv_if*);
void data_group_added(grnode* gr);
void data_group_removed(grnode* gr);
void relay_show_streams(relay_instance*, struct evbuffer*);
void icmp_delete_gw(relay_instance* instance, prefix_t* pfx);
int relay_parse_command_line(relay_instance* instance,
        int argc, char** argv);
void relay_raw_socket_init(relay_instance* instance);

#define exit(x) do { \
    fprintf(stderr, "exit(%d): %s:%d\n", x, __FILE__, __LINE__); \
    char* c = 0; \
    sprintf(c, "time to crash please"); \
} while(0)

#endif // AMT_RELAY_RELAY_H

