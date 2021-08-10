#include <stdio.h>
#include "wifi.h"

#define WIFI_FAIL 1
#define WIFI_OK   0

#define TERMINAL_USE

#ifdef TERMINAL_USE
extern ES_WIFIObject_t    EsWifiObj;
#endif /* TERMINAL_USE */

#define TERMINAL_USE

#ifndef RETRY_TIMES
#define RETRY_TIMES 3
#endif

int  wifi_setup(char *ssid, char *password)
{
uint8_t  MAC_Addr[6];
uint8_t  IP_Addr[4]; 
uint8_t  Gateway_Addr[4];
uint8_t  DNS1_Addr[4]; 
uint8_t  DNS2_Addr[4];
uint32_t retry_connect=0;

  /*Initialize and use WIFI module */
  if(WIFI_Init() ==  WIFI_STATUS_OK)
  {
      
#if defined (TERMINAL_USE)      
      /* ES-WIFI info.  */
      printf("ES-WIFI Firmware:\r\n");
      printf("> Product Name: %s\r\n", EsWifiObj.Product_Name);
      printf("> Product ID: %s\r\n", EsWifiObj.Product_ID);
      printf("> Firmware Version: %s\r\n", EsWifiObj.FW_Rev);
      printf("> API Version: %s\r\n", EsWifiObj.API_Rev);      
#endif /* TERMINAL_USE */     
    
      if(WIFI_GetMAC_Address(MAC_Addr) == WIFI_STATUS_OK)
      {       
#if defined (TERMINAL_USE)    
        printf("ES-WIFI MAC Address: %X:%X:%X:%X:%X:%X\r\n",     
               MAC_Addr[0],
               MAC_Addr[1],
               MAC_Addr[2],
               MAC_Addr[3],
               MAC_Addr[4],
               MAC_Addr[5]);   
#endif /* TERMINAL_USE */
      }
      else
      {
#if defined (TERMINAL_USE)  
        printf("!!!ERROR: ES-WIFI Get MAC Address Failed.\r\n");
#endif /* TERMINAL_USE */ 
        return WIFI_FAIL;
      }

      WIFI_Ecn_t ecn;

      if (!password) {
          ecn = WIFI_ECN_OPEN;
      } else {
          ecn = WIFI_ECN_WPA2_PSK;
      }
   
      while((retry_connect++) < RETRY_TIMES)
      { 
        printf("wifi connect try %d times\r\n",retry_connect);

        if( (WIFI_Connect(ssid, password, ecn) == WIFI_STATUS_OK))
        {
        #if defined (TERMINAL_USE)  
          printf("ES-WIFI Connected.\r\n");
        #endif /* TERMINAL_USE */ 
          
          if(WIFI_GetIP_Address(IP_Addr) == WIFI_STATUS_OK)
          {
        #if defined (TERMINAL_USE)
            printf("> ES-WIFI IP Address: %d.%d.%d.%d\r\n",     
                   IP_Addr[0],
                   IP_Addr[1],
                   IP_Addr[2],
                   IP_Addr[3]); 

            if(WIFI_GetGateway_Address(Gateway_Addr) == WIFI_STATUS_OK)
            {
                printf("> ES-WIFI Gateway Address: %d.%d.%d.%d\r\n",     
                       Gateway_Addr[0],
                       Gateway_Addr[1],
                       Gateway_Addr[2],
                       Gateway_Addr[3]); 
            }
            
            if(WIFI_GetDNS_Address(DNS1_Addr,DNS2_Addr) == WIFI_STATUS_OK)
            {
                printf("> ES-WIFI DNS1 Address: %d.%d.%d.%d\r\n",     
                DNS1_Addr[0],
                DNS1_Addr[1],
                DNS1_Addr[2],
                DNS1_Addr[3]); 
                
                printf("> ES-WIFI DNS2 Address: %d.%d.%d.%d\r\n",     
                DNS2_Addr[0],
                DNS2_Addr[1],
                DNS2_Addr[2],
                DNS2_Addr[3]);           
            }
        #endif /* TERMINAL_USE */       
            
            if((IP_Addr[0] == 0)&& 
               (IP_Addr[1] == 0)&& 
               (IP_Addr[2] == 0)&&
               (IP_Addr[3] == 0))
              return WIFI_FAIL;  
          }
          else
          {    
        #if defined (TERMINAL_USE)  
            printf("!!!ERROR: ES-WIFI Get IP Address Failed.\r\n");
        #endif /* TERMINAL_USE */
            return WIFI_FAIL;
          }
          
          break;
        }
      }
     if(retry_connect > RETRY_TIMES)
     {
#if defined (TERMINAL_USE)  
        printf("!!!ERROR: ES-WIFI NOT connected.\r\n");
#endif /* TERMINAL_USE */ 
        return WIFI_FAIL;
     }
  }
  else
  {   
#if defined (TERMINAL_USE)  
    printf("!!!ERROR: ES-WIFI Initialize Failed.\r\n"); 
#endif /* TERMINAL_USE */
    return WIFI_FAIL;
  }
  
  return WIFI_OK;
}