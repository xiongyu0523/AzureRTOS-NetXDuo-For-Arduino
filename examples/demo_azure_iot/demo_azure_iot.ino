/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

#include <nx_api.h>
#ifndef SAMPLE_DHCP_DISABLE
#include "nxd_dhcp_client.h"
#endif /* SAMPLE_DHCP_DISABLE */
#include "nxd_dns.h"
#include "nxd_sntp_client.h"
#include "nx_secure_tls_api.h"
#include "nx_azure_iot_hub_client.h"
#include "nx_azure_iot_provisioning_client.h"
#include "nx_azure_iot_cert.h"

/***********************************************************************
 * 
 *    WIFI credential Configuration
 *    
 **********************************************************************/

#ifndef WIFI_SSID
/* #define WIFI_SSID                       "" */
#error "Symbol WIFI_SSID must be defined." 
#endif

#ifndef WIFI_PASSWORD
/* #define WIFI_PASSWORD                   "" */
#error "Symbol WIFI_PASSWORD must be defined. Define to NULL to connect Open WiFi AP"
#endif

/***********************************************************************
 * 
 *    Azure IoT credential Configuration
 *    
 **********************************************************************/

/* This sample uses Symmetric key (SAS) to connect to IoT Hub by default,
   simply defining USE_DEVICE_CERTIFICATE and setting your device certificate in sample_device_identity.c
   to connect to IoT Hub with x509 certificate. Set up X.509 security in your Azure IoT Hub,
   refer to https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-security-x509-get-started  */
/* 
#define USE_DEVICE_CERTIFICATE          1
*/

/* Defined, DPS is enabled.  */
/*
#define ENABLE_DPS_SAMPLE
*/

#ifndef ENABLE_DPS_SAMPLE

#ifndef HOST_NAME
#define HOST_NAME                       "<youriothub>.azure-devices.net"
#endif

#ifndef DEVICE_ID
#define DEVICE_ID                       "<yourdevice>"
#endif

#else /* !ENABLE_DPS_SAMPLE */

#ifndef ENDPOINT
#define ENDPOINT                        ""
#endif

#ifndef ID_SCOPE
#define ID_SCOPE                        ""
#endif

#ifndef REGISTRATION_ID
#define REGISTRATION_ID                 ""
#endif

#endif /* ENABLE_DPS_SAMPLE */

/* Optional SYMMETRIC KEY.  */
#ifndef DEVICE_SYMMETRIC_KEY
#define DEVICE_SYMMETRIC_KEY            "<yoursymmetrickey>"
#endif

/* Optional module ID.  */
#ifndef MODULE_ID
#define MODULE_ID                       ""
#endif

#if (USE_DEVICE_CERTIFICATE == 1)

/* Using X509 certificate authenticate to connect to IoT Hub,
   set the device certificate as your device.  */

/* Device Key type. */
#ifndef DEVICE_KEY_TYPE
#define DEVICE_KEY_TYPE                 NX_SECURE_X509_KEY_TYPE_RSA_PKCS1_DER
#endif

/* Device certificate.  */
#ifndef DEVICE_CERT
#define DEVICE_CERT                     {0x00}
#endif

/* Device Private Key.  */ 
#ifndef DEVICE_PRIVATE_KEY
#define DEVICE_PRIVATE_KEY              {0x00}
#endif

const UCHAR sample_device_cert_ptr[] = DEVICE_CERT;
const UINT sample_device_cert_len = sizeof(sample_device_cert_ptr);
const UCHAR sample_device_private_key_ptr[] = DEVICE_PRIVATE_KEY;
const UINT sample_device_private_key_len = sizeof(sample_device_private_key_ptr);

NX_SECURE_X509_CERT                     device_certificate;

#endif /* USE_DEVICE_CERTIFICATE */

/***********************************************************************
 * 
 *    Ciphersuite Configuration section
 *    
 **********************************************************************/

/* Define supported crypto method.  */
extern NX_CRYPTO_METHOD crypto_method_hmac;
extern NX_CRYPTO_METHOD crypto_method_hmac_sha256;
extern NX_CRYPTO_METHOD crypto_method_tls_prf_sha256;
extern NX_CRYPTO_METHOD crypto_method_sha256;
extern NX_CRYPTO_METHOD crypto_method_sha384;
extern NX_CRYPTO_METHOD crypto_method_aes_cbc_128;
extern NX_CRYPTO_METHOD crypto_method_rsa;

#ifdef NX_SECURE_ENABLE_ECC_CIPHERSUITE
extern NX_CRYPTO_METHOD crypto_method_ecdhe;
extern NX_CRYPTO_METHOD crypto_method_ecdsa;
extern NX_CRYPTO_METHOD crypto_method_ec_secp384;
#endif

const NX_CRYPTO_METHOD *_nx_azure_iot_tls_supported_crypto[] =
{
    &crypto_method_hmac,
    &crypto_method_hmac_sha256,
    &crypto_method_tls_prf_sha256,
    &crypto_method_sha256,
    &crypto_method_sha384,
    &crypto_method_aes_cbc_128,
    &crypto_method_rsa,
#ifdef NX_SECURE_ENABLE_ECC_CIPHERSUITE
    &crypto_method_ecdhe,
    &crypto_method_ecdsa,
    &crypto_method_ec_secp384,
#endif
};

const UINT _nx_azure_iot_tls_supported_crypto_size = sizeof(_nx_azure_iot_tls_supported_crypto) / sizeof(NX_CRYPTO_METHOD*);

/* Define supported TLS ciphersuites.  */
#ifdef NX_SECURE_ENABLE_ECC_CIPHERSUITE
extern const NX_CRYPTO_CIPHERSUITE nx_crypto_tls_ecdhe_rsa_with_aes_128_cbc_sha256;
#else
extern const NX_CRYPTO_CIPHERSUITE nx_crypto_tls_rsa_with_aes_128_cbc_sha256;
#endif

/* Define supported X.509 ciphersuites.  */
extern const NX_CRYPTO_CIPHERSUITE nx_crypto_x509_rsa_sha_256;
extern const NX_CRYPTO_CIPHERSUITE nx_crypto_x509_rsa_sha_384;
#ifdef NX_SECURE_ENABLE_ECC_CIPHERSUITE
extern const NX_CRYPTO_CIPHERSUITE nx_crypto_x509_ecdsa_sha_256;
#endif

const NX_CRYPTO_CIPHERSUITE *_nx_azure_iot_tls_ciphersuite_map[] =
{
    /* TLS ciphersuites. */
#ifdef NX_SECURE_ENABLE_ECC_CIPHERSUITE
    &nx_crypto_tls_ecdhe_rsa_with_aes_128_cbc_sha256,
#else
    &nx_crypto_tls_rsa_with_aes_128_cbc_sha256,
#endif

    /* X.509 ciphersuites. */
    &nx_crypto_x509_rsa_sha_256,
    &nx_crypto_x509_rsa_sha_384,
#ifdef NX_SECURE_ENABLE_ECC_CIPHERSUITE
    &nx_crypto_x509_ecdsa_sha_256,
#endif
};

const UINT _nx_azure_iot_tls_ciphersuite_map_size = sizeof(_nx_azure_iot_tls_ciphersuite_map) / sizeof(NX_CRYPTO_CIPHERSUITE*);

/***********************************************************************
 * 
 *    Macro configurations section
 *    
 **********************************************************************/

#define SAMPLE_HELPER_STACK_SIZE                    (4096)
#define SAMPLE_HELPER_THREAD_PRIORITY               (4)
#define SAMPLE_IP_STACK_SIZE                        (2048)
#define SAMPLE_PACKET_COUNT                         (32)
#define SAMPLE_PACKET_SIZE                          (1536)
#define SAMPLE_POOL_SIZE                            ((SAMPLE_PACKET_SIZE + sizeof(NX_PACKET)) * SAMPLE_PACKET_COUNT)
#define SAMPLE_ARP_CACHE_SIZE                       (512)
#define SAMPLE_IP_THREAD_PRIORITY                   (1)
#define NX_AZURE_IOT_STACK_SIZE                     (2048)
#define NX_AZURE_IOT_THREAD_PRIORITY                (4)
#define SAMPLE_MAX_BUFFER                           (256)
#define SAMPLE_STACK_SIZE                           (2048)
#define SAMPLE_THREAD_PRIORITY                      (16)
#define MAX_PROPERTY_COUNT                          2
#define NX_AZURE_IOT_TLS_METADATA_BUFFER_SIZE       (10 * 1024)
#define SAMPLE_SNTP_SYNC_MAX                        30
#define SAMPLE_SNTP_UPDATE_MAX                      10
#define SAMPLE_SNTP_UPDATE_INTERVAL                 (NX_IP_PERIODIC_RATE / 2)
#define SAMPLE_SYSTEM_TIME                          1640995200
#define SAMPLE_UNIX_TO_NTP_EPOCH_SECOND             0x83AA7E80

#if defined(SAMPLE_DHCP_DISABLE) && !defined(SAMPLE_NETWORK_CONFIGURE)
#ifndef SAMPLE_IPV4_ADDRESS
/*#define SAMPLE_IPV4_ADDRESS           IP_ADDRESS(192, 168, 100, 33)*/
#error "SYMBOL SAMPLE_IPV4_ADDRESS must be defined. This symbol specifies the IP address of device. "
#endif /* SAMPLE_IPV4_ADDRESS */

#ifndef SAMPLE_IPV4_MASK
/*#define SAMPLE_IPV4_MASK              0xFFFFFF00UL*/
#error "SYMBOL SAMPLE_IPV4_MASK must be defined. This symbol specifies the IP address mask of device. "
#endif /* SAMPLE_IPV4_MASK */

#ifndef SAMPLE_GATEWAY_ADDRESS
/*#define SAMPLE_GATEWAY_ADDRESS        IP_ADDRESS(192, 168, 100, 1)*/
#error "SYMBOL SAMPLE_GATEWAY_ADDRESS must be defined. This symbol specifies the gateway address for routing. "
#endif /* SAMPLE_GATEWAY_ADDRESS */

#ifndef SAMPLE_DNS_SERVER_ADDRESS
/*#define SAMPLE_DNS_SERVER_ADDRESS     IP_ADDRESS(192, 168, 100, 1)*/
#error "SYMBOL SAMPLE_DNS_SERVER_ADDRESS must be defined. This symbol specifies the dns server address for routing. "
#endif /* SAMPLE_DNS_SERVER_ADDRESS */
#else
#define SAMPLE_IPV4_ADDRESS             IP_ADDRESS(0, 0, 0, 0)
#define SAMPLE_IPV4_MASK                IP_ADDRESS(0, 0, 0, 0)
#endif /* SAMPLE_DHCP_DISABLE */

/***********************************************************************
 * 
 *    Typedef section
 *    
 **********************************************************************/

/* Generally, IoTHub Client and DPS Client do not run at the same time, user can use union as below to
   share the memory between IoTHub Client and DPS Client. */
typedef union SAMPLE_CLIENT_UNION
{
    NX_AZURE_IOT_HUB_CLIENT               iothub_client;
    
#ifdef ENABLE_DPS_SAMPLE
    NX_AZURE_IOT_PROVISIONING_CLIENT      prov_client;
#endif

} SAMPLE_CLIENT;

/***********************************************************************
 * 
 *    Global Variables section
 *    
 **********************************************************************/

TX_THREAD            sample_helper_thread;
NX_PACKET_POOL       pool_0;
NX_IP                ip_0;
NX_DNS               dns_0;

#ifndef SAMPLE_DHCP_DISABLE
NX_DHCP              dhcp_0;
#endif

NX_SNTP_CLIENT       sntp_client;
ULONG                unix_time_base;
ULONG                sample_ip_stack[SAMPLE_IP_STACK_SIZE / sizeof(ULONG)];
ULONG                sample_pool[SAMPLE_POOL_SIZE / sizeof(ULONG)];
ULONG                sample_pool_size = sizeof(sample_pool);

#ifndef SAMPLE_NETWORK_CONFIGURE
ULONG                sample_arp_cache_area[SAMPLE_ARP_CACHE_SIZE / sizeof(ULONG)];
#endif

ULONG                sample_helper_thread_stack[SAMPLE_HELPER_STACK_SIZE / sizeof(ULONG)];
const CHAR           *sntp_servers[] = { "0.pool.ntp.org", "1.pool.ntp.org", "2.pool.ntp.org", "3.pool.ntp.org" };
UINT                 sntp_server_index;
void                 sample_helper_thread_entry(ULONG parameter);
NX_SECURE_X509_CERT  root_ca_cert;
NX_SECURE_X509_CERT  root_ca_cert_2;
NX_SECURE_X509_CERT  root_ca_cert_3;
UCHAR                nx_azure_iot_tls_metadata_buffer[NX_AZURE_IOT_TLS_METADATA_BUFFER_SIZE];
ULONG                nx_azure_iot_thread_stack[NX_AZURE_IOT_STACK_SIZE / sizeof(ULONG)];
NX_AZURE_IOT         nx_azure_iot;
SAMPLE_CLIENT        client;

#define iothub_client   client.iothub_client
#ifdef ENABLE_DPS_SAMPLE
#define prov_client     client.prov_client
#endif

#ifdef ENABLE_DPS_SAMPLE
UCHAR                sample_iothub_hostname[SAMPLE_MAX_BUFFER];
UCHAR                sample_iothub_device_id[SAMPLE_MAX_BUFFER];
#endif

TX_THREAD            sample_telemetry_thread;
TX_THREAD            sample_c2d_thread;
TX_THREAD            sample_direct_method_thread;
TX_THREAD            sample_device_twin_thread;
ULONG                sample_telemetry_thread_stack[SAMPLE_STACK_SIZE / sizeof(ULONG)];
ULONG                sample_c2d_thread_stack[SAMPLE_STACK_SIZE / sizeof(ULONG)];
ULONG                sample_direct_method_thread_stack[SAMPLE_STACK_SIZE / sizeof(ULONG)];
ULONG                sample_device_twin_thread_stack[SAMPLE_STACK_SIZE / sizeof(ULONG)];
const CHAR           *sample_properties[MAX_PROPERTY_COUNT][2] = { { "propertyA", "valueA" }, { "propertyB", "valueB" } };
CHAR                 method_response_payload[] = "{\"status\": \"OK\"}";
CHAR                 fixed_reported_properties[] = "{\"sample_report\": \"OK\"}";

/***********************************************************************
 * 
 *    External Function declartion section
 *    
 **********************************************************************/

/* These functions are defined in BSP use C */

extern "C" {
  
VOID SAMPLE_NETWORK_DRIVER(NX_IP_DRIVER *driver_req);

#ifdef SAMPLE_BOARD_SETUP
VOID SAMPLE_BOARD_SETUP();
#endif

#ifdef SAMPLE_WIFI_SETUP
INT SAMPLE_WIFI_SETUP(CHAR *ssid, CHAR *password);
#endif 

#ifdef SAMPLE_NETWORK_CONFIGURE
VOID SAMPLE_NETWORK_CONFIGURE(NX_IP *ip_ptr, ULONG *dns_address);
#endif

}

/***********************************************************************
 * 
 *    Function definition section
 *    
 **********************************************************************/

/* Define what the initial system looks like.  */
void tx_application_define(void *first_unused_memory)
{

UINT  status;

      NX_PARAMETER_NOT_USED(first_unused_memory);
  
      /* Initialize the NetX system.  */
      nx_system_initialize();

    /* Create a packet pool.  */
    status = nx_packet_pool_create(&pool_0, "NetX Main Packet Pool", SAMPLE_PACKET_SIZE,
                                   (UCHAR *)sample_pool, sample_pool_size);

    /* Check for pool creation error.  */
    if (status)
    {
        ("nx_packet_pool_create fail: %u\r\n", status);
        return;
    }

    /* Create an IP instance.  */
    status = nx_ip_create(&ip_0, "NetX IP Instance 0",
                          SAMPLE_IPV4_ADDRESS, SAMPLE_IPV4_MASK,
                          &pool_0, SAMPLE_NETWORK_DRIVER,
                          (UCHAR*)sample_ip_stack, sizeof(sample_ip_stack),
                          SAMPLE_IP_THREAD_PRIORITY);

    /* Check for IP create errors.  */
    if (status)
    {
        Serial.printf("nx_ip_create fail: %u\r\n", status);
        return;
    }

#ifndef SAMPLE_NETWORK_CONFIGURE
    /* Enable ARP and supply ARP cache memory for IP Instance 0.  */
    status = nx_arp_enable(&ip_0, (VOID *)sample_arp_cache_area, sizeof(sample_arp_cache_area));

    /* Check for ARP enable errors.  */
    if (status)
    {
        Serial.printf("nx_arp_enable fail: %u\r\n", status);
        return;
    }

    /* Enable ICMP traffic.  */
    status = nx_icmp_enable(&ip_0);

    /* Check for ICMP enable errors.  */
    if (status)
    {
        Serial.printf("nx_icmp_enable fail: %u\r\n", status);
        return;
    }
#endif

    /* Enable TCP traffic.  */
    status = nx_tcp_enable(&ip_0);

    /* Check for TCP enable errors.  */
    if (status)
    {
        Serial.printf("nx_tcp_enable fail: %u\r\n", status);
        return;
    }

    /* Enable UDP traffic.  */
    status = nx_udp_enable(&ip_0);

    /* Check for UDP enable errors.  */
    if (status)
    {
        Serial.printf("nx_udp_enable fail: %u\r\n", status);
        return;
    }

    /* Initialize TLS.  */
    nx_secure_tls_initialize();

    /* Create sample helper thread.  */
    status = tx_thread_create(&sample_helper_thread, "Demo Thread",
                              sample_helper_thread_entry, 0,
                              sample_helper_thread_stack, SAMPLE_HELPER_STACK_SIZE,
                              SAMPLE_HELPER_THREAD_PRIORITY, SAMPLE_HELPER_THREAD_PRIORITY,
                              TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Check status.  */
    if (status)
    {
        Serial.printf("Demo helper thread creation fail: %u\r\n", status);
        return;
    }
}

/* Define sample helper thread entry.  */
void sample_helper_thread_entry(ULONG parameter)
{

UINT    status;
ULONG   ip_address = 0;
ULONG   network_mask = 0;
ULONG   gateway_address = 0;
UINT    unix_time;
ULONG   dns_server_address[3];
#ifndef SAMPLE_DHCP_DISABLE
UINT    dns_server_address_size = sizeof(dns_server_address);
#endif

    NX_PARAMETER_NOT_USED(parameter);

#ifndef SAMPLE_DHCP_DISABLE
    dhcp_wait();
#elif defined(SAMPLE_NETWORK_CONFIGURE)
    SAMPLE_NETWORK_CONFIGURE(&ip_0, &dns_server_address[0]);
#else
    nx_ip_gateway_address_set(&ip_0, SAMPLE_GATEWAY_ADDRESS);
#endif /* SAMPLE_DHCP_DISABLE  */

    /* Get IP address and gateway address.  */
    nx_ip_address_get(&ip_0, &ip_address, &network_mask);
    nx_ip_gateway_address_get(&ip_0, &gateway_address);

    /* Output IP address and gateway address.  */
    Serial.printf("IP address: %lu.%lu.%lu.%lu\r\n",
           (ip_address >> 24),
           (ip_address >> 16 & 0xFF),
           (ip_address >> 8 & 0xFF),
           (ip_address & 0xFF));
    Serial.printf("Mask: %lu.%lu.%lu.%lu\r\n",
           (network_mask >> 24),
           (network_mask >> 16 & 0xFF),
           (network_mask >> 8 & 0xFF),
           (network_mask & 0xFF));
    Serial.printf("Gateway: %lu.%lu.%lu.%lu\r\n",
           (gateway_address >> 24),
           (gateway_address >> 16 & 0xFF),
           (gateway_address >> 8 & 0xFF),
           (gateway_address & 0xFF));

#ifndef SAMPLE_DHCP_DISABLE
    /* Retrieve DNS server address.  */
    nx_dhcp_interface_user_option_retrieve(&dhcp_0, 0, NX_DHCP_OPTION_DNS_SVR, (UCHAR *)(dns_server_address),
                                           &dns_server_address_size);
#elif !defined(SAMPLE_NETWORK_CONFIGURE)
    dns_server_address[0] = SAMPLE_DNS_SERVER_ADDRESS;
#endif /* SAMPLE_DHCP_DISABLE */

    /* Create DNS.  */
    status = dns_create(dns_server_address[0]);

    /* Check for DNS create errors.  */
    if (status)
    {
        Serial.printf("dns_create fail: %u\r\n", status);
        return;
    }

    /* Sync up time by SNTP at start up.  */
    status = sntp_time_sync();

    /* Check status.  */
    if (status)
    {
        Serial.printf("SNTP Time Sync failed.\r\n");
        Serial.printf("Set Time to default value: SAMPLE_SYSTEM_TIME.");
        unix_time_base = SAMPLE_SYSTEM_TIME;
    }
    else
    {
        Serial.printf("SNTP Time Sync successfully.\r\n");
    }

    unix_time_get((ULONG *)&unix_time);
    srand(unix_time);

    /* Start sample.  */
    sample_entry(&ip_0, &pool_0, &dns_0, unix_time_get);
}

#ifndef SAMPLE_DHCP_DISABLE
void dhcp_wait()
{
ULONG   actual_status;

    Serial.printf("DHCP In Progress...\r\n");

    /* Create the DHCP instance.  */
    nx_dhcp_create(&dhcp_0, &ip_0, "DHCP Client");

    /* Request NTP server.  */
    nx_dhcp_user_option_request(&dhcp_0, NX_DHCP_OPTION_NTP_SVR);

    /* Start the DHCP Client.  */
    nx_dhcp_start(&dhcp_0);

    /* Wait util address is solved.  */
    nx_ip_status_check(&ip_0, NX_IP_ADDRESS_RESOLVED, &actual_status, NX_WAIT_FOREVER);
}
#endif

UINT dns_create(ULONG dns_server_address)
{
UINT    status;

    /* Create a DNS instance for the Client.  Note this function will create
       the DNS Client packet pool for creating DNS message packets intended
       for querying its DNS server.  */
    status = nx_dns_create(&dns_0, &ip_0, (UCHAR *)"DNS Client");
    if (status)
    {
        return(status);
    }

    /* Is the DNS client configured for the host application to create the packet pool?  */
#ifdef NX_DNS_CLIENT_USER_CREATE_PACKET_POOL

    /* Yes, use the packet pool created above which has appropriate payload size
       for DNS messages.  */
    status = nx_dns_packet_pool_set(&dns_0, ip_0.nx_ip_default_packet_pool);
    if (status)
    {
        nx_dns_delete(&dns_0);
        return(status);
    }
#endif /* NX_DNS_CLIENT_USER_CREATE_PACKET_POOL */

    /* Add an IPv4 server address to the Client list.  */
    status = nx_dns_server_add(&dns_0, dns_server_address);
    if (status)
    {
        nx_dns_delete(&dns_0);
        return(status);
    }

    /* Output DNS Server address.  */
    Serial.printf("DNS Server address: %lu.%lu.%lu.%lu\r\n",
           (dns_server_address >> 24),
           (dns_server_address >> 16 & 0xFF),
           (dns_server_address >> 8 & 0xFF),
           (dns_server_address & 0xFF));

    return(NX_SUCCESS);
}

/* Sync up the local time.  */
UINT sntp_time_sync_internal(ULONG sntp_server_address)
{
UINT    status;
UINT    server_status;
UINT    i;

    /* Create the SNTP Client to run in broadcast mode.. */
    status =  nx_sntp_client_create(&sntp_client, &ip_0, 0, &pool_0,  
                                    NX_NULL,
                                    NX_NULL,
                                    NX_NULL /* no random_number_generator callback */);

    /* Check status.  */
    if (status)
    {
        return(status);
    }

    /* Use the IPv4 service to initialize the Client and set the IPv4 SNTP server. */
    status = nx_sntp_client_initialize_unicast(&sntp_client, sntp_server_address);

    /* Check status.  */
    if (status)
    {
        nx_sntp_client_delete(&sntp_client);
        return(status);
    }

    /* Set local time to 0 */
    status = nx_sntp_client_set_local_time(&sntp_client, 0, 0);

    /* Check status.  */
    if (status)
    {
        nx_sntp_client_delete(&sntp_client);
        return(status);
    }

    /* Run Unicast client */
    status = nx_sntp_client_run_unicast(&sntp_client);

    /* Check status.  */
    if (status)
    {
        nx_sntp_client_stop(&sntp_client);
        nx_sntp_client_delete(&sntp_client);
        return(status);
    }

    /* Wait till updates are received */
    for (i = 0; i < SAMPLE_SNTP_UPDATE_MAX; i++)
    {

        /* First verify we have a valid SNTP service running. */
        status = nx_sntp_client_receiving_updates(&sntp_client, &server_status);

        /* Check status.  */
        if ((status == NX_SUCCESS) && (server_status == NX_TRUE))
        {

            /* Server status is good. Now get the Client local time. */
            ULONG sntp_seconds, sntp_fraction;
            ULONG system_time_in_second;

            /* Get the local time.  */
            status = nx_sntp_client_get_local_time(&sntp_client, &sntp_seconds, &sntp_fraction, NX_NULL);

            /* Check status.  */
            if (status != NX_SUCCESS)
            {
                continue;
            }

            /* Get the system time in second.  */
            system_time_in_second = tx_time_get() / TX_TIMER_TICKS_PER_SECOND;

            /* Convert to Unix epoch and minus the current system time.  */
            unix_time_base = (sntp_seconds - (system_time_in_second + SAMPLE_UNIX_TO_NTP_EPOCH_SECOND));

            /* Time sync successfully.  */

            /* Stop and delete SNTP.  */
            nx_sntp_client_stop(&sntp_client);
            nx_sntp_client_delete(&sntp_client);

            return(NX_SUCCESS);
        }

        /* Sleep.  */
        tx_thread_sleep(SAMPLE_SNTP_UPDATE_INTERVAL);
    }

    /* Time sync failed.  */

    /* Stop and delete SNTP.  */
    nx_sntp_client_stop(&sntp_client);
    nx_sntp_client_delete(&sntp_client);

    /* Return success.  */
    return(NX_NOT_SUCCESSFUL);
}

UINT sntp_time_sync()
{
UINT status;
ULONG sntp_server_address[3];
#ifndef SAMPLE_DHCP_DISABLE
UINT  sntp_server_address_size = sizeof(sntp_server_address);
#endif

#ifndef SAMPLE_DHCP_DISABLE
    /* Retrieve NTP server address.  */
    status = nx_dhcp_interface_user_option_retrieve(&dhcp_0, 0, NX_DHCP_OPTION_NTP_SVR, (UCHAR *)(sntp_server_address),
                                                    &sntp_server_address_size);

    /* Check status.  */
    if (status == NX_SUCCESS)
    {
        for (UINT i = 0; (i * 4) < sntp_server_address_size; i++)
        {
            Serial.printf("SNTP Time Sync...%lu.%lu.%lu.%lu (DHCP)\r\n", 
                   (sntp_server_address[i] >> 24),
                   (sntp_server_address[i] >> 16 & 0xFF),
                   (sntp_server_address[i] >> 8 & 0xFF),
                   (sntp_server_address[i] & 0xFF));

            /* Start SNTP to sync the local time.  */
            status = sntp_time_sync_internal(sntp_server_address[i]);

            /* Check status.  */
            if(status == NX_SUCCESS)
            {
                return(NX_SUCCESS);
            }
        }
    }
#endif /* SAMPLE_DHCP_DISABLE */

    /* Sync time by NTP server array.  */
    for (UINT i = 0; i < SAMPLE_SNTP_SYNC_MAX; i++)
    {
        Serial.printf("SNTP Time Sync...%s\r\n", sntp_servers[sntp_server_index]);

        /* Look up SNTP Server address. */
        status = nx_dns_host_by_name_get(&dns_0, (UCHAR *)sntp_servers[sntp_server_index], &sntp_server_address[0], 5 * NX_IP_PERIODIC_RATE);

        /* Check status.  */
        if (status == NX_SUCCESS)
        {

            /* Start SNTP to sync the local time.  */
            status = sntp_time_sync_internal(sntp_server_address[0]);

            /* Check status.  */
            if(status == NX_SUCCESS)
            {
                return(NX_SUCCESS);
            }
        }

        /* Switch SNTP server every time.  */
        sntp_server_index = (sntp_server_index + 1) % (sizeof(sntp_servers) / sizeof(sntp_servers[0]));
    }

    return(NX_NOT_SUCCESSFUL);
}

UINT unix_time_get(ULONG *unix_time)
{
    /* Return number of seconds since Unix Epoch (1/1/1970 00:00:00).  */
    *unix_time =  unix_time_base + (tx_time_get() / TX_TIMER_TICKS_PER_SECOND);

    return(NX_SUCCESS);
}

VOID printf_packet(NX_PACKET *packet_ptr)
{
    while (packet_ptr != NX_NULL)
    {
        Serial.printf("%.*s", (INT)(packet_ptr -> nx_packet_append_ptr - packet_ptr -> nx_packet_prepend_ptr),
               (CHAR *)packet_ptr -> nx_packet_prepend_ptr);
        packet_ptr = packet_ptr -> nx_packet_next;
    }
}

VOID connection_status_callback(NX_AZURE_IOT_HUB_CLIENT *hub_client_ptr, UINT status)
{
    NX_PARAMETER_NOT_USED(hub_client_ptr);
    if (status)
    {
        Serial.printf("Disconnected from IoTHub!: error code = 0x%08x\r\n", status);
    }
    else
    {
        Serial.printf("Connected to IoTHub.\r\n");
    }
}

UINT sample_initialize_iothub(NX_AZURE_IOT_HUB_CLIENT *iothub_client_ptr)
{
UINT status;
#ifdef ENABLE_DPS_SAMPLE
UCHAR *iothub_hostname = NX_NULL;
UCHAR *iothub_device_id = NX_NULL;
UINT iothub_hostname_length = 0;
UINT iothub_device_id_length = 0;
#else
UCHAR *iothub_hostname = (UCHAR *)HOST_NAME;
UCHAR *iothub_device_id = (UCHAR *)DEVICE_ID;
UINT iothub_hostname_length = sizeof(HOST_NAME) - 1;
UINT iothub_device_id_length = sizeof(DEVICE_ID) - 1;
#endif /* ENABLE_DPS_SAMPLE */

#ifdef ENABLE_DPS_SAMPLE

    /* Run DPS.  */
    if ((status = sample_dps_entry(&iothub_hostname, &iothub_hostname_length,
                                   &iothub_device_id, &iothub_device_id_length)))
    {
        Serial.printf("Failed on sample_dps_entry!: error code = 0x%08x\r\n", status);
        return(status);
    }
#endif /* ENABLE_DPS_SAMPLE */

    Serial.printf("IoTHub Host Name: %.*s; Device ID: %.*s.\r\n",
           iothub_hostname_length, iothub_hostname, iothub_device_id_length, iothub_device_id);

    /* Initialize IoTHub client.  */
    if ((status = nx_azure_iot_hub_client_initialize(iothub_client_ptr, &nx_azure_iot,
                                                     iothub_hostname, iothub_hostname_length,
                                                     iothub_device_id, iothub_device_id_length,
                                                     (UCHAR *)MODULE_ID, sizeof(MODULE_ID) - 1,
                                                     _nx_azure_iot_tls_supported_crypto,
                                                     _nx_azure_iot_tls_supported_crypto_size,
                                                     _nx_azure_iot_tls_ciphersuite_map,
                                                     _nx_azure_iot_tls_ciphersuite_map_size,
                                                     nx_azure_iot_tls_metadata_buffer,
                                                     sizeof(nx_azure_iot_tls_metadata_buffer),
                                                     &root_ca_cert)))
    {
        Serial.printf("Failed on nx_azure_iot_hub_client_initialize!: error code = 0x%08x\r\n", status);
        return(status);
    }

    /* Add more CA certificates.  */
    if ((status = nx_azure_iot_hub_client_trusted_cert_add(iothub_client_ptr, &root_ca_cert_2)))
    {
        Serial.printf("Failed on nx_azure_iot_hub_client_trusted_cert_add!: error code = 0x%08x\r\n", status);
    }
    else if ((status = nx_azure_iot_hub_client_trusted_cert_add(iothub_client_ptr, &root_ca_cert_3)))
    {
        Serial.printf("Failed on nx_azure_iot_hub_client_trusted_cert_add!: error code = 0x%08x\r\n", status);
    }

#if (USE_DEVICE_CERTIFICATE == 1)

    /* Initialize the device certificate.  */
    else if ((status = nx_secure_x509_certificate_initialize(&device_certificate,
                                                             (UCHAR *)sample_device_cert_ptr, (USHORT)sample_device_cert_len,
                                                             NX_NULL, 0,
                                                             (UCHAR *)sample_device_private_key_ptr, (USHORT)sample_device_private_key_len,
                                                             DEVICE_KEY_TYPE)))
    {
        Serial.printf("Failed on nx_secure_x509_certificate_initialize!: error code = 0x%08x\r\n", status);
    }

    /* Set device certificate.  */
    else if ((status = nx_azure_iot_hub_client_device_cert_set(iothub_client_ptr, &device_certificate)))
    {
        Serial.printf("Failed on nx_azure_iot_hub_client_device_cert_set!: error code = 0x%08x\r\n", status);
    }
#else

    /* Set symmetric key.  */
    else if ((status = nx_azure_iot_hub_client_symmetric_key_set(iothub_client_ptr,
                                                                 (UCHAR *)DEVICE_SYMMETRIC_KEY,
                                                                 sizeof(DEVICE_SYMMETRIC_KEY) - 1)))
    {
        Serial.printf("Failed on nx_azure_iot_hub_client_symmetric_key_set!\r\n");
    }
#endif /* USE_DEVICE_CERTIFICATE */

    /* Set connection status callback.  */
    else if ((status = nx_azure_iot_hub_client_connection_status_callback_set(iothub_client_ptr,
                                                                              connection_status_callback)))
    {
        Serial.printf("Failed on connection_status_callback!\r\n");
    }    
    else if ((status = nx_azure_iot_hub_client_cloud_message_enable(iothub_client_ptr)))
    {
        Serial.printf("C2D receive enable failed!: error code = 0x%08x\r\n", status);
    }
    else if ((status = nx_azure_iot_hub_client_direct_method_enable(iothub_client_ptr)))
    {
        Serial.printf("Direct method receive enable failed!: error code = 0x%08x\r\n", status);
    }
    else if ((status = nx_azure_iot_hub_client_device_twin_enable(iothub_client_ptr)))
    {
        Serial.printf("device twin enabled failed!: error code = 0x%08x\r\n", status);
    }

    if (status)
    {
        nx_azure_iot_hub_client_deinitialize(iothub_client_ptr);
    }
    
    return(status);
}

void log_callback(az_log_classification classification, UCHAR *msg, UINT msg_len)
{
    if (classification == AZ_LOG_IOT_AZURERTOS)
    {
        Serial.printf("%.*s", msg_len, (CHAR *)msg);
    }
}

void sample_entry(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr, UINT (*unix_time_callback)(ULONG *unix_time))
{
UINT status = 0;
UINT loop = NX_TRUE;

    nx_azure_iot_log_init(log_callback);

    /* Create Azure IoT handler.  */
    if ((status = nx_azure_iot_create(&nx_azure_iot, (UCHAR *)"Azure IoT", ip_ptr, pool_ptr, dns_ptr,
                                      nx_azure_iot_thread_stack, sizeof(nx_azure_iot_thread_stack),
                                      NX_AZURE_IOT_THREAD_PRIORITY, unix_time_callback)))
    {
        Serial.printf("Failed on nx_azure_iot_create!: error code = 0x%08x\r\n", status);
        return;
    }

    /* Initialize CA certificates.  */
    if ((status = nx_secure_x509_certificate_initialize(&root_ca_cert, (UCHAR *)_nx_azure_iot_root_cert,
                                                        (USHORT)_nx_azure_iot_root_cert_size,
                                                        NX_NULL, 0, NULL, 0, NX_SECURE_X509_KEY_TYPE_NONE)))
    {
        Serial.printf("Failed to initialize ROOT CA certificate!: error code = 0x%08x\r\n", status);
        nx_azure_iot_delete(&nx_azure_iot);
        return;
    }

    if ((status = nx_secure_x509_certificate_initialize(&root_ca_cert_2, (UCHAR *)_nx_azure_iot_root_cert_2,
                                                        (USHORT)_nx_azure_iot_root_cert_size_2,
                                                        NX_NULL, 0, NULL, 0, NX_SECURE_X509_KEY_TYPE_NONE)))
    {
        Serial.printf("Failed to initialize ROOT CA certificate!: error code = 0x%08x\r\n", status);
        nx_azure_iot_delete(&nx_azure_iot);
        return;
    }

    if ((status = nx_secure_x509_certificate_initialize(&root_ca_cert_3, (UCHAR *)_nx_azure_iot_root_cert_3,
                                                        (USHORT)_nx_azure_iot_root_cert_size_3,
                                                        NX_NULL, 0, NULL, 0, NX_SECURE_X509_KEY_TYPE_NONE)))
    {
        Serial.printf("Failed to initialize ROOT CA certificate!: error code = 0x%08x\r\n", status);
        nx_azure_iot_delete(&nx_azure_iot);
        return;
    }
    
    if ((status = sample_initialize_iothub(&iothub_client)))
    {
        Serial.printf("Failed to initialize iothub client: error code = 0x%08x\r\n", status);
        nx_azure_iot_delete(&nx_azure_iot);
        return;
    }

    if (nx_azure_iot_hub_client_connect(&iothub_client, NX_TRUE, NX_WAIT_FOREVER))
    {
        Serial.printf("Failed on nx_azure_iot_hub_client_connect!\r\n");
        nx_azure_iot_hub_client_deinitialize(&iothub_client);
        nx_azure_iot_delete(&nx_azure_iot);
        return;
    }

    /* Create Telemetry sample thread.  */
    if ((status = tx_thread_create(&sample_telemetry_thread, "Sample Telemetry Thread",
                                   sample_telemetry_thread_entry, 0,
                                   (UCHAR *)sample_telemetry_thread_stack, SAMPLE_STACK_SIZE,
                                   SAMPLE_THREAD_PRIORITY, SAMPLE_THREAD_PRIORITY,
                                   1, TX_AUTO_START)))
    {
        Serial.printf("Failed to create telemetry sample thread!: error code = 0x%08x\r\n", status);
    }

    /* Create C2D sample thread.  */
    if ((status = tx_thread_create(&sample_c2d_thread, "Sample C2D Thread",
                                   sample_c2d_thread_entry, 0,
                                   (UCHAR *)sample_c2d_thread_stack, SAMPLE_STACK_SIZE,
                                   SAMPLE_THREAD_PRIORITY, SAMPLE_THREAD_PRIORITY,
                                   1, TX_AUTO_START)))
    {
        Serial.printf("Failed to create c2d sample thread!: error code = 0x%08x\r\n", status);
    }

    /* Create Direct Method sample thread.  */
    if ((status = tx_thread_create(&sample_direct_method_thread, "Sample Direct Method Thread",
                                   sample_direct_method_thread_entry, 0,
                                   (UCHAR *)sample_direct_method_thread_stack, SAMPLE_STACK_SIZE,
                                   SAMPLE_THREAD_PRIORITY, SAMPLE_THREAD_PRIORITY,
                                   1, TX_AUTO_START)))
    {
        Serial.printf("Failed to create direct method sample thread!: error code = 0x%08x\r\n", status);
    }

    /* Create Device twin sample thread.  */
    if ((status = tx_thread_create(&sample_device_twin_thread, "Sample Device Twin Thread",
                                   sample_device_twin_thread_entry, 0,
                                   (UCHAR *)sample_device_twin_thread_stack, SAMPLE_STACK_SIZE,
                                   SAMPLE_THREAD_PRIORITY, SAMPLE_THREAD_PRIORITY,
                                   1, TX_AUTO_START)))
    {
        Serial.printf("Failed to create device twin sample thread!: error code = 0x%08x\r\n", status);
    }

    /* Simply loop in sample.  */
    while (loop)
    {
        tx_thread_sleep(NX_IP_PERIODIC_RATE);
    }
}

#ifdef ENABLE_DPS_SAMPLE
UINT sample_dps_entry(UCHAR **iothub_hostname, UINT *iothub_hostname_length, UCHAR **iothub_device_id, UINT *iothub_device_id_length)
{
UINT status;

    Serial.printf("Start Provisioning Client...\r\n");

    /* Initialize IoT provisioning client.  */
    if ((status = nx_azure_iot_provisioning_client_initialize(&prov_client, &nx_azure_iot,
                                                              (UCHAR *)ENDPOINT, sizeof(ENDPOINT) - 1,
                                                              (UCHAR *)ID_SCOPE, sizeof(ID_SCOPE) - 1,
                                                              (UCHAR *)REGISTRATION_ID, sizeof(REGISTRATION_ID) - 1,
                                                              _nx_azure_iot_tls_supported_crypto,
                                                              _nx_azure_iot_tls_supported_crypto_size,
                                                              _nx_azure_iot_tls_ciphersuite_map,
                                                              _nx_azure_iot_tls_ciphersuite_map_size,
                                                              nx_azure_iot_tls_metadata_buffer,
                                                              sizeof(nx_azure_iot_tls_metadata_buffer),
                                                              &root_ca_cert)))
    {
        Serial.printf("Failed on nx_azure_iot_provisioning_client_initialize!: error code = 0x%08x\r\n", status);
        return(status);
    }

    /* Initialize length of hostname and device ID.  */
    *iothub_hostname_length = sizeof(sample_iothub_hostname);
    *iothub_device_id_length = sizeof(sample_iothub_device_id);

    /* Add more CA certificates.  */
    if ((status = nx_azure_iot_provisioning_client_trusted_cert_add(&prov_client, &root_ca_cert_2)))
    {
        Serial.printf("Failed on nx_azure_iot_provisioning_client_trusted_cert_add!: error code = 0x%08x\r\n", status);
    }
    else if ((status = nx_azure_iot_provisioning_client_trusted_cert_add(&prov_client, &root_ca_cert_3)))
    {
        Serial.printf("Failed on nx_azure_iot_provisioning_client_trusted_cert_add!: error code = 0x%08x\r\n", status);
    }

#if (USE_DEVICE_CERTIFICATE == 1)

    /* Initialize the device certificate.  */
    else if ((status = nx_secure_x509_certificate_initialize(&device_certificate, (UCHAR *)sample_device_cert_ptr, (USHORT)sample_device_cert_len, NX_NULL, 0,
                                                             (UCHAR *)sample_device_private_key_ptr, (USHORT)sample_device_private_key_len, DEVICE_KEY_TYPE)))
    {
        Serial.printf("Failed on nx_secure_x509_certificate_initialize!: error code = 0x%08x\r\n", status);
    }

    /* Set device certificate.  */
    else if ((status = nx_azure_iot_provisioning_client_device_cert_set(&prov_client, &device_certificate)))
    {
        Serial.printf("Failed on nx_azure_iot_provisioning_client_device_cert_set!: error code = 0x%08x\r\n", status);
    }
#else

    /* Set symmetric key.  */
    else if ((status = nx_azure_iot_provisioning_client_symmetric_key_set(&prov_client, (UCHAR *)DEVICE_SYMMETRIC_KEY,
                                                                          sizeof(DEVICE_SYMMETRIC_KEY) - 1)))
    {
        Serial.printf("Failed on nx_azure_iot_hub_client_symmetric_key_set!: error code = 0x%08x\r\n", status);
    }
#endif /* USE_DEVICE_CERTIFICATE */

    /* Register device */
    else if ((status = nx_azure_iot_provisioning_client_register(&prov_client, NX_WAIT_FOREVER)))
    {
        Serial.printf("Failed on nx_azure_iot_provisioning_client_register!: error code = 0x%08x\r\n", status);
    }

    /* Get Device info */
    else if ((status = nx_azure_iot_provisioning_client_iothub_device_info_get(&prov_client,
                                                                               sample_iothub_hostname, iothub_hostname_length,
                                                                               sample_iothub_device_id, iothub_device_id_length)))
    {
        Serial.printf("Failed on nx_azure_iot_provisioning_client_iothub_device_info_get!: error code = 0x%08x\r\n", status);
    }
    else
    {
        *iothub_hostname = sample_iothub_hostname;
        *iothub_device_id = sample_iothub_device_id;
        Serial.printf("Registered Device Successfully.\r\n");
    }

    /* Destroy Provisioning Client.  */
    nx_azure_iot_provisioning_client_deinitialize(&prov_client);

    return(status);
}
#endif /* ENABLE_DPS_SAMPLE */

void sample_telemetry_thread_entry(ULONG parameter)
{
UINT i = 0;
UINT status = 0;
CHAR buffer[30];
UINT buffer_length;
UCHAR loop = NX_TRUE;
NX_PACKET *packet_ptr;

    NX_PARAMETER_NOT_USED(parameter);

    /* Loop to send telemetry message.  */
    while (loop)
    {

        /* Create a telemetry message packet.  */
        if ((status = nx_azure_iot_hub_client_telemetry_message_create(&iothub_client, &packet_ptr, NX_WAIT_FOREVER)))
        {
            Serial.printf("Telemetry message create failed!: error code = 0x%08x\r\n", status);
            break;
        }

        /* Add properties to telemetry message.  */
        for (int index = 0; index < MAX_PROPERTY_COUNT; index++)
        {
            if ((status =
                    nx_azure_iot_hub_client_telemetry_property_add(packet_ptr,
                                                                   (UCHAR *)sample_properties[index][0],
                                                                   (USHORT)strlen(sample_properties[index][0]),
                                                                   (UCHAR *)sample_properties[index][1],
                                                                   (USHORT)strlen(sample_properties[index][1]),
                                                                   NX_WAIT_FOREVER)))
            {
                Serial.printf("Telemetry property add failed!: error code = 0x%08x\r\n", status);
                break;
            }
        }

        if (status)
        {
            nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
            break;
        }

        buffer_length = (UINT)snprintf(buffer, sizeof(buffer), "{\"Message ID\":%u}", i++);
        if (nx_azure_iot_hub_client_telemetry_send(&iothub_client, packet_ptr,
                                                   (UCHAR *)buffer, buffer_length, NX_WAIT_FOREVER))
        {
            Serial.printf("Telemetry message send failed!: error code = 0x%08x\r\n", status);
            nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
            break;
        }
        Serial.printf("Telemetry message send: %s.\r\n", buffer);

        tx_thread_sleep(5 * NX_IP_PERIODIC_RATE);
    }
}

void sample_c2d_thread_entry(ULONG parameter)
{
UCHAR loop = NX_TRUE;
NX_PACKET *packet_ptr;
UINT status = 0;
USHORT property_buf_size;
const UCHAR *property_buf;

    NX_PARAMETER_NOT_USED(parameter);

    /* Loop to receive c2d message.  */
    while (loop)
    {
        if ((status = nx_azure_iot_hub_client_cloud_message_receive(&iothub_client, &packet_ptr, NX_WAIT_FOREVER)))
        {
            Serial.printf("C2D receive failed!: error code = 0x%08x\r\n", status);
            break;
        }

        if ((status = nx_azure_iot_hub_client_cloud_message_property_get(&iothub_client, packet_ptr,
                                                                         (UCHAR *)sample_properties[0][0],
                                                                         (USHORT)strlen(sample_properties[0][0]),
                                                                         &property_buf, &property_buf_size)) == NX_AZURE_IOT_SUCCESS)
        {
            Serial.printf("Receive property: %s = %.*s\r\n", sample_properties[0][0],
                   (INT)property_buf_size, property_buf);
        }

        Serial.printf("Receive message: ");
        printf_packet(packet_ptr);
        Serial.printf("\r\n");

        nx_packet_release(packet_ptr);
    }
}

void sample_direct_method_thread_entry(ULONG parameter)
{
UCHAR loop = NX_TRUE;
NX_PACKET *packet_ptr;
UINT status = 0;
USHORT method_name_length;
const UCHAR *method_name_ptr;
USHORT context_length;
VOID *context_ptr;

    NX_PARAMETER_NOT_USED(parameter);

    /* Loop to receive direct method message.  */
    while (loop)
    {
        if ((status = nx_azure_iot_hub_client_direct_method_message_receive(&iothub_client,
                                                                            &method_name_ptr, &method_name_length,
                                                                            &context_ptr, &context_length,
                                                                            &packet_ptr, NX_WAIT_FOREVER)))
        {
            Serial.printf("Direct method receive failed!: error code = 0x%08x\r\n", status);
            break;
        }

        Serial.printf("Receive method call: %.*s, with payload:", (INT)method_name_length, (CHAR *)method_name_ptr);
        printf_packet(packet_ptr);
        Serial.printf("\r\n");

        if ((status = nx_azure_iot_hub_client_direct_method_message_response(&iothub_client, 200 /* method status */,
                                                                             context_ptr, context_length,
                                                                             (UCHAR *)method_response_payload, sizeof(method_response_payload) - 1,
                                                                             NX_WAIT_FOREVER)))
        {
            Serial.printf("Direct method response failed!: error code = 0x%08x\r\n", status);
            nx_packet_release(packet_ptr);
            break;
        }

        nx_packet_release(packet_ptr);
    }
}

void sample_device_twin_thread_entry(ULONG parameter)
{
UCHAR loop = NX_TRUE;
NX_PACKET *packet_ptr;
UINT status = 0;
UINT response_status;
UINT request_id;
ULONG reported_property_version;

    NX_PARAMETER_NOT_USED(parameter);

    if ((status = nx_azure_iot_hub_client_device_twin_properties_request(&iothub_client, NX_WAIT_FOREVER)))
    {
        Serial.printf("device twin document request failed!: error code = 0x%08x\r\n", status);
        return;
    }

    if ((status = nx_azure_iot_hub_client_device_twin_properties_receive(&iothub_client, &packet_ptr, NX_WAIT_FOREVER)))
    {
        Serial.printf("device twin document receive failed!: error code = 0x%08x\r\n", status);
        return;
    }

    Serial.printf("Receive twin properties :");
    printf_packet(packet_ptr);
    Serial.printf("\r\n");
    nx_packet_release(packet_ptr);

    /* Loop to receive device twin message.  */
    while (loop)
    {
        if ((status = nx_azure_iot_hub_client_device_twin_desired_properties_receive(&iothub_client, &packet_ptr,
                                                                                     NX_WAIT_FOREVER)))
        {
            Serial.printf("Receive desired property receive failed!: error code = 0x%08x\r\n", status);
            break;
        }

        Serial.printf("Receive desired property call: ");
        printf_packet(packet_ptr);
        Serial.printf("\r\n");
        nx_packet_release(packet_ptr);

        if ((status = nx_azure_iot_hub_client_device_twin_reported_properties_send(&iothub_client,
                                                                                   (UCHAR *)fixed_reported_properties, sizeof(fixed_reported_properties) - 1,
                                                                                   &request_id, &response_status,
                                                                                   &reported_property_version,
                                                                                   NX_WAIT_FOREVER)))
        {
            Serial.printf("Device twin reported properties failed!: error code = 0x%08x\r\n", status);
            break;
        }

        if ((response_status < 200) || (response_status >= 300))
        {
            Serial.printf("device twin report properties failed with code : %d\r\n", response_status);
            break;
        }
    }
} 

void setup()
{
    Serial.begin(115200);
    Serial.printf("Azure RTOS NetX Duo Demo - Azure IoT\r\n");
  
#ifdef SAMPLE_BOARD_SETUP
    SAMPLE_BOARD_SETUP();
#endif /* SAMPLE_BOARD_SETUP */

#ifdef SAMPLE_WIFI_SETUP
    SAMPLE_WIFI_SETUP(WIFI_SSID, WIFI_PASSWORD);
#endif /* WIFI_SETUP */

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}

void loop() {
  // put your main code here, to run repeatedly:

}