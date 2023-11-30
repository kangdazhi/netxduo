
#include "nx_api.h"

#ifdef __PRODUCT_NETXDUO__

#include "netx_mdns_test.h"

/* Frame (225 bytes) */
static const unsigned char pkt1[225] = {
0x01, 0x00, 0x5e, 0x00, 0x00, 0xfb, 0x00, 0x11, /* ..^..... */
0x22, 0x33, 0x44, 0x56, 0x08, 0x00, 0x45, 0x00, /* "3DV..E. */
0x00, 0xd3, 0x00, 0x07, 0x40, 0x00, 0xff, 0x11, /* ....@... */
0x8f, 0xd5, 0x0a, 0x00, 0x00, 0x42, 0xe0, 0x00, /* .....B.. */
0x00, 0xfb, 0x14, 0xe9, 0x14, 0xe9, 0x00, 0xbf, /* ........ */
0x13, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, /* .w...... */
0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0f, 0x53, /* .......S */
0x69, 0x6d, 0x70, 0x6c, 0x65, 0x57, 0x65, 0x62, /* impleWeb */
0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x05, 0x5f, /* Server._ */
0x68, 0x74, 0x74, 0x70, 0x04, 0x5f, 0x74, 0x63, /* http._tc */
0x70, 0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x00, /* p.local. */
0x00, 0xff, 0x00, 0x01, 0x0f, 0x53, 0x69, 0x6d, /* .....Sim */
0x70, 0x6c, 0x65, 0x57, 0x65, 0x62, 0x53, 0x65, /* pleWebSe */
0x72, 0x76, 0x65, 0x72, 0x05, 0x5f, 0x68, 0x74, /* rver._ht */
0x74, 0x70, 0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, /* tp._tcp. */
0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x00, 0x00, 0x21, /* local..! */
0x80, 0x01, 0x00, 0x00, 0x00, 0x64, 0x00, 0x19, /* .....d.. */
0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x0b, 0x41, /* .....P.A */
0x52, 0x4d, 0x4d, 0x44, 0x4e, 0x53, 0x54, 0x65, /* RMMDNSTe */
0x73, 0x74, 0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, /* st.local */
0x00, 0x0f, 0x53, 0x69, 0x6d, 0x70, 0x6c, 0x65, /* ..Simple */
0x57, 0x65, 0x62, 0x53, 0x65, 0x72, 0x76, 0x65, /* WebServe */
0x72, 0x05, 0x5f, 0x68, 0x74, 0x74, 0x70, 0x04, /* r._http. */
0x5f, 0x74, 0x63, 0x70, 0x05, 0x6c, 0x6f, 0x63, /* _tcp.loc */
0x61, 0x6c, 0x00, 0x00, 0x10, 0x80, 0x01, 0x00, /* al...... */
0x00, 0x00, 0x64, 0x00, 0x14, 0x08, 0x70, 0x61, /* ..d...pa */
0x70, 0x65, 0x72, 0x3d, 0x41, 0x34, 0x0a, 0x76, /* per=A4.v */
0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x3d, 0x30, /* ersion=0 */
0x31                                            /* 1 */
};

/* Frame (235 bytes) */
static const unsigned char pkt2[235] = {
0x01, 0x00, 0x5e, 0x00, 0x00, 0xfb, 0xec, 0x17, /* ..^..... */
0x2f, 0xbc, 0x39, 0x02, 0x08, 0x00, 0x45, 0x00, /* /.9...E. */
0x00, 0xdd, 0x05, 0x71, 0x00, 0x00, 0xff, 0x11, /* ...q.... */
0xca, 0xa2, 0x0a, 0x00, 0x00, 0x01, 0xe0, 0x00, /* ........ */
0x00, 0xfb, 0x14, 0xe9, 0x14, 0xe9, 0x00, 0xc9, /* ........ */
0x3d, 0x40, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00, /* =@...... */
0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x0f, 0x53, /* .......S */
0x69, 0x6d, 0x70, 0x6c, 0x65, 0x57, 0x65, 0x62, /* impleWeb */
0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x05, 0x5f, /* Server._ */
0x68, 0x74, 0x74, 0x70, 0x04, 0x5f, 0x74, 0x63, /* http._tc */
0x70, 0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x00, /* p.local. */
0x00, 0x21, 0x80, 0x01, 0x00, 0x00, 0x00, 0x78, /* .!.....x */
0x00, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, /* .......P */
0x09, 0x43, 0x68, 0x65, 0x6e, 0x62, 0x6f, 0x2d, /* .Chenbo- */
0x50, 0x43, 0xc0, 0x27, 0xc0, 0x0c, 0x00, 0x10, /* PC.'.... */
0x80, 0x01, 0x00, 0x00, 0x11, 0x94, 0x00, 0x16, /* ........ */
0x09, 0x5b, 0x70, 0x61, 0x70, 0x65, 0x72, 0x3d, /* .[paper= */
0x41, 0x34, 0x0b, 0x56, 0x65, 0x72, 0x73, 0x69, /* A4.Versi */
0x6f, 0x6e, 0x3d, 0x30, 0x31, 0x5d, 0xc0, 0x3e, /* on=01].> */
0x00, 0x01, 0x80, 0x01, 0x00, 0x00, 0x00, 0x78, /* .......x */
0x00, 0x04, 0x0a, 0x00, 0x00, 0x01, 0xc0, 0x3e, /* .......> */
0x00, 0x1c, 0x80, 0x01, 0x00, 0x00, 0x00, 0x78, /* .......x */
0x00, 0x10, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0xc0, 0x93, 0x9d, 0xaa, 0xda, 0xe3, /* ........ */
0xea, 0xba, 0xc0, 0x3e, 0x00, 0x2f, 0x80, 0x01, /* ...>./.. */
0x00, 0x00, 0x00, 0x78, 0x00, 0x08, 0xc0, 0x3e, /* ...x...> */
0x00, 0x04, 0x40, 0x00, 0x00, 0x08, 0xc0, 0x0c, /* ..@..... */
0x00, 0x2f, 0x80, 0x01, 0x00, 0x00, 0x00, 0x78, /* ./.....x */
0x00, 0x09, 0xc0, 0x0c, 0x00, 0x05, 0x00, 0x00, /* ........ */
0x80, 0x00, 0x40                                /* ..@ */
};

/* Frame (237 bytes) */
static const unsigned char pkt3[237] = {
0x01, 0x00, 0x5e, 0x00, 0x00, 0xfb, 0x00, 0x11, /* ..^..... */
0x22, 0x33, 0x44, 0x56, 0x08, 0x00, 0x45, 0x00, /* "3DV..E. */
0x00, 0xdf, 0x00, 0x08, 0x40, 0x00, 0xff, 0x11, /* ....@... */
0x8f, 0xc8, 0x0a, 0x00, 0x00, 0x42, 0xe0, 0x00, /* .....B.. */
0x00, 0xfb, 0x14, 0xe9, 0x14, 0xe9, 0x00, 0xcb, /* ........ */
0x15, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, /* .f...... */
0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x13, 0x53, /* .......S */
0x69, 0x6d, 0x70, 0x6c, 0x65, 0x57, 0x65, 0x62, /* impleWeb */
0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, 0x28, /* Server ( */
0x32, 0x29, 0x05, 0x5f, 0x68, 0x74, 0x74, 0x70, /* 2)._http */
0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, 0x6c, 0x6f, /* ._tcp.lo */
0x63, 0x61, 0x6c, 0x00, 0x00, 0xff, 0x00, 0x01, /* cal..... */
0x13, 0x53, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x57, /* .SimpleW */
0x65, 0x62, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, /* ebServer */
0x20, 0x28, 0x32, 0x29, 0x05, 0x5f, 0x68, 0x74, /*  (2)._ht */
0x74, 0x70, 0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, /* tp._tcp. */
0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x00, 0x00, 0x21, /* local..! */
0x80, 0x01, 0x00, 0x00, 0x00, 0x64, 0x00, 0x19, /* .....d.. */
0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x0b, 0x41, /* .....P.A */
0x52, 0x4d, 0x4d, 0x44, 0x4e, 0x53, 0x54, 0x65, /* RMMDNSTe */
0x73, 0x74, 0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, /* st.local */
0x00, 0x13, 0x53, 0x69, 0x6d, 0x70, 0x6c, 0x65, /* ..Simple */
0x57, 0x65, 0x62, 0x53, 0x65, 0x72, 0x76, 0x65, /* WebServe */
0x72, 0x20, 0x28, 0x32, 0x29, 0x05, 0x5f, 0x68, /* r (2)._h */
0x74, 0x74, 0x70, 0x04, 0x5f, 0x74, 0x63, 0x70, /* ttp._tcp */
0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x00, 0x00, /* .local.. */
0x10, 0x80, 0x01, 0x00, 0x00, 0x00, 0x64, 0x00, /* ......d. */
0x14, 0x08, 0x70, 0x61, 0x70, 0x65, 0x72, 0x3d, /* ..paper= */
0x41, 0x34, 0x0a, 0x76, 0x65, 0x72, 0x73, 0x69, /* A4.versi */
0x6f, 0x6e, 0x3d, 0x30, 0x31                    /* on=01 */
};

/* Frame (237 bytes) */
static const unsigned char pkt4[237] = {
0x01, 0x00, 0x5e, 0x00, 0x00, 0xfb, 0x00, 0x11, /* ..^..... */
0x22, 0x33, 0x44, 0x56, 0x08, 0x00, 0x45, 0x00, /* "3DV..E. */
0x00, 0xdf, 0x00, 0x09, 0x40, 0x00, 0xff, 0x11, /* ....@... */
0x8f, 0xc7, 0x0a, 0x00, 0x00, 0x42, 0xe0, 0x00, /* .....B.. */
0x00, 0xfb, 0x14, 0xe9, 0x14, 0xe9, 0x00, 0xcb, /* ........ */
0x15, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, /* .f...... */
0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x13, 0x53, /* .......S */
0x69, 0x6d, 0x70, 0x6c, 0x65, 0x57, 0x65, 0x62, /* impleWeb */
0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, 0x28, /* Server ( */
0x32, 0x29, 0x05, 0x5f, 0x68, 0x74, 0x74, 0x70, /* 2)._http */
0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, 0x6c, 0x6f, /* ._tcp.lo */
0x63, 0x61, 0x6c, 0x00, 0x00, 0xff, 0x00, 0x01, /* cal..... */
0x13, 0x53, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x57, /* .SimpleW */
0x65, 0x62, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, /* ebServer */
0x20, 0x28, 0x32, 0x29, 0x05, 0x5f, 0x68, 0x74, /*  (2)._ht */
0x74, 0x70, 0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, /* tp._tcp. */
0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x00, 0x00, 0x21, /* local..! */
0x80, 0x01, 0x00, 0x00, 0x00, 0x64, 0x00, 0x19, /* .....d.. */
0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x0b, 0x41, /* .....P.A */
0x52, 0x4d, 0x4d, 0x44, 0x4e, 0x53, 0x54, 0x65, /* RMMDNSTe */
0x73, 0x74, 0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, /* st.local */
0x00, 0x13, 0x53, 0x69, 0x6d, 0x70, 0x6c, 0x65, /* ..Simple */
0x57, 0x65, 0x62, 0x53, 0x65, 0x72, 0x76, 0x65, /* WebServe */
0x72, 0x20, 0x28, 0x32, 0x29, 0x05, 0x5f, 0x68, /* r (2)._h */
0x74, 0x74, 0x70, 0x04, 0x5f, 0x74, 0x63, 0x70, /* ttp._tcp */
0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x00, 0x00, /* .local.. */
0x10, 0x80, 0x01, 0x00, 0x00, 0x00, 0x64, 0x00, /* ......d. */
0x14, 0x08, 0x70, 0x61, 0x70, 0x65, 0x72, 0x3d, /* ..paper= */
0x41, 0x34, 0x0a, 0x76, 0x65, 0x72, 0x73, 0x69, /* A4.versi */
0x6f, 0x6e, 0x3d, 0x30, 0x31                    /* on=01 */
};

/* Frame (237 bytes) */
static const unsigned char pkt5[237] = {
0x01, 0x00, 0x5e, 0x00, 0x00, 0xfb, 0x00, 0x11, /* ..^..... */
0x22, 0x33, 0x44, 0x56, 0x08, 0x00, 0x45, 0x00, /* "3DV..E. */
0x00, 0xdf, 0x00, 0x0a, 0x40, 0x00, 0xff, 0x11, /* ....@... */
0x8f, 0xc6, 0x0a, 0x00, 0x00, 0x42, 0xe0, 0x00, /* .....B.. */
0x00, 0xfb, 0x14, 0xe9, 0x14, 0xe9, 0x00, 0xcb, /* ........ */
0x15, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, /* .f...... */
0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x13, 0x53, /* .......S */
0x69, 0x6d, 0x70, 0x6c, 0x65, 0x57, 0x65, 0x62, /* impleWeb */
0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, 0x28, /* Server ( */
0x32, 0x29, 0x05, 0x5f, 0x68, 0x74, 0x74, 0x70, /* 2)._http */
0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, 0x6c, 0x6f, /* ._tcp.lo */
0x63, 0x61, 0x6c, 0x00, 0x00, 0xff, 0x00, 0x01, /* cal..... */
0x13, 0x53, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x57, /* .SimpleW */
0x65, 0x62, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, /* ebServer */
0x20, 0x28, 0x32, 0x29, 0x05, 0x5f, 0x68, 0x74, /*  (2)._ht */
0x74, 0x70, 0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, /* tp._tcp. */
0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x00, 0x00, 0x21, /* local..! */
0x80, 0x01, 0x00, 0x00, 0x00, 0x64, 0x00, 0x19, /* .....d.. */
0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x0b, 0x41, /* .....P.A */
0x52, 0x4d, 0x4d, 0x44, 0x4e, 0x53, 0x54, 0x65, /* RMMDNSTe */
0x73, 0x74, 0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, /* st.local */
0x00, 0x13, 0x53, 0x69, 0x6d, 0x70, 0x6c, 0x65, /* ..Simple */
0x57, 0x65, 0x62, 0x53, 0x65, 0x72, 0x76, 0x65, /* WebServe */
0x72, 0x20, 0x28, 0x32, 0x29, 0x05, 0x5f, 0x68, /* r (2)._h */
0x74, 0x74, 0x70, 0x04, 0x5f, 0x74, 0x63, 0x70, /* ttp._tcp */
0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x00, 0x00, /* .local.. */
0x10, 0x80, 0x01, 0x00, 0x00, 0x00, 0x64, 0x00, /* ......d. */
0x14, 0x08, 0x70, 0x61, 0x70, 0x65, 0x72, 0x3d, /* ..paper= */
0x41, 0x34, 0x0a, 0x76, 0x65, 0x72, 0x73, 0x69, /* A4.versi */
0x6f, 0x6e, 0x3d, 0x30, 0x31                    /* on=01 */
};

/* Frame (261 bytes) */
static const unsigned char pkt6[261] = {
0x01, 0x00, 0x5e, 0x00, 0x00, 0xfb, 0x00, 0x11, /* ..^..... */
0x22, 0x33, 0x44, 0x56, 0x08, 0x00, 0x45, 0x00, /* "3DV..E. */
0x00, 0xf7, 0x00, 0x0b, 0x40, 0x00, 0xff, 0x11, /* ....@... */
0x8f, 0xad, 0x0a, 0x00, 0x00, 0x42, 0xe0, 0x00, /* .....B.. */
0x00, 0xfb, 0x14, 0xe9, 0x14, 0xe9, 0x00, 0xe3, /* ........ */
0xf7, 0x54, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00, /* .T...... */
0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x13, 0x53, /* .......S */
0x69, 0x6d, 0x70, 0x6c, 0x65, 0x57, 0x65, 0x62, /* impleWeb */
0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, 0x28, /* Server ( */
0x32, 0x29, 0x05, 0x5f, 0x68, 0x74, 0x74, 0x70, /* 2)._http */
0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, 0x6c, 0x6f, /* ._tcp.lo */
0x63, 0x61, 0x6c, 0x00, 0x00, 0x21, 0x80, 0x01, /* cal..!.. */
0x00, 0x00, 0x00, 0x64, 0x00, 0x19, 0x00, 0x00, /* ...d.... */
0x00, 0x00, 0x00, 0x50, 0x0b, 0x41, 0x52, 0x4d, /* ...P.ARM */
0x4d, 0x44, 0x4e, 0x53, 0x54, 0x65, 0x73, 0x74, /* MDNSTest */
0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x00, 0x13, /* .local.. */
0x53, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x57, 0x65, /* SimpleWe */
0x62, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, /* bServer  */
0x28, 0x32, 0x29, 0x05, 0x5f, 0x68, 0x74, 0x74, /* (2)._htt */
0x70, 0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, 0x6c, /* p._tcp.l */
0x6f, 0x63, 0x61, 0x6c, 0x00, 0x00, 0x10, 0x80, /* ocal.... */
0x01, 0x00, 0x00, 0x00, 0x64, 0x00, 0x14, 0x08, /* ....d... */
0x70, 0x61, 0x70, 0x65, 0x72, 0x3d, 0x41, 0x34, /* paper=A4 */
0x0a, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, /* .version */
0x3d, 0x30, 0x31, 0x05, 0x5f, 0x68, 0x74, 0x74, /* =01._htt */
0x70, 0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, 0x6c, /* p._tcp.l */
0x6f, 0x63, 0x61, 0x6c, 0x00, 0x00, 0x0c, 0x00, /* ocal.... */
0x01, 0x00, 0x00, 0x00, 0x64, 0x00, 0x26, 0x13, /* ....d.&. */
0x53, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x57, 0x65, /* SimpleWe */
0x62, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, /* bServer  */
0x28, 0x32, 0x29, 0x05, 0x5f, 0x68, 0x74, 0x74, /* (2)._htt */
0x70, 0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, 0x6c, /* p._tcp.l */
0x6f, 0x63, 0x61, 0x6c, 0x00                    /* ocal. */
};

/* Frame (261 bytes) */
static const unsigned char pkt7[261] = {
0x01, 0x00, 0x5e, 0x00, 0x00, 0xfb, 0x00, 0x11, /* ..^..... */
0x22, 0x33, 0x44, 0x56, 0x08, 0x00, 0x45, 0x00, /* "3DV..E. */
0x00, 0xf7, 0x00, 0x0c, 0x40, 0x00, 0xff, 0x11, /* ....@... */
0x8f, 0xac, 0x0a, 0x00, 0x00, 0x42, 0xe0, 0x00, /* .....B.. */
0x00, 0xfb, 0x14, 0xe9, 0x14, 0xe9, 0x00, 0xe3, /* ........ */
0xf7, 0x54, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00, /* .T...... */
0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x13, 0x53, /* .......S */
0x69, 0x6d, 0x70, 0x6c, 0x65, 0x57, 0x65, 0x62, /* impleWeb */
0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, 0x28, /* Server ( */
0x32, 0x29, 0x05, 0x5f, 0x68, 0x74, 0x74, 0x70, /* 2)._http */
0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, 0x6c, 0x6f, /* ._tcp.lo */
0x63, 0x61, 0x6c, 0x00, 0x00, 0x21, 0x80, 0x01, /* cal..!.. */
0x00, 0x00, 0x00, 0x64, 0x00, 0x19, 0x00, 0x00, /* ...d.... */
0x00, 0x00, 0x00, 0x50, 0x0b, 0x41, 0x52, 0x4d, /* ...P.ARM */
0x4d, 0x44, 0x4e, 0x53, 0x54, 0x65, 0x73, 0x74, /* MDNSTest */
0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x00, 0x13, /* .local.. */
0x53, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x57, 0x65, /* SimpleWe */
0x62, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, /* bServer  */
0x28, 0x32, 0x29, 0x05, 0x5f, 0x68, 0x74, 0x74, /* (2)._htt */
0x70, 0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, 0x6c, /* p._tcp.l */
0x6f, 0x63, 0x61, 0x6c, 0x00, 0x00, 0x10, 0x80, /* ocal.... */
0x01, 0x00, 0x00, 0x00, 0x64, 0x00, 0x14, 0x08, /* ....d... */
0x70, 0x61, 0x70, 0x65, 0x72, 0x3d, 0x41, 0x34, /* paper=A4 */
0x0a, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, /* .version */
0x3d, 0x30, 0x31, 0x05, 0x5f, 0x68, 0x74, 0x74, /* =01._htt */
0x70, 0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, 0x6c, /* p._tcp.l */
0x6f, 0x63, 0x61, 0x6c, 0x00, 0x00, 0x0c, 0x00, /* ocal.... */
0x01, 0x00, 0x00, 0x00, 0x64, 0x00, 0x26, 0x13, /* ....d.&. */
0x53, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x57, 0x65, /* SimpleWe */
0x62, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, /* bServer  */
0x28, 0x32, 0x29, 0x05, 0x5f, 0x68, 0x74, 0x74, /* (2)._htt */
0x70, 0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, 0x6c, /* p._tcp.l */
0x6f, 0x63, 0x61, 0x6c, 0x00                    /* ocal. */
};

/* Frame (261 bytes) */
static const unsigned char pkt8[261] = {
0x01, 0x00, 0x5e, 0x00, 0x00, 0xfb, 0x00, 0x11, /* ..^..... */
0x22, 0x33, 0x44, 0x56, 0x08, 0x00, 0x45, 0x00, /* "3DV..E. */
0x00, 0xf7, 0x00, 0x0d, 0x40, 0x00, 0xff, 0x11, /* ....@... */
0x8f, 0xab, 0x0a, 0x00, 0x00, 0x42, 0xe0, 0x00, /* .....B.. */
0x00, 0xfb, 0x14, 0xe9, 0x14, 0xe9, 0x00, 0xe3, /* ........ */
0xf7, 0x54, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00, /* .T...... */
0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x13, 0x53, /* .......S */
0x69, 0x6d, 0x70, 0x6c, 0x65, 0x57, 0x65, 0x62, /* impleWeb */
0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, 0x28, /* Server ( */
0x32, 0x29, 0x05, 0x5f, 0x68, 0x74, 0x74, 0x70, /* 2)._http */
0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, 0x6c, 0x6f, /* ._tcp.lo */
0x63, 0x61, 0x6c, 0x00, 0x00, 0x21, 0x80, 0x01, /* cal..!.. */
0x00, 0x00, 0x00, 0x64, 0x00, 0x19, 0x00, 0x00, /* ...d.... */
0x00, 0x00, 0x00, 0x50, 0x0b, 0x41, 0x52, 0x4d, /* ...P.ARM */
0x4d, 0x44, 0x4e, 0x53, 0x54, 0x65, 0x73, 0x74, /* MDNSTest */
0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x00, 0x13, /* .local.. */
0x53, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x57, 0x65, /* SimpleWe */
0x62, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, /* bServer  */
0x28, 0x32, 0x29, 0x05, 0x5f, 0x68, 0x74, 0x74, /* (2)._htt */
0x70, 0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, 0x6c, /* p._tcp.l */
0x6f, 0x63, 0x61, 0x6c, 0x00, 0x00, 0x10, 0x80, /* ocal.... */
0x01, 0x00, 0x00, 0x00, 0x64, 0x00, 0x14, 0x08, /* ....d... */
0x70, 0x61, 0x70, 0x65, 0x72, 0x3d, 0x41, 0x34, /* paper=A4 */
0x0a, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, /* .version */
0x3d, 0x30, 0x31, 0x05, 0x5f, 0x68, 0x74, 0x74, /* =01._htt */
0x70, 0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, 0x6c, /* p._tcp.l */
0x6f, 0x63, 0x61, 0x6c, 0x00, 0x00, 0x0c, 0x00, /* ocal.... */
0x01, 0x00, 0x00, 0x00, 0x64, 0x00, 0x26, 0x13, /* ....d.&. */
0x53, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x57, 0x65, /* SimpleWe */
0x62, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, /* bServer  */
0x28, 0x32, 0x29, 0x05, 0x5f, 0x68, 0x74, 0x74, /* (2)._htt */
0x70, 0x04, 0x5f, 0x74, 0x63, 0x70, 0x05, 0x6c, /* p._tcp.l */
0x6f, 0x63, 0x61, 0x6c, 0x00                    /* ocal. */
};


static MDNS_SERVICE mdns_service = {"SimpleWebServer", "_http._tcp", NX_NULL, "paper=A4;version=01", 100, 0, 0, 80, NX_MDNS_RR_SET_UNIQUE, 0};

MDNS_TEST_SEQ mdns_probing_conflict[] = {
    {TITLE, "Probing conflict", 16, 0},
    
    /* Wait 5 seconds. */
    {WAIT, NX_NULL, 0, 5},

    {MDNS_SERVICE_ADD, (char*)&mdns_service, 0, 0},

    /* Wait the first probing. */
    {MDNS_CHECK_DATA_V4, (char*)&pkt1[0], sizeof(pkt1), 1},

    /* Inject response. */
    {INJECT, (char*)&pkt2[0], sizeof(pkt2), 0},

    /* Expect probing with other name. */
    {MDNS_CHECK_DATA_V4, (char*)&pkt3[0], sizeof(pkt3), 1},

    /* Set callback state. */
    {MDNS_SET_PROBING_CALLBACK_STATE, NX_NULL, NX_MDNS_LOCAL_SERVICE_REGISTERED_SUCCESS, 0},

    /* Wait 3 seconds. */
    {WAIT, NX_NULL, 0, 3},

    /* Check callback invoked. */
    {MDNS_CHECK_PROBING_CALLBACK_INVOKED, NX_NULL, 0, 1},
};

int mdns_probing_conflict_size = sizeof(mdns_probing_conflict) / sizeof(MDNS_TEST_SEQ);

#endif /* __PRODUCT_NETXDUO__  */

