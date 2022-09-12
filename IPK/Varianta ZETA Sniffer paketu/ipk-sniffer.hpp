#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <netinet/in.h>

#define MAC_LEN 6
#define IPv4_IHL(ip) (ip->ver_ihl & 0x0f)
#define TCP_DOF(tcp) (tcp->data_offset >> 4)

//struct prototypes
//arguments
struct arguments_t {
    std::string interface;
    bool intf;
    bool ptcs[4];
    int port;
    int packet_cnt;
};

//ethernet frame header
struct ethernet_header {
  uint8_t  dest[MAC_LEN];	/* destination eth addr	*/
  uint8_t  source[MAC_LEN];	/* source ether addr	*/
  uint16_t ether_type;		        /* packet type ID field	*/
};

//ipv4
struct ipv4_header {
    uint8_t ver_ihl;
    uint8_t tos;
    uint16_t len;
    uint16_t id;
    uint16_t frag_offset;
    uint8_t ttl;
    uint8_t proto;
    uint16_t checksum;
    struct in_addr source;
    struct in_addr dest;
};

//ipv6
struct ipv6_header {
    uint32_t ver_tc_flow;
    uint16_t len;
    uint8_t next_header;
    uint8_t hop_lim;
    struct in6_addr source;
    struct in6_addr dest;
};

//tcp
struct tcp_header {
    uint16_t source;
    uint16_t dest;
    uint32_t seq;
    uint32_t ack;
    uint8_t data_offset;
    uint8_t flags;
    uint16_t window_size;
    uint16_t checksum;
    uint16_t urgp;
};

//udp
struct udp_header {
    uint16_t source;
    uint16_t dest;
    uint16_t len;
    uint16_t checksum;
};

//arp
struct arp_header {
    uint16_t hw_type;
    uint16_t ptc_type;
    uint8_t hw_len;
    uint8_t ptc_len;
    uint16_t operation;
    uint8_t sender_hw_address[6];
    uint32_t sender_ptc_address;
    uint8_t target_hw_address[6];
    uint32_t target_ptc_address;
};