//! @file
//! @brief Source file
//!
//! A simple program for sending a magic packet/Wake-On-LAN (WOL) packet
//! to a network card of a computer to wake up the PC.
//!
//! @note Compile it for Linux with:
//! gcc -Wall -Wextra -O3 -o wol wol.c wake_on_lan.c && strip wol
//!
//! @note Compile it and reduce size for Windows with:
//! gcc -Wall -Wextra -O3 -o wol.exe wol.c wake_on_lan.c -lws2_32
//! strip wol.exe

/*---------------------------------------------------------------------*
 *  private: include files
 *---------------------------------------------------------------------*/

#include "wake_on_lan.h"

#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/*---------------------------------------------------------------------*
 *  private: definitions
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: typedefs
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: variables
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  public:  variables
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: function prototypes
 *---------------------------------------------------------------------*/

int main(int argc, char * const argv[]);


/*---------------------------------------------------------------------*
 *  private: functions
 *---------------------------------------------------------------------*/

int main(int argc, char * const argv[])
{
#   if ( UINTMAX_MAX < UINT16_MAX )
#       error The function strtoumax() can not handle the size of the datatype
#   endif

    char ip[30] = { 0 };
    uint16_t port = 60000;
    char mac[30] = { 0 };
    
    bool parameter_i = false;
    bool parameter_m = false;
    bool help = false;
    bool silent = false;

    for (int i = 0; i < argc; ++i) {
        if('-' == argv[i][0])
        {
            switch(argv[i][1])
            {
                case 'i':
                    parameter_i = true;
                    i++;
                    strncpy(ip, argv[i], sizeof(ip)-1);
                    continue;

                case 'p':
                    i++;
                    port = strtoumax(argv[i], NULL, 10);
                    continue;

                case 'm':
                    parameter_m = true;
                    i++;
                    strncpy(mac, argv[i], sizeof(mac)-1);
                    continue;

                case 'h':
                    help = true;
                    break;

                case 's':
                    silent = true;
                    break;

                default:
                    break;
            }
        }
   }

   int return_value = 1;

   if(parameter_i && parameter_m)
   {
       int return_value = wake_on_lan(NULL, ip, port, mac);
       if( 0 != return_value)
       {
           if(1 <= return_value && 9 >= return_value )
           if(!silent)
           {
               printf("Error: %s\n", wake_on_lan_errors[return_value]);
               fflush(stdout);
           }
       }
   }
   else
   {
       help = true;
   }

   if(help)
   {
       if(!silent)
       {
           printf(
               "Sends a magic packet/Wake-On-LAN (WOL) packet to a network card of a computer to wake up the PC\n"
               "wol.exe <-i <\"192.168.178.255\">> <-m <\"FF:FF:FF:FF:FF:FF\">> [-m {60000}] [-h] [-s]\n"
               "Parameters:\n"
               " -i   Sets the IP address\n"
               " -p   Sets the port\n"
               " -m   Sets the MAC address\n"
               " -h   Shows this help\n"
               " -s   Mute output\n");
           fflush(stdout);
       }
   }
    
    return return_value;
}


/*---------------------------------------------------------------------*
 *  public:  functions
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  eof
 *---------------------------------------------------------------------*/

