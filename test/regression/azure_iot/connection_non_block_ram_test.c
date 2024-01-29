/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
 * 
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>  /* macros: https://api.cmocka.org/group__cmocka__asserts.html */

#include "nx_api.h"
#include "nx_azure_iot_hub_client.h"
#include "nx_azure_iot_cert.h"
#include "nx_azure_iot_ciphersuites.h"


#define DEMO_DHCP_DISABLE
#define DEMO_IPV4_ADDRESS         IP_ADDRESS(192, 168, 100, 33)
#define DEMO_IPV4_MASK            0xFFFFFF00UL
#define DEMO_GATEWAY_ADDRESS      IP_ADDRESS(192, 168, 100, 1)
#define DEMO_DNS_SERVER_ADDRESS   IP_ADDRESS(192, 168, 100, 1)
#define NETWORK_DRIVER            _nx_ram_network_driver

/* Include main.c in the test case since we need to disable DHCP in this test. */
#include "main.c"


#define STRING_UNSIGNED_ARGS(s) (UCHAR *)s, strlen(s)

#ifndef DEMO_CLOUD_STACK_SIZE
#define DEMO_CLOUD_STACK_SIZE   2048
#endif /* DEMO_CLOUD_STACK_SIZE */

#ifndef DEMO_CLOUD_THREAD_PRIORITY
#define DEMO_CLOUD_THREAD_PRIORITY  (3)
#endif /* DEMO_CLOUD_THREAD_PRIORITY */

static void mqtt_server_init(NX_IP *ip_ptr);
static void mqtt_server_accept(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr);
static void mqtt_server_deinit(NX_IP *ip_ptr);
void connection_status_cb(struct NX_AZURE_IOT_HUB_CLIENT_STRUCT *hub_client_ptr, UINT status);

static NX_AZURE_IOT iot;
static NX_AZURE_IOT_HUB_CLIENT iot_client;
static NX_SECURE_X509_CERT root_ca_cert;
static UCHAR metadata_buffer[NX_AZURE_IOT_TLS_METADATA_BUFFER_SIZE];
static ULONG demo_cloud_thread_stack[DEMO_CLOUD_STACK_SIZE / sizeof(ULONG)];

static UINT connection_status = NX_AZURE_IOT_HUB_CLIENT_STATUS_NOT_CONNECTED;

/* Define server side data. */
static NX_SECURE_TLS_SESSION tls_server_session;
static NX_SECURE_X509_CERT server_certificate;
static NX_TCP_SOCKET server_socket;
static UCHAR server_metadata_buffer[16 * 1024];
static UCHAR server_tls_packet_buffer[4096];

static unsigned char server_cert_der[] = {
  0x30, 0x82, 0x03, 0xd2, 0x30, 0x82, 0x02, 0xba, 0xa0, 0x03, 0x02, 0x01,
  0x02, 0x02, 0x01, 0x01, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86,
  0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x30, 0x7a, 0x31, 0x0b, 0x30,
  0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53, 0x31, 0x0b,
  0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0c, 0x02, 0x43, 0x41, 0x31,
  0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x07, 0x0c, 0x09, 0x53, 0x61,
  0x6e, 0x20, 0x44, 0x69, 0x65, 0x67, 0x6f, 0x31, 0x16, 0x30, 0x14, 0x06,
  0x03, 0x55, 0x04, 0x0a, 0x0c, 0x0d, 0x45, 0x78, 0x70, 0x72, 0x65, 0x73,
  0x73, 0x20, 0x4c, 0x6f, 0x67, 0x69, 0x63, 0x31, 0x14, 0x30, 0x12, 0x06,
  0x03, 0x55, 0x04, 0x0b, 0x0c, 0x0b, 0x4e, 0x65, 0x74, 0x58, 0x20, 0x53,
  0x65, 0x63, 0x75, 0x72, 0x65, 0x31, 0x1c, 0x30, 0x1a, 0x06, 0x03, 0x55,
  0x04, 0x03, 0x0c, 0x13, 0x4e, 0x65, 0x74, 0x58, 0x20, 0x53, 0x65, 0x63,
  0x75, 0x72, 0x65, 0x20, 0x54, 0x65, 0x73, 0x74, 0x20, 0x43, 0x41, 0x30,
  0x1e, 0x17, 0x0d, 0x31, 0x36, 0x31, 0x31, 0x31, 0x31, 0x31, 0x39, 0x35,
  0x31, 0x30, 0x30, 0x5a, 0x17, 0x0d, 0x32, 0x36, 0x31, 0x31, 0x30, 0x39,
  0x31, 0x39, 0x35, 0x31, 0x30, 0x30, 0x5a, 0x30, 0x62, 0x31, 0x0b, 0x30,
  0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53, 0x31, 0x0b,
  0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0c, 0x02, 0x43, 0x41, 0x31,
  0x16, 0x30, 0x14, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x0d, 0x45, 0x78,
  0x70, 0x72, 0x65, 0x73, 0x73, 0x20, 0x4c, 0x6f, 0x67, 0x69, 0x63, 0x31,
  0x14, 0x30, 0x12, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x0c, 0x0b, 0x4e, 0x65,
  0x74, 0x58, 0x20, 0x53, 0x65, 0x63, 0x75, 0x72, 0x65, 0x31, 0x18, 0x30,
  0x16, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x0f, 0x77, 0x77, 0x77, 0x2e,
  0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d, 0x30,
  0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7,
  0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00, 0x30,
  0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01, 0x00, 0xae, 0x03, 0x2c, 0xec,
  0xa2, 0x79, 0xd1, 0x15, 0x20, 0x88, 0x4d, 0xcd, 0xa2, 0x1b, 0x05, 0xe3,
  0xbd, 0x55, 0xad, 0xc6, 0x1f, 0x64, 0xe8, 0xb5, 0xc5, 0x0d, 0x67, 0xfc,
  0x7e, 0xda, 0xfb, 0x70, 0xf6, 0xc9, 0x47, 0x87, 0x3a, 0xaa, 0x88, 0x00,
  0xf1, 0xa7, 0xf7, 0xe1, 0xf5, 0x2c, 0x54, 0x0e, 0x33, 0xda, 0xbe, 0x9c,
  0x66, 0x30, 0xd9, 0x40, 0xeb, 0x1d, 0xce, 0xe1, 0x55, 0x15, 0x2b, 0x11,
  0x47, 0x6c, 0x7e, 0x88, 0xc6, 0x24, 0xcf, 0x87, 0x1b, 0xb5, 0x1f, 0x47,
  0xb9, 0xef, 0xad, 0x29, 0xd3, 0x2e, 0x43, 0xee, 0x39, 0xdd, 0x09, 0x54,
  0xba, 0xfc, 0xed, 0xbc, 0x2e, 0x0e, 0x53, 0x15, 0x37, 0xcb, 0xc5, 0xf5,
  0xee, 0x70, 0x2a, 0xe8, 0x01, 0x6d, 0xb1, 0x39, 0x94, 0x5a, 0xc2, 0x8a,
  0x00, 0x04, 0xa9, 0xff, 0xea, 0x56, 0xf7, 0xd7, 0xa8, 0x1b, 0xa4, 0x26,
  0xcd, 0x28, 0xaf, 0xfa, 0x52, 0x85, 0x1c, 0x26, 0x3e, 0x5e, 0x01, 0xf7,
  0xe1, 0x66, 0xff, 0xac, 0xad, 0x9c, 0x98, 0x2f, 0xe0, 0x7e, 0x9f, 0xf1,
  0x33, 0x31, 0xc3, 0x7f, 0xe6, 0x58, 0x5d, 0xd8, 0x5f, 0x7d, 0x2b, 0x5a,
  0x55, 0xcf, 0xb1, 0x91, 0x53, 0x41, 0x04, 0xac, 0x86, 0x5e, 0x01, 0x35,
  0x2b, 0x74, 0x8d, 0x46, 0x4d, 0x48, 0xc0, 0x5f, 0x83, 0x67, 0xb5, 0x6d,
  0x52, 0x3f, 0x3e, 0xe6, 0xec, 0xf8, 0x2e, 0x10, 0x28, 0xdb, 0x69, 0xa6,
  0x9d, 0x4b, 0xde, 0x19, 0x2e, 0xd2, 0x5f, 0xc8, 0xa9, 0x3b, 0x52, 0xe9,
  0xb2, 0xcd, 0x6e, 0x19, 0x22, 0xf9, 0x99, 0xa6, 0xcc, 0xf5, 0xd3, 0xec,
  0xff, 0x0c, 0x77, 0x6f, 0x25, 0x92, 0x07, 0x4c, 0x64, 0x7d, 0x34, 0x49,
  0x6f, 0xff, 0x0a, 0xa8, 0x15, 0x64, 0x72, 0x2d, 0x4f, 0x42, 0x05, 0xe8,
  0x2b, 0x01, 0xf1, 0xe3, 0x65, 0x94, 0x23, 0xd9, 0xdf, 0x5e, 0x3b, 0xb5,
  0x02, 0x03, 0x01, 0x00, 0x01, 0xa3, 0x7b, 0x30, 0x79, 0x30, 0x09, 0x06,
  0x03, 0x55, 0x1d, 0x13, 0x04, 0x02, 0x30, 0x00, 0x30, 0x2c, 0x06, 0x09,
  0x60, 0x86, 0x48, 0x01, 0x86, 0xf8, 0x42, 0x01, 0x0d, 0x04, 0x1f, 0x16,
  0x1d, 0x4f, 0x70, 0x65, 0x6e, 0x53, 0x53, 0x4c, 0x20, 0x47, 0x65, 0x6e,
  0x65, 0x72, 0x61, 0x74, 0x65, 0x64, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69,
  0x66, 0x69, 0x63, 0x61, 0x74, 0x65, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d,
  0x0e, 0x04, 0x16, 0x04, 0x14, 0x8d, 0xb0, 0xee, 0x8f, 0x6b, 0x43, 0x52,
  0x29, 0xf4, 0x25, 0xff, 0x3c, 0xda, 0x5f, 0xb3, 0xce, 0x9b, 0x7b, 0x75,
  0xe1, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16,
  0x80, 0x14, 0x1b, 0x8d, 0x06, 0xd9, 0x6b, 0xad, 0xee, 0x82, 0x24, 0x26,
  0x55, 0x9a, 0x1b, 0x03, 0x44, 0x92, 0x0a, 0x06, 0x92, 0x48, 0x30, 0x0d,
  0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05,
  0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 0x75, 0x83, 0x89, 0xab, 0x84, 0x52,
  0x5f, 0xa4, 0x9e, 0x98, 0xca, 0xa3, 0xf9, 0xab, 0xd4, 0x04, 0x32, 0xa4,
  0x8c, 0x96, 0x90, 0x39, 0x88, 0x92, 0xc3, 0xcd, 0x51, 0xc3, 0x01, 0x35,
  0x03, 0x78, 0xfa, 0x0d, 0x1e, 0x7b, 0x79, 0xe9, 0x7d, 0xd8, 0x68, 0x7a,
  0x65, 0xc6, 0x00, 0x7c, 0xa1, 0x7a, 0x52, 0xc9, 0xa3, 0xf4, 0x0b, 0xbd,
  0x76, 0x24, 0xdf, 0xde, 0x22, 0x2d, 0x95, 0xc5, 0xb6, 0x54, 0xb1, 0xac,
  0xb6, 0x9a, 0xe4, 0x68, 0x0f, 0x97, 0x4a, 0x44, 0xa2, 0x87, 0x01, 0x82,
  0xd4, 0x25, 0xbd, 0x01, 0xbc, 0x35, 0x8a, 0x6d, 0xb7, 0x7c, 0x48, 0xaa,
  0x92, 0xd7, 0x57, 0x76, 0x6a, 0xb0, 0xc9, 0x46, 0xa6, 0xbe, 0xbf, 0x0f,
  0xf0, 0xea, 0x62, 0x57, 0x71, 0x42, 0xf6, 0x67, 0xa7, 0xa1, 0x50, 0x87,
  0x14, 0x8e, 0x32, 0xd0, 0x5e, 0xc9, 0x7b, 0x79, 0x7e, 0xfa, 0x17, 0xc7,
  0xad, 0xbd, 0xc3, 0x98, 0x79, 0x45, 0xfb, 0x7f, 0xf7, 0xe6, 0x9f, 0x77,
  0xb3, 0x44, 0xc3, 0xaf, 0x6b, 0x61, 0x6a, 0x04, 0x68, 0x24, 0x2d, 0x31,
  0xf1, 0x28, 0x2c, 0xf4, 0xf0, 0x07, 0xfe, 0xfd, 0x66, 0x98, 0x77, 0x37,
  0x7b, 0x80, 0x1f, 0xb2, 0x49, 0xe4, 0xa6, 0x24, 0x72, 0x42, 0xf4, 0xca,
  0x91, 0x80, 0xa1, 0xb2, 0x0a, 0xc9, 0xc0, 0x93, 0xa7, 0x22, 0x0b, 0x13,
  0x8a, 0xb2, 0x75, 0x4b, 0x66, 0xf9, 0x87, 0x3a, 0x51, 0x97, 0xc7, 0x1e,
  0x2b, 0x61, 0x81, 0x5c, 0xf0, 0xf8, 0x4c, 0xdb, 0x36, 0xc7, 0xba, 0x49,
  0xd9, 0x04, 0x6a, 0x95, 0xb0, 0x7f, 0xfc, 0xce, 0xca, 0x23, 0xad, 0xf9,
  0xaf, 0x8a, 0x72, 0x8e, 0xab, 0xb8, 0x8b, 0x7e, 0xf7, 0x39, 0xa6, 0x22,
  0x56, 0x03, 0x72, 0x06, 0xc3, 0x57, 0x1f, 0x32, 0xaa, 0xb5, 0xa6, 0x00,
  0x67, 0x88, 0x4b, 0x40, 0xe9, 0x5e, 0x4a, 0x6f, 0x76, 0xe8
};

static unsigned char server_cert_key_der[] = {
  0x30, 0x82, 0x04, 0xa4, 0x02, 0x01, 0x00, 0x02, 0x82, 0x01, 0x01, 0x00,
  0xae, 0x03, 0x2c, 0xec, 0xa2, 0x79, 0xd1, 0x15, 0x20, 0x88, 0x4d, 0xcd,
  0xa2, 0x1b, 0x05, 0xe3, 0xbd, 0x55, 0xad, 0xc6, 0x1f, 0x64, 0xe8, 0xb5,
  0xc5, 0x0d, 0x67, 0xfc, 0x7e, 0xda, 0xfb, 0x70, 0xf6, 0xc9, 0x47, 0x87,
  0x3a, 0xaa, 0x88, 0x00, 0xf1, 0xa7, 0xf7, 0xe1, 0xf5, 0x2c, 0x54, 0x0e,
  0x33, 0xda, 0xbe, 0x9c, 0x66, 0x30, 0xd9, 0x40, 0xeb, 0x1d, 0xce, 0xe1,
  0x55, 0x15, 0x2b, 0x11, 0x47, 0x6c, 0x7e, 0x88, 0xc6, 0x24, 0xcf, 0x87,
  0x1b, 0xb5, 0x1f, 0x47, 0xb9, 0xef, 0xad, 0x29, 0xd3, 0x2e, 0x43, 0xee,
  0x39, 0xdd, 0x09, 0x54, 0xba, 0xfc, 0xed, 0xbc, 0x2e, 0x0e, 0x53, 0x15,
  0x37, 0xcb, 0xc5, 0xf5, 0xee, 0x70, 0x2a, 0xe8, 0x01, 0x6d, 0xb1, 0x39,
  0x94, 0x5a, 0xc2, 0x8a, 0x00, 0x04, 0xa9, 0xff, 0xea, 0x56, 0xf7, 0xd7,
  0xa8, 0x1b, 0xa4, 0x26, 0xcd, 0x28, 0xaf, 0xfa, 0x52, 0x85, 0x1c, 0x26,
  0x3e, 0x5e, 0x01, 0xf7, 0xe1, 0x66, 0xff, 0xac, 0xad, 0x9c, 0x98, 0x2f,
  0xe0, 0x7e, 0x9f, 0xf1, 0x33, 0x31, 0xc3, 0x7f, 0xe6, 0x58, 0x5d, 0xd8,
  0x5f, 0x7d, 0x2b, 0x5a, 0x55, 0xcf, 0xb1, 0x91, 0x53, 0x41, 0x04, 0xac,
  0x86, 0x5e, 0x01, 0x35, 0x2b, 0x74, 0x8d, 0x46, 0x4d, 0x48, 0xc0, 0x5f,
  0x83, 0x67, 0xb5, 0x6d, 0x52, 0x3f, 0x3e, 0xe6, 0xec, 0xf8, 0x2e, 0x10,
  0x28, 0xdb, 0x69, 0xa6, 0x9d, 0x4b, 0xde, 0x19, 0x2e, 0xd2, 0x5f, 0xc8,
  0xa9, 0x3b, 0x52, 0xe9, 0xb2, 0xcd, 0x6e, 0x19, 0x22, 0xf9, 0x99, 0xa6,
  0xcc, 0xf5, 0xd3, 0xec, 0xff, 0x0c, 0x77, 0x6f, 0x25, 0x92, 0x07, 0x4c,
  0x64, 0x7d, 0x34, 0x49, 0x6f, 0xff, 0x0a, 0xa8, 0x15, 0x64, 0x72, 0x2d,
  0x4f, 0x42, 0x05, 0xe8, 0x2b, 0x01, 0xf1, 0xe3, 0x65, 0x94, 0x23, 0xd9,
  0xdf, 0x5e, 0x3b, 0xb5, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02, 0x82, 0x01,
  0x01, 0x00, 0xa5, 0x22, 0x2c, 0x52, 0xd0, 0x09, 0x4c, 0x4a, 0x81, 0x59,
  0xf8, 0x83, 0xa9, 0x4f, 0x7d, 0xb2, 0x56, 0xad, 0xe5, 0x3f, 0xfb, 0xf0,
  0xf6, 0x09, 0xf1, 0x5b, 0x3c, 0x90, 0x58, 0x0e, 0x15, 0xc9, 0x68, 0xd9,
  0x30, 0x40, 0xfb, 0x82, 0x73, 0x98, 0x79, 0xbb, 0xcd, 0xb8, 0x27, 0xc3,
  0x8e, 0x6c, 0xff, 0xf6, 0x99, 0x26, 0xb0, 0xaf, 0xb0, 0xac, 0x33, 0xb3,
  0x50, 0xed, 0x73, 0xa1, 0xa8, 0x02, 0x38, 0xc6, 0x93, 0xf9, 0xd6, 0x17,
  0x7e, 0xbd, 0x97, 0xa4, 0xb5, 0x6f, 0x8a, 0xdb, 0x11, 0x78, 0x7c, 0x89,
  0x0e, 0x3c, 0x17, 0xbb, 0x54, 0x2c, 0x8d, 0x5a, 0x93, 0x7d, 0x1e, 0x33,
  0xc7, 0xd2, 0x7d, 0xe5, 0xaa, 0x12, 0x2d, 0xd9, 0x52, 0x4e, 0x63, 0x74,
  0xa6, 0x57, 0x9f, 0x1a, 0xd6, 0x3c, 0xc1, 0xb1, 0xab, 0x66, 0x4a, 0x0b,
  0x88, 0x1d, 0xa6, 0xd1, 0xbc, 0x60, 0x7a, 0x17, 0x1f, 0x8f, 0x9b, 0x35,
  0x57, 0xf8, 0xd0, 0x1c, 0xd3, 0xa6, 0x56, 0xc8, 0x03, 0x9c, 0x08, 0x3b,
  0x1b, 0x5b, 0xc2, 0x03, 0x3b, 0x3a, 0xa4, 0xe8, 0xed, 0x75, 0x66, 0xb0,
  0x85, 0x56, 0x40, 0xfe, 0xae, 0x97, 0x7e, 0xc0, 0x79, 0x49, 0x13, 0x8b,
  0x01, 0x0c, 0xae, 0x4c, 0x3d, 0x54, 0x47, 0xc5, 0x51, 0x40, 0x3d, 0xcc,
  0x4d, 0x17, 0xb3, 0x4e, 0x1d, 0x85, 0x1c, 0x41, 0x07, 0x03, 0x5e, 0xf9,
  0xfa, 0x17, 0x81, 0x24, 0x34, 0xaa, 0xbf, 0x67, 0x73, 0xb6, 0x9c, 0x67,
  0x36, 0xd9, 0xee, 0xf7, 0x86, 0x4c, 0x4d, 0x79, 0xca, 0xd7, 0xfd, 0x72,
  0xf9, 0xb3, 0x73, 0xc3, 0x57, 0xe5, 0x39, 0x72, 0x93, 0x56, 0xc2, 0xec,
  0xf8, 0x25, 0xe4, 0x8f, 0xba, 0xd0, 0x6f, 0x23, 0x8c, 0x39, 0x9e, 0x05,
  0x1a, 0x4e, 0xdc, 0x5e, 0xcd, 0x17, 0x59, 0x94, 0x37, 0x22, 0xb7, 0x39,
  0x50, 0x65, 0xdc, 0x91, 0x3c, 0xe1, 0x02, 0x81, 0x81, 0x00, 0xe4, 0xc6,
  0x42, 0xe5, 0xea, 0xe5, 0x32, 0xf3, 0x51, 0x36, 0x7b, 0x8c, 0x5b, 0x72,
  0x24, 0x1a, 0x4a, 0x44, 0x4f, 0x64, 0xe5, 0xa7, 0x74, 0xd9, 0xb2, 0x29,
  0x8a, 0x08, 0xcf, 0x9b, 0xd2, 0x9d, 0xc4, 0x20, 0x4c, 0xd3, 0x60, 0x4d,
  0xf7, 0xb7, 0xac, 0x92, 0x6b, 0x2b, 0x95, 0x73, 0x6e, 0x57, 0x00, 0x20,
  0x9d, 0xb2, 0xf6, 0xbd, 0x0b, 0xbb, 0xaa, 0x7e, 0x7e, 0x3e, 0x53, 0xfb,
  0x79, 0x7e, 0x45, 0xd5, 0x2e, 0xab, 0x5e, 0xff, 0x5c, 0x0a, 0x45, 0x2d,
  0x27, 0x19, 0xb0, 0x59, 0x0a, 0x39, 0x89, 0xf6, 0xae, 0xc6, 0xe2, 0xd1,
  0x07, 0x58, 0xbe, 0x95, 0x27, 0xaf, 0xf7, 0xa6, 0x2f, 0xaa, 0x37, 0x25,
  0x7c, 0x7b, 0xd3, 0xda, 0x13, 0x76, 0x0a, 0xb6, 0x6c, 0x99, 0x53, 0x5d,
  0xa5, 0x75, 0xfa, 0x10, 0x9b, 0x7f, 0xfe, 0xd7, 0xb4, 0x18, 0x95, 0xa8,
  0x65, 0x85, 0x07, 0xc5, 0xc4, 0xad, 0x02, 0x81, 0x81, 0x00, 0xc2, 0xb8,
  0x8e, 0xed, 0x9d, 0x4a, 0x1f, 0x9c, 0xda, 0x73, 0xf0, 0x2c, 0x35, 0x91,
  0xe4, 0x40, 0x78, 0xe1, 0x12, 0xf3, 0x08, 0xef, 0xdf, 0x97, 0xa0, 0xb0,
  0xdd, 0xea, 0xc2, 0xb9, 0x5b, 0xf8, 0xa1, 0xac, 0x32, 0xfd, 0xb8, 0xe9,
  0x0f, 0xed, 0xfd, 0xe0, 0xdc, 0x38, 0x90, 0x5e, 0xf5, 0x4c, 0x02, 0xc3,
  0x1a, 0x72, 0x18, 0xf7, 0xfe, 0xb7, 0xb8, 0x2a, 0xf8, 0x72, 0xbb, 0x99,
  0x56, 0xec, 0x85, 0x58, 0x31, 0x7e, 0x64, 0xdf, 0x02, 0x05, 0xe3, 0xb2,
  0xbb, 0xe2, 0x1b, 0xd6, 0x43, 0x73, 0xf8, 0x0f, 0xaf, 0x89, 0x57, 0x44,
  0x5f, 0x30, 0x1c, 0xe5, 0x78, 0xbf, 0x0b, 0xe7, 0x4b, 0xbe, 0x80, 0x2f,
  0x3d, 0x35, 0x44, 0xfc, 0x9e, 0x0d, 0x85, 0x5d, 0x94, 0x6e, 0xe9, 0x6a,
  0x72, 0xa7, 0x46, 0xd8, 0x64, 0x6c, 0xe9, 0x61, 0x92, 0xa0, 0xb6, 0xd1,
  0xee, 0xa6, 0xa6, 0xf4, 0x2c, 0x29, 0x02, 0x81, 0x81, 0x00, 0xb4, 0xa7,
  0x7b, 0x1c, 0x64, 0x29, 0x29, 0xda, 0xca, 0x3e, 0xe3, 0xc1, 0x2a, 0x55,
  0x2f, 0xfd, 0x32, 0xb8, 0x4e, 0x99, 0xb6, 0x60, 0x4d, 0xfd, 0xba, 0x9a,
  0xe2, 0xcd, 0xa2, 0x63, 0xc2, 0x25, 0xa3, 0x42, 0x7e, 0x68, 0x4c, 0x9c,
  0x45, 0x09, 0x5d, 0xd5, 0x21, 0x9c, 0x01, 0x20, 0x6d, 0xf9, 0x75, 0xb8,
  0x4b, 0xcf, 0x8e, 0xd8, 0x29, 0xf3, 0xbf, 0xe6, 0xb3, 0x7a, 0x34, 0x87,
  0x58, 0xa1, 0x46, 0x33, 0xd9, 0xee, 0xa9, 0xcd, 0xac, 0xb8, 0xcf, 0x77,
  0xa0, 0x70, 0xc0, 0xb9, 0x0f, 0x41, 0xf0, 0x98, 0x43, 0xdb, 0xfa, 0x30,
  0x66, 0x44, 0xc5, 0xfa, 0xb2, 0xa4, 0x5a, 0x43, 0x79, 0x50, 0x48, 0xcb,
  0xe9, 0x49, 0x3f, 0x39, 0xee, 0x34, 0x40, 0xb1, 0x5d, 0x80, 0x96, 0x3c,
  0x54, 0xf4, 0x9c, 0xcb, 0x90, 0x7f, 0xba, 0x96, 0x4b, 0x39, 0x3e, 0xb5,
  0x03, 0xb5, 0xd1, 0x35, 0x72, 0xe1, 0x02, 0x81, 0x80, 0x60, 0x14, 0xd5,
  0x61, 0xe6, 0x24, 0xf7, 0x28, 0x5c, 0x9a, 0xac, 0xbe, 0x03, 0xc8, 0xf3,
  0x49, 0xe4, 0xdb, 0x9a, 0x90, 0x15, 0xae, 0xd7, 0x33, 0x68, 0x75, 0x1d,
  0x6b, 0x83, 0x9e, 0x17, 0x05, 0xbe, 0x30, 0xcc, 0x10, 0x6a, 0x37, 0x86,
  0x46, 0xb6, 0xe9, 0x47, 0x81, 0x19, 0xab, 0xe1, 0x7a, 0x1a, 0x3a, 0xcf,
  0x47, 0xd1, 0x8e, 0x3d, 0x3f, 0xc6, 0x3e, 0x5d, 0xcd, 0xaf, 0x47, 0xe0,
  0x9e, 0x60, 0xc5, 0xbd, 0xd6, 0x52, 0x4b, 0xc0, 0x21, 0xcb, 0xd3, 0x1b,
  0xe6, 0x5c, 0x3a, 0x03, 0x9a, 0xab, 0xa2, 0x81, 0xc9, 0x51, 0x28, 0x49,
  0x97, 0xe2, 0x0a, 0x50, 0xe4, 0x64, 0x29, 0x43, 0x34, 0xc2, 0xe7, 0x8c,
  0x5a, 0x46, 0xaa, 0x28, 0x0b, 0x1f, 0xed, 0xa7, 0x1a, 0x7b, 0x4e, 0xad,
  0x38, 0x61, 0x3a, 0xd1, 0x82, 0xf4, 0x3d, 0xd3, 0x2e, 0x3e, 0x47, 0xa4,
  0x6c, 0xd3, 0x20, 0xd4, 0xd1, 0x02, 0x81, 0x80, 0x68, 0x1a, 0x8d, 0x3c,
  0x18, 0x3f, 0x42, 0x5e, 0x38, 0x6d, 0x0a, 0x1e, 0x52, 0xd5, 0x8f, 0xd6,
  0x32, 0xff, 0x7c, 0x1c, 0xf3, 0x20, 0x8b, 0x92, 0xa5, 0x44, 0xff, 0x08,
  0x21, 0xa1, 0xce, 0x68, 0x8b, 0x03, 0xe0, 0x90, 0xeb, 0x01, 0x4e, 0x85,
  0xf9, 0xc5, 0xb7, 0x86, 0xee, 0xd0, 0x59, 0x10, 0x73, 0x98, 0x2a, 0xcb,
  0xf6, 0xfe, 0x0d, 0xba, 0x07, 0x91, 0x18, 0xf6, 0xbc, 0x93, 0x8a, 0x91,
  0xdd, 0x80, 0x16, 0x37, 0xdf, 0x75, 0x46, 0x87, 0x68, 0xee, 0xf4, 0x76,
  0x0c, 0xc5, 0x87, 0x38, 0xf5, 0xb6, 0xda, 0x8a, 0xee, 0x62, 0xc8, 0xc0,
  0xa2, 0x8d, 0xbf, 0xd5, 0xf8, 0xba, 0xb5, 0x74, 0xf0, 0x07, 0xa6, 0x1c,
  0xcf, 0x76, 0x61, 0xbe, 0xa4, 0x88, 0x4a, 0x95, 0xb0, 0xa3, 0x70, 0x73,
  0xa1, 0x6f, 0x73, 0xf0, 0xe8, 0x38, 0x8d, 0xe8, 0xd0, 0x7e, 0x2c, 0x0c,
  0xdc, 0x21, 0xfa, 0xc1
};

static unsigned char ca_cert_der[] = {
  0x30, 0x82, 0x03, 0xc7, 0x30, 0x82, 0x02, 0xaf, 0xa0, 0x03, 0x02, 0x01,
  0x02, 0x02, 0x09, 0x00, 0xa1, 0x79, 0xb0, 0x6a, 0x32, 0xbc, 0x48, 0x67,
  0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01,
  0x0b, 0x05, 0x00, 0x30, 0x7a, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55,
  0x04, 0x06, 0x13, 0x02, 0x55, 0x53, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03,
  0x55, 0x04, 0x08, 0x0c, 0x02, 0x43, 0x41, 0x31, 0x12, 0x30, 0x10, 0x06,
  0x03, 0x55, 0x04, 0x07, 0x0c, 0x09, 0x53, 0x61, 0x6e, 0x20, 0x44, 0x69,
  0x65, 0x67, 0x6f, 0x31, 0x16, 0x30, 0x14, 0x06, 0x03, 0x55, 0x04, 0x0a,
  0x0c, 0x0d, 0x45, 0x78, 0x70, 0x72, 0x65, 0x73, 0x73, 0x20, 0x4c, 0x6f,
  0x67, 0x69, 0x63, 0x31, 0x14, 0x30, 0x12, 0x06, 0x03, 0x55, 0x04, 0x0b,
  0x0c, 0x0b, 0x4e, 0x65, 0x74, 0x58, 0x20, 0x53, 0x65, 0x63, 0x75, 0x72,
  0x65, 0x31, 0x1c, 0x30, 0x1a, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x13,
  0x4e, 0x65, 0x74, 0x58, 0x20, 0x53, 0x65, 0x63, 0x75, 0x72, 0x65, 0x20,
  0x54, 0x65, 0x73, 0x74, 0x20, 0x43, 0x41, 0x30, 0x1e, 0x17, 0x0d, 0x31,
  0x36, 0x31, 0x31, 0x31, 0x31, 0x31, 0x39, 0x35, 0x30, 0x30, 0x38, 0x5a,
  0x17, 0x0d, 0x32, 0x36, 0x31, 0x31, 0x30, 0x39, 0x31, 0x39, 0x35, 0x30,
  0x30, 0x38, 0x5a, 0x30, 0x7a, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55,
  0x04, 0x06, 0x13, 0x02, 0x55, 0x53, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03,
  0x55, 0x04, 0x08, 0x0c, 0x02, 0x43, 0x41, 0x31, 0x12, 0x30, 0x10, 0x06,
  0x03, 0x55, 0x04, 0x07, 0x0c, 0x09, 0x53, 0x61, 0x6e, 0x20, 0x44, 0x69,
  0x65, 0x67, 0x6f, 0x31, 0x16, 0x30, 0x14, 0x06, 0x03, 0x55, 0x04, 0x0a,
  0x0c, 0x0d, 0x45, 0x78, 0x70, 0x72, 0x65, 0x73, 0x73, 0x20, 0x4c, 0x6f,
  0x67, 0x69, 0x63, 0x31, 0x14, 0x30, 0x12, 0x06, 0x03, 0x55, 0x04, 0x0b,
  0x0c, 0x0b, 0x4e, 0x65, 0x74, 0x58, 0x20, 0x53, 0x65, 0x63, 0x75, 0x72,
  0x65, 0x31, 0x1c, 0x30, 0x1a, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x13,
  0x4e, 0x65, 0x74, 0x58, 0x20, 0x53, 0x65, 0x63, 0x75, 0x72, 0x65, 0x20,
  0x54, 0x65, 0x73, 0x74, 0x20, 0x43, 0x41, 0x30, 0x82, 0x01, 0x22, 0x30,
  0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01,
  0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00, 0x30, 0x82, 0x01, 0x0a, 0x02,
  0x82, 0x01, 0x01, 0x00, 0xd1, 0xdc, 0x3c, 0xe1, 0x1c, 0x7a, 0x3d, 0xb7,
  0x76, 0xcf, 0xab, 0xd7, 0x3c, 0x38, 0xb0, 0x81, 0xb6, 0x37, 0x52, 0xa3,
  0x3d, 0x6f, 0xcd, 0x89, 0xa6, 0xa2, 0xf3, 0xa8, 0xb0, 0x8d, 0xee, 0x0b,
  0x36, 0x94, 0x83, 0x0e, 0x7f, 0x39, 0x87, 0x6e, 0xee, 0x19, 0xe2, 0x1f,
  0x92, 0x3d, 0x01, 0x05, 0x4f, 0x11, 0xcd, 0xcb, 0xa0, 0x79, 0xfc, 0x9d,
  0x6e, 0x93, 0xb1, 0xb7, 0x03, 0xf3, 0xfe, 0xeb, 0x30, 0x67, 0x38, 0x85,
  0x28, 0xdf, 0x93, 0xdb, 0xcb, 0xcb, 0xb1, 0xbe, 0xd3, 0xe1, 0xc2, 0x7d,
  0x8d, 0xbb, 0x70, 0x76, 0x99, 0x08, 0x7c, 0x3f, 0x21, 0x2f, 0x37, 0x97,
  0xf7, 0xe8, 0x6e, 0x8c, 0x7e, 0xbc, 0x30, 0x5f, 0xbf, 0x32, 0x51, 0x1d,
  0x66, 0x76, 0xad, 0x39, 0xfc, 0x94, 0xd4, 0x65, 0xf6, 0xd2, 0x0b, 0x37,
  0xd3, 0x4a, 0xe6, 0xe1, 0xdf, 0x4a, 0x8f, 0x3b, 0x33, 0x16, 0xbe, 0xf7,
  0xd9, 0xbd, 0x73, 0x64, 0xdf, 0x34, 0xa3, 0x55, 0xe7, 0xac, 0xab, 0xa7,
  0xae, 0xc2, 0x20, 0x46, 0xc2, 0xd1, 0xe3, 0x25, 0x3a, 0x47, 0x68, 0x92,
  0xac, 0xd6, 0x12, 0xa4, 0x0a, 0xce, 0xdc, 0xe2, 0x24, 0x12, 0xee, 0xe1,
  0xb2, 0xcd, 0x09, 0xa8, 0xef, 0x36, 0xea, 0x76, 0xf9, 0xb6, 0x63, 0xaa,
  0xac, 0xdd, 0x46, 0x06, 0x6e, 0xd9, 0x1e, 0x08, 0xac, 0x57, 0x12, 0x6c,
  0x21, 0xef, 0x8e, 0xae, 0xf0, 0x27, 0xf1, 0x5c, 0x79, 0xb4, 0xb6, 0x26,
  0x92, 0x11, 0xda, 0xca, 0x80, 0x5e, 0x92, 0x4c, 0xb5, 0xd8, 0xb5, 0x84,
  0x95, 0xe3, 0xef, 0xbc, 0x7e, 0x7d, 0x68, 0x74, 0x4c, 0x34, 0x1a, 0x50,
  0x6d, 0x2d, 0x5f, 0x1b, 0x0e, 0xbe, 0xf5, 0xb4, 0xf1, 0x32, 0x16, 0x44,
  0x24, 0x7a, 0x0e, 0x4b, 0xcd, 0xfa, 0xa5, 0x03, 0x95, 0x2e, 0x44, 0x65,
  0xa8, 0x74, 0xea, 0x17, 0xdd, 0x99, 0xbd, 0xcb, 0x02, 0x03, 0x01, 0x00,
  0x01, 0xa3, 0x50, 0x30, 0x4e, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e,
  0x04, 0x16, 0x04, 0x14, 0x1b, 0x8d, 0x06, 0xd9, 0x6b, 0xad, 0xee, 0x82,
  0x24, 0x26, 0x55, 0x9a, 0x1b, 0x03, 0x44, 0x92, 0x0a, 0x06, 0x92, 0x48,
  0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80,
  0x14, 0x1b, 0x8d, 0x06, 0xd9, 0x6b, 0xad, 0xee, 0x82, 0x24, 0x26, 0x55,
  0x9a, 0x1b, 0x03, 0x44, 0x92, 0x0a, 0x06, 0x92, 0x48, 0x30, 0x0c, 0x06,
  0x03, 0x55, 0x1d, 0x13, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff, 0x30,
  0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b,
  0x05, 0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 0x70, 0xc7, 0x6a, 0x75, 0x27,
  0x14, 0xa0, 0x1c, 0xe0, 0xe0, 0x84, 0x7c, 0x6c, 0x16, 0xa9, 0x0d, 0x4d,
  0xb1, 0xc3, 0x87, 0x37, 0xf6, 0x86, 0x89, 0x6f, 0x73, 0xf0, 0x59, 0x9b,
  0x8c, 0xa4, 0x83, 0x10, 0x2d, 0xb7, 0x8b, 0xd0, 0x9a, 0x81, 0xe0, 0x5c,
  0xd7, 0x20, 0x6f, 0xdc, 0xfc, 0xc8, 0xa0, 0xc2, 0x8e, 0x54, 0xe6, 0xfb,
  0x61, 0x85, 0x37, 0x4b, 0x22, 0x47, 0x09, 0x95, 0x44, 0x12, 0x75, 0xf0,
  0xcf, 0x0b, 0x90, 0x48, 0xb0, 0x02, 0x4c, 0xef, 0x3f, 0xde, 0x6a, 0xfd,
  0xb1, 0x8b, 0x88, 0xd7, 0x84, 0xe5, 0x34, 0x02, 0x96, 0x0a, 0x3f, 0xa8,
  0x8c, 0xbd, 0x1a, 0xd8, 0xf7, 0xf9, 0xe5, 0x49, 0x87, 0xd0, 0x20, 0x4f,
  0xd8, 0xcd, 0xc0, 0xb9, 0x11, 0x2a, 0xd9, 0x0f, 0x75, 0xa6, 0xee, 0x76,
  0x15, 0x9f, 0x12, 0x50, 0x68, 0x4c, 0xc0, 0x05, 0x46, 0x8d, 0xdd, 0x93,
  0x74, 0x31, 0x82, 0x20, 0x37, 0x24, 0x58, 0xb2, 0x88, 0x9b, 0x21, 0xc1,
  0x48, 0xc4, 0x8d, 0x68, 0x3b, 0x91, 0x2c, 0x34, 0xcb, 0x94, 0xd0, 0xbc,
  0xe3, 0x05, 0x24, 0x05, 0xcc, 0xea, 0x05, 0xb1, 0x52, 0x74, 0x4a, 0x23,
  0x65, 0xc4, 0x40, 0x04, 0x86, 0xb1, 0x80, 0x61, 0x97, 0xdc, 0x94, 0x16,
  0x4e, 0x63, 0x31, 0x72, 0x4e, 0x45, 0xe8, 0x3e, 0x3b, 0xb6, 0x99, 0xae,
  0xd8, 0x91, 0x25, 0x3d, 0x62, 0x92, 0x6d, 0x72, 0x01, 0x2c, 0xca, 0x67,
  0x0a, 0xec, 0x00, 0xeb, 0x10, 0xff, 0x6d, 0xac, 0x89, 0x19, 0x2c, 0xb7,
  0xb3, 0xa5, 0xf7, 0xa1, 0x4a, 0xc3, 0xc1, 0xdd, 0xaf, 0xb5, 0x1a, 0x16,
  0x44, 0xdc, 0xa8, 0xb5, 0xca, 0xd0, 0x30, 0xaa, 0x7e, 0x73, 0xd5, 0x2e,
  0x65, 0xd6, 0xf9, 0xbf, 0x5f, 0xda, 0x6f, 0x13, 0xe9, 0xd7, 0x12, 0x6c,
  0x3a, 0x6c, 0x50, 0x26, 0x78, 0x6e, 0xc6, 0xeb, 0x75, 0xe1, 0x3c
};

static void mqtt_server_init(NX_IP *ip_ptr);

VOID demo_entry(NX_IP* ip_ptr, NX_PACKET_POOL* pool_ptr, NX_DNS* dns_ptr, UINT (*unix_time_callback)(ULONG *unix_time))
{
CHAR *host_name = "www.example.com";
CHAR *device_id = "device_id";
CHAR *module_id = "module_id";
CHAR *symmetric_key = "symmetric_key";
ULONG pool_ptr_available_packet;

    /* Setup an MQTT server first. */
    mqtt_server_init(ip_ptr);

    /* Initialize root certificate.  */
    assert_int_equal(nx_secure_x509_certificate_initialize(&root_ca_cert, (UCHAR *)ca_cert_der, sizeof(ca_cert_der),
                                                           NX_NULL, 0, NULL, 0, NX_SECURE_X509_KEY_TYPE_NONE),
                     NX_AZURE_IOT_SUCCESS);

    assert_int_equal(nx_azure_iot_create(&iot, (UCHAR *)"Azure IoT", ip_ptr, pool_ptr, dns_ptr, (UCHAR *)demo_cloud_thread_stack,
                                         sizeof(demo_cloud_thread_stack), DEMO_CLOUD_THREAD_PRIORITY, unix_time_callback),
                     NX_AZURE_IOT_SUCCESS);

    /* Record number of available packet before test */
    pool_ptr_available_packet = pool_ptr -> nx_packet_pool_available;

    assert_int_equal(nx_azure_iot_hub_client_initialize(&iot_client, &iot,
                                                        STRING_UNSIGNED_ARGS(host_name),
                                                        STRING_UNSIGNED_ARGS(device_id),
                                                        STRING_UNSIGNED_ARGS(""),
                                                        _nx_azure_iot_tls_supported_crypto,
                                                        _nx_azure_iot_tls_supported_crypto_size,
                                                        _nx_azure_iot_tls_ciphersuite_map,
                                                        _nx_azure_iot_tls_ciphersuite_map_size,
                                                        metadata_buffer, sizeof(metadata_buffer),
                                                        &root_ca_cert),
                     NX_AZURE_IOT_SUCCESS);

    assert_int_equal(nx_azure_iot_hub_client_connection_status_callback_set(&iot_client,
                                                                            connection_status_cb),
                     NX_SUCCESS);

    assert_int_equal(nx_azure_iot_hub_client_connect(&iot_client, NX_TRUE, NX_NO_WAIT),
                     NX_AZURE_IOT_CONNECTING);

    /* Establish MQTT connection. */
    mqtt_server_accept(ip_ptr, pool_ptr);

    /* Wait one second for connection. */
    tx_thread_sleep(NX_IP_PERIODIC_RATE);

    /* Check connection status. */
    assert_int_equal(connection_status, NX_AZURE_IOT_SUCCESS);

    /* Cleanup MQTT server. */
    mqtt_server_deinit(ip_ptr);

    assert_int_equal(nx_azure_iot_hub_client_disconnect(&iot_client),
                     NX_AZURE_IOT_SUCCESS);

    assert_int_equal(nx_azure_iot_hub_client_deinitialize(&iot_client),
                     NX_AZURE_IOT_SUCCESS);

    /* Check if all the packet are released */
    assert_int_equal(pool_ptr -> nx_packet_pool_available, pool_ptr_available_packet);

    /* SUCCESS: iot is created. */
    assert_int_equal(nx_azure_iot_delete(&iot), NX_AZURE_IOT_SUCCESS);
}


static void mqtt_server_init(NX_IP *ip_ptr)
{

    /* Create a socket.  */
    assert_int_equal(nx_tcp_socket_create(ip_ptr, &server_socket, "Server Socket",
                                          NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 1000,
                                          NX_NULL, NX_NULL),
                     NX_SUCCESS);

    /* Setup this thread to listen.  */
    assert_int_equal(nx_tcp_server_socket_listen(ip_ptr, NXD_MQTT_TLS_PORT,
                                                 &server_socket, 5, NX_NULL),
                     NX_SUCCESS);

    assert_int_equal(_nx_secure_tls_session_create_ext(&tls_server_session,
                                                       _nx_azure_iot_tls_supported_crypto,
                                                       _nx_azure_iot_tls_supported_crypto_size,
                                                       _nx_azure_iot_tls_ciphersuite_map,
                                                       _nx_azure_iot_tls_ciphersuite_map_size,
                                                       server_metadata_buffer,
                                                       sizeof(server_metadata_buffer)),
                     NX_SUCCESS);

    assert_int_equal(nx_secure_x509_certificate_initialize(&server_certificate,
                                                           server_cert_der, sizeof(server_cert_der),
                                                           NX_NULL, 0, server_cert_key_der,
                                                           sizeof(server_cert_key_der), NX_SECURE_X509_KEY_TYPE_RSA_PKCS1_DER),
                     NX_SUCCESS);

    assert_int_equal(nx_secure_tls_local_certificate_add(&tls_server_session, &server_certificate), NX_SUCCESS);

    assert_int_equal(nx_secure_tls_session_packet_buffer_set(&tls_server_session,
                                                             server_tls_packet_buffer,
                                                             sizeof(server_tls_packet_buffer)),
                     NX_SUCCESS);
}

static void mqtt_server_accept(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr)
{
NX_PACKET *packet_ptr;
UCHAR *byte;

    /* Accept a connection from client socket.  */
    assert_int_equal(nx_tcp_server_socket_accept(&server_socket, NX_WAIT_FOREVER), NX_SUCCESS);
    assert_int_equal(nx_secure_tls_session_start(&tls_server_session, &server_socket, NX_WAIT_FOREVER), NX_SUCCESS);

    /* Receive MQTT CONN. */
    assert_int_equal(nx_secure_tls_session_receive(&tls_server_session, &packet_ptr, NX_WAIT_FOREVER), NX_SUCCESS);
    assert_int_equal(nx_packet_release(packet_ptr), NX_SUCCESS);
    assert_int_equal(nx_secure_tls_packet_allocate(&tls_server_session, pool_ptr, &packet_ptr, NX_WAIT_FOREVER), NX_SUCCESS);

    /* Construct CONNACK packet. */
    byte = packet_ptr->nx_packet_prepend_ptr;
    byte[0] = 0x20;
    byte[1] = 0x02;
    byte[2] = 0;
    byte[3] = 0;
    packet_ptr->nx_packet_append_ptr = packet_ptr->nx_packet_prepend_ptr + 4;
    packet_ptr->nx_packet_length = 4;

    /* Send CONNACK packet. */
    assert_int_equal(nx_secure_tls_session_send(&tls_server_session, packet_ptr, NX_WAIT_FOREVER), NX_SUCCESS);
}

static void mqtt_server_deinit(NX_IP *ip_ptr)
{
    nx_secure_tls_session_end(&tls_server_session, NX_NO_WAIT);
    nx_tcp_socket_disconnect(&server_socket, NX_NO_WAIT);
    nx_tcp_server_socket_unaccept(&server_socket);
    nx_secure_tls_session_delete(&tls_server_session);
    nx_tcp_server_socket_unlisten(ip_ptr, NXD_MQTT_TLS_PORT);
    nx_tcp_socket_delete(&server_socket);
}

void connection_status_cb(struct NX_AZURE_IOT_HUB_CLIENT_STRUCT *hub_client_ptr, UINT status)
{
    connection_status = status;
}

UINT  __wrap__nxde_dns_host_by_name_get(NX_DNS *dns_ptr, UCHAR *host_name, NXD_ADDRESS *host_address_ptr,
                                        ULONG wait_option, UINT lookup_type)
{
    printf("HIJACKED: %s\n", __func__);
    host_address_ptr -> nxd_ip_version = NX_IP_VERSION_V4;
    host_address_ptr -> nxd_ip_address.v4 = IP_ADDRESS(127, 0, 0, 1);
    return(NX_SUCCESS);
}
