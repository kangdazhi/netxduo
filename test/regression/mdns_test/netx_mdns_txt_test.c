#include   "tx_api.h"
#include   "nx_api.h"
#include   "nx_ram_network_driver_test_1500.h"
                         
extern void    test_control_return(UINT status);

#if defined __PRODUCT_NETXDUO__ && !defined NX_MDNS_DISABLE_SERVER && !defined NX_DISABLE_IPV4
#include   "nxd_mdns.h"

#define     DEMO_STACK_SIZE    2048
#define     BUFFER_SIZE        10240
#define     LOCAL_FULL_SERVICE_COUNT    16
#define     PEER_FULL_SERVICE_COUNT     16
#define     PEER_PARTIAL_SERVICE_COUNT  32

/* Define the ThreadX and NetX object control blocks...  */

static TX_THREAD               ntest_0;

static NX_PACKET_POOL          pool_0;
static NX_IP                   ip_0;

/* Define the NetX MDNS object control blocks.  */

static NX_MDNS                 mdns_0;
static UCHAR                   buffer[BUFFER_SIZE];
static ULONG                   current_buffer_size;
static CHAR                   *test_strings[] = {

    /* Total length 255. */
    "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ0abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ0abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ0abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTU=12345678",

    /* Total length 256. */
    "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ0abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ0abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ0abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTU=123456789"
};

/* Define the counters used in the test application...  */

static ULONG                   error_counter;
static CHAR                   *pointer;

/* Define thread prototypes.  */

static void    ntest_0_entry(ULONG thread_input);
extern void    test_control_return(UINT status);
extern VOID    _nx_ram_network_driver_1500(NX_IP_DRIVER *driver_req_ptr);

/* Define what the initial system looks like.  */

#ifdef CTEST
VOID test_application_define(void *first_unused_memory)
#else
void           netx_mdns_txt_test(void *first_unused_memory)
#endif
{

UINT       status;

    /* Setup the working pointer.  */
    pointer = (CHAR *) first_unused_memory;
    error_counter = 0;

    /* Initialize the NetX system.  */
    nx_system_initialize();

    /* Create a packet pool.  */
    status = nx_packet_pool_create(&pool_0, "NetX Main Packet Pool", 512, pointer, 8192);
    pointer = pointer + 8192;

    if(status)
        error_counter++;

    /* Create an IP instance.  */
    status = nx_ip_create(&ip_0, "NetX IP Instance 0", IP_ADDRESS(1, 2, 3, 4), 0xFFFFFF00UL, &pool_0, 
                          _nx_ram_network_driver_1500, pointer, 2048, 1);
    pointer = pointer + 2048;

    /* Check for IP create errors.  */
    if (status)
        error_counter++;

    /* Enable ARP and supply ARP cache memory for IP Instance 0.  */
    status = nx_arp_enable(&ip_0, (void *) pointer, 1024);
    pointer = pointer + 1024;
    if(status)
        error_counter++;

    /* Enable UDP processing for both IP instances.  */
    status = nx_udp_enable(&ip_0);

    /* Check UDP enable status.  */
    if(status)
        error_counter++;
    
    status = nx_igmp_enable(&ip_0);

    /* Check status. */
    if(status)
        error_counter++;

    /* Create the test thread.  */
    tx_thread_create(&ntest_0, "thread 0", ntest_0_entry, NX_NULL,  
                     pointer, DEMO_STACK_SIZE, 
                     3, 3, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + DEMO_STACK_SIZE;
}

/* Define the test threads.  */

static void    ntest_0_entry(ULONG thread_input)
{
UINT       status;
ULONG      actual_status;

    printf("NetX Test:   MDNS TXT Test.............................................");

    /* Ensure the IP instance has been initialized.  */
    status = nx_ip_status_check(&ip_0, NX_IP_INITIALIZE_DONE, &actual_status, 100);

    /* Check status. */
    if(status != NX_SUCCESS)
    {
        printf("ERROR!\n");
        test_control_return(1);
    }

    /* Create mDNS. */
    current_buffer_size = (BUFFER_SIZE >> 1);
    status = nx_mdns_create(&mdns_0, &ip_0, &pool_0, 2, pointer, DEMO_STACK_SIZE, "NETX-MDNS",  
                            buffer, current_buffer_size, buffer + current_buffer_size, current_buffer_size, NX_NULL);
    pointer = pointer + DEMO_STACK_SIZE;

    /* Check status. */
    if(status != NX_SUCCESS)
    {
        printf("ERROR!\n");
        test_control_return(1);
    }

    /* Enable mDNS.  */
    status = nx_mdns_enable(&mdns_0, 0);

    /* Check status. */
    if(status != NX_SUCCESS)
    {
        printf("ERROR!\n");
        test_control_return(1);
    }

    /* Add services. */
    /* The TXT length is 255. */
    status = nx_mdns_service_add(&mdns_0, "test", (CHAR *)"_ipp._tcp", NX_NULL, test_strings[0], 100, 0, 0, 80, NX_MDNS_RR_SET_UNIQUE, 0);

    /* Check status. */
    if(status)
        error_counter++;

#ifndef NX_DISABLE_ERROR_CHECKING
    /* The TXT length is 256. */
    status = nx_mdns_service_add(&mdns_0, "test1", (CHAR *)"_ipp._tcp", NX_NULL, test_strings[1], 100, 0, 0, 80, NX_MDNS_RR_SET_UNIQUE, 0);

    /* Check status. */
    if(!status)
        error_counter++;
#endif /* NX_DISABLE_ERROR_CHECKING  */

    /* Determine if the test was successful.  */
    if(error_counter)
    {
        printf("ERROR!\n");
        test_control_return(1);
    }
    else
    {
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}   
#else
#ifdef CTEST
VOID test_application_define(void *first_unused_memory)
#else
void           netx_mdns_txt_test(void *first_unused_memory)
#endif
{ 
    printf("NetX Test:   MDNS TXT Test.............................................N/A\n");
    test_control_return(3);
}
#endif /* NX_MDNS_DISABLE_SERVER  */
