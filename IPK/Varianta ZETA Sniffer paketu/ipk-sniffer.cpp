#include <cstring>
#include <getopt.h>
#include <pcap.h>
#include <iomanip>
#include <iostream>
#include <arpa/inet.h>
//#include <sys/socket.h>
//#include <netinet/if_ether.h>
//#include <netinet/ether.h>
#include <time.h>
#include "ipk-sniffer.hpp"

using namespace std;

#define IPV4_ET 2048
#define IPV6_ET 34525
#define ARP_ET 2054
#define ETH_HEADER_SIZE 14
#define STRING_POP(str, x) \
    for (int i = 0; i < x; i++) {\
        str.pop_back();\
    }

//check size
void check_header_size(int size, int min, int max, string type) {
    if (size < min || size > max) {
        fprintf(stderr, "Invalid %s header size: %d", type.c_str(), size);
        exit(EXIT_FAILURE);
    }
}

//check for number
bool is_number(char *string) {
    for (size_t i = 0; i < strlen(string); i++) {
        if (!isdigit(string[i])) return false;
    }
    return true;
}

//print packet: offset hex ascii
void print_packet(const u_char *packet, int size) {
    u_char *payload = (u_char*)packet;  //save data
    int payload_size = size;            //save size
    int width = 16;                     //set width
    int offset = 0;                     //offset
    string data_string = " ";           //ascii string

    //while not at the end
    while (payload_size > 0) {
        printf("0x%04X:  ", offset); //print offset

        //print hex data
        for(int i = 0; i < width; i++) {
            if (payload_size <= 0) {
                printf("   "); //print spaces once on end
            } else {
                printf("%02X ", *payload);
                //if printable, save it to string and for later printing
                if (isprint(*payload)) {
                    char c = *payload;
                    data_string.push_back(c);
                } else {
                    data_string.push_back('.');
                }
                payload++; //move to next char/hex
            }
            payload_size--; //deprecated current size

            if (i == 7) cout << " "; //print extra space after eight hex
        }

        printf("%s\n", data_string.c_str());
        data_string = " "; //reset string
        offset += width;   //add to offset
    }
}

//on correct protocol
void got_protocol(const u_char *packet, int protocol, int offset, int size, string source_addr, string dest_addr) {
    //create headers
    struct tcp_header *tcp;
    struct udp_header *udp;

    //select and print protocol specific information
    switch (protocol) {
        case IPPROTO_TCP:
            tcp = (struct tcp_header *)(packet + ETH_HEADER_SIZE + offset); //save data
            check_header_size(TCP_DOF(tcp)*4, 20, 60, "TCP");               //check size
            printf(" %s : %d > %s : %d", source_addr.c_str(), ntohs(tcp->source), dest_addr.c_str(), ntohs(tcp->dest));
            break;
        case IPPROTO_UDP:
            udp = (struct udp_header *)(packet + ETH_HEADER_SIZE + offset); //save data
            check_header_size(udp->len, 8, 65535, "UDP");                   //check size
            printf(" %s : %d > %s : %d", source_addr.c_str(), ntohs(udp->source), dest_addr.c_str(), ntohs(udp->dest));
            break;
        case IPPROTO_ICMP:
        case IPPROTO_ICMPV6:
        case -99:
            printf(" %s > %s", source_addr.c_str(), dest_addr.c_str());
            break;
        default:
            fprintf(stderr, "Invalid protocol %d\n", protocol);
            exit(EXIT_FAILURE);
            break;
    }
    printf(", lenght %d bytes\n", size);
    print_packet(packet, size);
}

//on packet capture
void on_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    //get and print time
    time_t rawtime;
    time(&rawtime);
    struct tm *ptm;
    ptm = gmtime(&rawtime);
    string usec = to_string(header->ts.tv_usec);
    STRING_POP(usec, 3)
    cout << put_time(ptm, "%F") << "T" << put_time(ptm, "%T") << "." << usec << "Z";

    //header structs
    struct ethernet_header *ethernet;
    struct ipv4_header *ipv4;
    struct ipv6_header *ipv6;
    int ip_size;  //header size
    int protocol; //protocol
    char mac[22]; //mac tmp storage for arp
    string from = "";
    string to = "";
    ethernet = (struct ethernet_header *) packet; //get ethernet header from packet

    switch (ntohs(ethernet->ether_type)) {
        case IPV4_ET:
            ipv4 = (struct ipv4_header *)(packet + ETH_HEADER_SIZE);
            ip_size = IPv4_IHL(ipv4) * 4;
            check_header_size(ip_size, 20, 60, "IPv4");
            from = inet_ntoa(ipv4->source);
            to = inet_ntoa(ipv4->dest);
            protocol = ipv4->proto;
            break;
        case ARP_ET:
            protocol = -99;
            sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
                    ethernet->source[0],
                    ethernet->source[1],
                    ethernet->source[2],
                    ethernet->source[3],
                    ethernet->source[4],
                    ethernet->source[5]);
            from = mac;
            sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
                    ethernet->dest[0],
                    ethernet->dest[1],
                    ethernet->dest[2],
                    ethernet->dest[3],
                    ethernet->dest[4],
                    ethernet->dest[5]);
            to = mac;
            break;
        case IPV6_ET:
            ipv6 = (struct ipv6_header *)(packet + ETH_HEADER_SIZE);
            ip_size = 40; //fixed lenght
            char buf6[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &(ipv6->source), buf6, sizeof(buf6));
            from = buf6;
            inet_ntop(AF_INET6, &(ipv6->dest), buf6, sizeof(buf6));
            to = buf6;
            protocol = ipv6->next_header;
            break;
        default: 
            fprintf(stderr, "Invalid protocol '%x'\n", ntohs(ethernet->ether_type));
            exit(EXIT_FAILURE);
            break;
    }

    got_protocol(packet, protocol, ip_size, header->len, from, to);
}

//create filter string
string get_filter(bool protocols[4], int port) {
    string expression = "";
    if (protocols[2]) {
        expression.append("(icmp or icmp6");
        if (protocols[3]) {
            expression.append(" or arp");
        }
        expression.append(") or ");
    }
    else if (protocols[3]) {
        expression.append("arp or ");
    }
    if (!protocols[1] && !protocols[0]) {
        STRING_POP(expression, 3);
    }
    if (protocols[0]) {
        expression.append("((tcp");
        if (protocols[1]) {
            expression.append(" or udp");
        }
        expression.push_back(')');
        if (port != -1) {
            expression.append(" and port ");
            expression.append(to_string(port));
        }
        expression.push_back(')');
    }
    else if (protocols[1]){
        expression.append("(udp");
        if (port != -1) {
            expression.append(" and port ");
            expression.append(to_string(port));
        }
        expression.push_back(')');
    }
    return expression;
}

//print active interfaces
void print_interfaces() {
    char error[PCAP_ERRBUF_SIZE];
    pcap_if_t *interfaces, *intf;

    if (pcap_findalldevs(&interfaces, error) == -1) {
        fprintf(stderr, "Error finding devices: %s\n", error);
        exit(EXIT_FAILURE);
    }

    for (intf = interfaces; intf != nullptr; intf = intf->next) {
        if (intf->addresses != 0) {
            printf("%s\n", intf->name);
        }
    }
    pcap_freealldevs(interfaces);
}

//manage arguments
arguments_t manage_arguments(int argc, char *argv[]) {
    //no argument given - print interfaces
    if (argc == 1) {
        print_interfaces();
        exit(EXIT_SUCCESS);
    }

    arguments_t arguments = {"", false, {false, false, false, false}, -1, 1}; //arguments struct
    int opt;                                                                  //selected option
    int index = 0;                                                            //index for long options
    static struct option long_options[] = {                                   //long options struct
        {"interface", optional_argument, 0, 'i'},
        {"tcp",       no_argument,       0, 't'},
        {"udp",       no_argument,       0, 'u'},
        {"icmp",      no_argument,       0,  0 },
        {"arp",       no_argument,       0,  0 }
    };
    //go throught arguments and flip/save corret values
    while ((opt = getopt_long(argc, argv, "i::p:tun:", long_options, &index)) != -1 ) {
        switch (opt) {
        case 0:
            if (string{long_options[index].name} == "icmp") arguments.ptcs[2] = true;
            if (string{long_options[index].name} == "arp") arguments.ptcs[3] = true;
            break;
        case 'i': arguments.intf = true;   break;
        case 't': arguments.ptcs[0] = true; break;
        case 'u': arguments.ptcs[1] = true; break;
        case 'p':
        case 'n':
            //check that value is a number
            if (!is_number(optarg)) {
                fprintf(stderr, "Value '%s' is not a number\n", optarg);
                exit(EXIT_FAILURE);
            }
            if (opt == 'n') arguments.packet_cnt = std::stoi(optarg);
            if (opt == 'p') arguments.port = std::stoi(optarg);
            break;
        //exit on unknown option
        case '?':
        default:
            exit(EXIT_FAILURE);
        }
    }
    if (!arguments.intf && argc > 1) {
        fprintf(stderr, "No network device given\n");
        exit(EXIT_FAILURE);
    }
    //if there is an interface value, save it. Otherwise print interfaces
    if (optind < argc) arguments.interface = argv[optind];
    else {
        print_interfaces();
        exit(EXIT_SUCCESS);
    }
    //no protocols == all protocols
    if (!arguments.ptcs[0] && !arguments.ptcs[1] && !arguments.ptcs[2] && !arguments.ptcs[3]) {
        for (int i = 0; i < 4; i++) {
            arguments.ptcs[i] = true;
        }
    }
    return arguments;
}

int main(int argc, char *argv[]){
    arguments_t arguments = manage_arguments(argc, argv); //get arguments
    char errbuf[PCAP_ERRBUF_SIZE];                        //error buffer
    bpf_u_int32 net;                                      //ip
    bpf_u_int32 mask;                                     //mask
    pcap_t *descr;                                        //set device
    struct bpf_program fp;                                //filter
    string filter_exp;                                    //filter expression

    //search for interface
    if (pcap_lookupnet(arguments.interface.c_str(), &net, &mask, errbuf) == -1) { 
        fprintf(stderr, "Device '%s' not found\n", arguments.interface.c_str());
        exit(EXIT_FAILURE);
    }

    //open device for capture
    descr = pcap_open_live(arguments.interface.c_str(), BUFSIZ, 0, 1000, errbuf);
    if (descr == NULL) {
        fprintf(stderr, "Failed to open device: %s\n", errbuf);
        exit(EXIT_FAILURE);
    }

    filter_exp = get_filter(arguments.ptcs, arguments.port); //get filter expression
    //compile filter
    if (pcap_compile(descr, &fp, filter_exp.c_str(), 0, net) == -1) {
        fprintf(stderr, "Failed to parse filter '%s': %s\n", filter_exp.c_str(), pcap_geterr(descr));
        exit(EXIT_FAILURE);
    }
    //set filter
    if (pcap_setfilter(descr, &fp) == -1) {
        fprintf(stderr, "Failed to install filter '%s': %s\n", filter_exp.c_str(), pcap_geterr(descr));
        exit(EXIT_FAILURE);
    }

    //capture n packages
    pcap_loop(descr, arguments.packet_cnt, on_packet, nullptr);

    //free filter and device
    pcap_freecode(&fp);
    pcap_close(descr);

    //exit
    return(EXIT_SUCCESS);
}