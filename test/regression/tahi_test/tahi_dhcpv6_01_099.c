#include "nx_api.h"
#if defined(NX_TAHI_ENABLE) && defined(FEATURE_NX_IPV6)  
#include "netx_tahi.h"

#if 0
/* Frame (110 bytes) */
static char pkt1[110] = {
    0x33, 0x33, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, /* 33...... */
    0x00, 0x00, 0xa6, 0xa6, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
    0x00, 0x00, 0x00, 0x38, 0x3a, 0xff, 0xfe, 0x80, /* ...8:... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
    0x00, 0xff, 0xfe, 0x00, 0xa6, 0xa6, 0xff, 0x02, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0x00, /* ........ */
    0x42, 0x9e, 0x00, 0x80, 0x0b, 0xb8, 0x00, 0x00, /* B....... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0xa6, 0xa6, 0x03, 0x04, /* ........ */
    0x40, 0x80, 0x00, 0x00, 0x38, 0x40, 0x00, 0x00, /* @...8@.. */
    0x1c, 0x20, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* . ....?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00              /* ...... */
};

/* Frame (70 bytes) */
static char pkt2[70] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
    0x00, 0x00, 0xa1, 0xa1, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
    0x00, 0x00, 0x00, 0x10, 0x3a, 0x40, 0x3f, 0xfe, /* ....:@?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
    0x00, 0xff, 0xfe, 0x00, 0xa1, 0xa1, 0x3f, 0xfe, /* ......?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x80, 0x00, /* "..3DV.. */
    0xe9, 0x7a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* .z...... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00              /* ...... */
};
#endif

/* Frame (118 bytes) */
static char pkt3[118] = {
    0x33, 0x33, 0x00, 0x01, 0x00, 0x02, 0x00, 0x11, /* 33...... */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x40, 0x11, 0xff, 0xfe, 0x80, /* ...@.... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x02, 0x22, /* ......." */
    0x02, 0x23, 0x00, 0x40, 0x6d, 0x29, 0x01, 0x3e, /* .#.@m).> */
    0xf0, 0x12, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, /* ........ */
    0x00, 0x01, 0xac, 0x7d, 0x87, 0x3a, 0x00, 0x11, /* ...}.:.. */
    0x22, 0x33, 0x44, 0x56, 0x00, 0x03, 0x00, 0x0c, /* "3DV.... */
    0xc0, 0xde, 0xdb, 0xad, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, /* ........ */
    0x00, 0x00, 0x00, 0x06, 0x00, 0x08, 0x00, 0x17, /* ........ */
    0x00, 0x1f, 0x00, 0x29, 0x00, 0x18              /* ...).. */
    };

/* Frame (146 bytes) */
static char pkt4[146] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
    0x00, 0x00, 0xa1, 0xa1, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
    0x00, 0x00, 0x00, 0x5c, 0x11, 0x40, 0xfe, 0x80, /* ...\.@.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
    0x00, 0xff, 0xfe, 0x00, 0xa1, 0xa1, 0xfe, 0x80, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x02, 0x22, /* "..3DV." */
    0x02, 0x22, 0x00, 0x5c, 0x1a, 0x77, 0x02, 0x3e, /* .".\.w.> */
    0xf0, 0x12, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, /* ........ */
    0x00, 0x01, 0xac, 0x7d, 0x87, 0x3a, 0x00, 0x11, /* ...}.:.. */
    0x22, 0x33, 0x44, 0x56, 0x00, 0x02, 0x00, 0x0e, /* "3DV.... */
    0x00, 0x01, 0x00, 0x01, 0x00, 0x06, 0x1a, 0x80, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0xa1, 0xa1, 0x00, 0x03, /* ........ */
    0x00, 0x28, 0xc0, 0xde, 0xdb, 0xad, 0x00, 0x00, /* .(...... */
    0x00, 0x32, 0x00, 0x00, 0x00, 0x50, 0x00, 0x05, /* .2...P.. */
    0x00, 0x18, 0x3f, 0xfe, 0x05, 0x01, 0xff, 0xff, /* ..?..... */
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0xab, 0xcd, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, /* .....d.. */
    0x00, 0xc8                                      /* .. */
    };

/* Frame (164 bytes) */
static char pkt5[164] = {
    0x33, 0x33, 0x00, 0x01, 0x00, 0x02, 0x00, 0x11, /* 33...... */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x6e, 0x11, 0xff, 0xfe, 0x80, /* ...n.... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x02, 0x22, /* ......." */
    0x02, 0x23, 0x00, 0x6e, 0x9c, 0x98, 0x03, 0x08, /* .#.n.... */
    0x0e, 0x90, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, /* ........ */
    0x00, 0x01, 0xac, 0x7d, 0x87, 0x3a, 0x00, 0x11, /* ...}.:.. */
    0x22, 0x33, 0x44, 0x56, 0x00, 0x08, 0x00, 0x02, /* "3DV.... */
    0x00, 0x00, 0x00, 0x02, 0x00, 0x0e, 0x00, 0x01, /* ........ */
    0x00, 0x01, 0x00, 0x06, 0x1a, 0x80, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0xa1, 0xa1, 0x00, 0x03, 0x00, 0x28, /* .......( */
    0xc0, 0xde, 0xdb, 0xad, 0x00, 0x00, 0x00, 0x32, /* .......2 */
    0x00, 0x00, 0x00, 0x50, 0x00, 0x05, 0x00, 0x18, /* ...P.... */
    0x3f, 0xfe, 0x05, 0x01, 0xff, 0xff, 0x01, 0x00, /* ?....... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xab, 0xcd, /* ........ */
    0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0xc8, /* ...d.... */
    0x00, 0x06, 0x00, 0x08, 0x00, 0x17, 0x00, 0x1f, /* ........ */
    0x00, 0x29, 0x00, 0x18                          /* .).. */
    };

/* Frame (154 bytes) */
static char pkt6[154] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
    0x00, 0x00, 0xa1, 0xa1, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
    0x00, 0x00, 0x00, 0x64, 0x11, 0x40, 0xfe, 0x80, /* ...d.@.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
    0x00, 0xff, 0xfe, 0x00, 0xa1, 0xa1, 0xfe, 0x80, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x02, 0x23, /* "..3DV.# */
    0x02, 0x22, 0x00, 0x64, 0xf7, 0x0d, 0x07, 0x08, /* .".d.... */
    0x0e, 0x90, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, /* ........ */
    0x00, 0x01, 0xac, 0x7d, 0x87, 0x3a, 0x00, 0x11, /* ...}.:.. */
    0x22, 0x33, 0x44, 0x56, 0x00, 0x02, 0x00, 0x0e, /* "3DV.... */
    0x00, 0x01, 0x00, 0x01, 0x00, 0x06, 0x1a, 0x80, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0xa1, 0xa1, 0x00, 0x03, /* ........ */
    0x00, 0x28, 0xc0, 0xde, 0xdb, 0xad, 0x00, 0x00, /* .(...... */
    0x00, 0x32, 0x00, 0x00, 0x00, 0x50, 0x00, 0x05, /* .2...P.. */
    0x00, 0x18, 0x3f, 0xfe, 0x05, 0x01, 0xff, 0xff, /* ..?..... */
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0xab, 0xcd, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, /* .....d.. */
    0x00, 0xc8, 0x00, 0x0d, 0x00, 0x04, 0x00, 0x00, /* ........ */
    0x00, 0x00                                      /* .. */
    };

/* Frame (78 bytes) */
static char pkt7[78] = {
    0x33, 0x33, 0xff, 0x00, 0xab, 0xcd, 0x00, 0x11, /* 33...... */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x18, 0x3a, 0xff, 0x00, 0x00, /* ....:... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x02, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x01, 0xff, 0x00, 0xab, 0xcd, 0x87, 0x00, /* ........ */
    0xdd, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* ......?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0xab, 0xcd              /* ...... */
};

/* Frame (78 bytes) */
static char pkt8[78] = {
    0x33, 0x33, 0xff, 0x00, 0xab, 0xcd, 0x00, 0x11, /* 33...... */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x18, 0x3a, 0xff, 0x00, 0x00, /* ....:... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x02, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x01, 0xff, 0x00, 0xab, 0xcd, 0x87, 0x00, /* ........ */
    0xdd, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* ......?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0xab, 0xcd              /* ...... */
};

/* Frame (78 bytes) */
static char pkt9[78] = {
    0x33, 0x33, 0xff, 0x00, 0xab, 0xcd, 0x00, 0x11, /* 33...... */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x18, 0x3a, 0xff, 0x00, 0x00, /* ....:... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x02, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x01, 0xff, 0x00, 0xab, 0xcd, 0x87, 0x00, /* ........ */
    0xdd, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* ......?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0xab, 0xcd              /* ...... */
};

/* Frame (116 bytes) */
static char pkt10[116] = {
    0x33, 0x33, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, /* 33...... */
    0x00, 0x00, 0xa2, 0xa2, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
    0x00, 0x00, 0x00, 0x3e, 0x11, 0x01, 0xfe, 0x80, /* ...>.... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
    0x00, 0xff, 0xfe, 0x00, 0xa2, 0xa2, 0xff, 0x02, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x02, 0x22, /* ......." */
    0x02, 0x22, 0x00, 0x3e, 0x5b, 0x9d, 0x0b, 0x00, /* .".>[... */
    0x00, 0x66, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, /* .f...... */
    0x00, 0x01, 0x00, 0x04, 0x93, 0xe0, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0xa2, 0xa2, 0x00, 0x02, 0x00, 0x0e, /* ........ */
    0x00, 0x01, 0x00, 0x01, 0x00, 0x06, 0x1a, 0x80, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0xa1, 0xa1, 0x00, 0x06, /* ........ */
    0x00, 0x02, 0x00, 0x17, 0x00, 0x0d, 0x00, 0x04, /* ........ */
    0x00, 0x00, 0x00, 0x00                          /* .... */
};




TAHI_TEST_SEQ tahi_dhcpv6_01_099[] = {
    {TITLE, "dhcpv6 01-099", 13, 0},

    {CHECK,  &pkt3[0], sizeof(pkt3), 30},
    {INJECT, &pkt4[0], sizeof(pkt4), 0},
    {CHECK,  &pkt5[0], sizeof(pkt5), 5},
    {INJECT, &pkt6[0], sizeof(pkt6), 0},

    {CHECK, &pkt7[0], sizeof(pkt7), 5},
    {CHECK, &pkt8[0], sizeof(pkt8), 5},
    {CHECK, &pkt9[0], sizeof(pkt9), 5},

    {WAIT,   NX_NULL,  0, 5}, /* Wait for the valid address state. */

    {INJECT, &pkt10[0], sizeof(pkt10), 0},

    {N_CHECK, (CHAR *)ANY, 0, 10},



    {CLEANUP, NX_NULL, 0, 0},
    {DUMP, NX_NULL, 0, 0}
};

int  tahi_dhcpv6_01_099_size = sizeof(tahi_dhcpv6_01_099) / sizeof(TAHI_TEST_SEQ);
#endif
