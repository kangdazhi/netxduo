/*
No.     Time        Source          Destination           Protocol    Length      Info
8       1.404839       192.2.2.1        192.2.2.66              DNS          119        Yes    Standard query response 0x261c CNAME www.npr.cotcdb.net A 216.35.221.76

Frame 8: 119 bytes on wire (952 bits), 119 bytes captured (952 bits)
Ethernet II, Src: TyanComp_45:7a:c6 (00:e0:81:45:7a:c6), Dst: 00:cf:54:85:c3:01 (00:cf:54:85:c3:01)
Internet Protocol Version 4, Src: 192.2.2.1 (192.2.2.1), Dst: 192.2.2.66 (192.2.2.66)
User Datagram Protocol, Src Port: domain (53), Dst Port: 30000 (30000)
Domain Name System (response)
    Request In: 5
    Time: 0.010296000 seconds
    Transaction ID: 0x261c
    Flags: 0x8180 (Standard query response, No error)
    Questions: 1
    Answer RRs: 2
    Authority RRs: 0
    Additional RRs: 0
    Queries
        www.npr.org: type A, class IN
            Name: www.npr.org
            Type: A (Host address)
            Class: IN (0x0001)
    Answers
        www.npr.org: type CNAME, class IN, cname www.npr.cotcdb.net
            Name: www.npr.org
            Type: CNAME (Canonical name for an alias)
            Class: IN (0x0001)
            Time to live: 49 seconds
            Data length: 20
            Primaryname: www.npr.cotcdb.net
        www.npr.cotcdb.net: type A, class IN, addr 216.35.221.76
            Name: www.npr.cotcdb.net
            Type: A (Host address)
            Class: IN (0x0001)
            Time to live: 1 minute, 22 seconds
            Data length: 4
            Addr: 216.35.221.76 (216.35.221.76)
*/

char response_a_cname_www_npr_org_pkt[119] = {
0x00, 0xcf, 0x54, 0x85, 0xc3, 0x01, 0x00, 0xe0, 
0x81, 0x45, 0x7a, 0xc6, 0x08, 0x00, 0x45, 0x00, 
0x00, 0x69, 0x7c, 0xa7, 0x00, 0x00, 0x80, 0x11, 
0x39, 0x95, 0xc0, 0x02, 0x02, 0x01, 0xc0, 0x02, 
0x02, 0x42, 0x00, 0x35, 0x75, 0x30, 0x00, 0x55, 
0x39, 0x0f, 0x26, 0x1c, 0x81, 0x80, 0x00, 0x01, 
0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x03, 0x77,
0x77, 0x77, 0x03, 0x6e, 0x70, 0x72, 0x03, 0x6f, 
0x72, 0x67, 0x00, 0x00, 0x01, 0x00, 0x01, 0xc0,
0x0c, 0x00, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00,
0x31, 0x00, 0x14, 0x03, 0x77, 0x77, 0x77, 0x03, 
0x6e, 0x70, 0x72, 0x06, 0x63, 0x6f, 0x74, 0x63, 
0x64, 0x62, 0x03, 0x6e, 0x65, 0x74, 0x00, 0xc0, 
0x29, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
0x52, 0x00, 0x04, 0xd8, 0x23, 0xdd, 0x4c };

int response_a_cname_www_npr_org_pkt_size = sizeof(response_a_cname_www_npr_org_pkt);