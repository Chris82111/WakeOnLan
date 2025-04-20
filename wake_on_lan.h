//! @file
//! @brief The wake_on_lan header file.
//! @details The module can be used in C and C++ under Windows and Linux
//!
//! A simple module for sending a magic packet/Wake-On-LAN (WOL) packet
//! to a network card of a computer to wake up the PC.
//!
//! @note Under Windows, the file must be linked with the `-lws2_32` switch.

#ifndef INC_WAKE_ON_LAN_H_
#define INC_WAKE_ON_LAN_H_


#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------*
 *  public: include files
 *---------------------------------------------------------------------*/

#include <stdint.h>


/*---------------------------------------------------------------------*
 *  public: define
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  public: typedefs
 *---------------------------------------------------------------------*/

//! @brief Return values of the ::wake_on_lan() function
//! @details The error value can be converted into a string using the array ::wake_on_lan_errors.
typedef enum wake_on_lan_errors_e
{
    WAKE_ON_LAN_ERRORS_NONE = 0,        //!< No errors
    WAKE_ON_LAN_ERRORS_UNKNOWN,         //!< Unknown error
    WAKE_ON_LAN_ERRORS_IP,              //!< Failed to convert IP
    WAKE_ON_LAN_ERRORS_MAC,             //!< Failed to convert MAC
    WAKE_ON_LAN_ERRORS_WSA_STARTUP,     //!< Windows only, the value of `WSAGetLastError()` is stored in ::wake_on_lan_s::last_error
    WAKE_ON_LAN_ERRORS_WINSOCK,         //!< Windows only, ::wake_on_lan_s::last_error contains the main version shifted 8-fold to the left and the added secondary version
    WAKE_ON_LAN_ERRORS_SOCKET_CREATION, //!< The value of `WSAGetLastError()`/`errno` is stored in ::wake_on_lan_s::last_error
    WAKE_ON_LAN_ERRORS_SOCKET_OPTION,   //!< The value of `WSAGetLastError()`/`errno` is stored in ::wake_on_lan_s::last_error
    WAKE_ON_LAN_ERRORS_SEND,            //!< The value of `WSAGetLastError()`/`sendto()` is stored in ::wake_on_lan_s::last_error
    WAKE_ON_LAN_ERRORS_SOCKET_CLOSE,    //!< Windows only, the value of `WSAGetLastError()` is stored in ::wake_on_lan_s::last_error
}wake_on_lan_errors_t;

//! @brief Structure to get more information about the ::wake_on_lan() function
typedef struct wake_on_lan_s
{
    uint32_t ip_v4;                     //!< IP v4 address as number
    int64_t mac;                        //!< MAC address as number
    wake_on_lan_errors_t return_value;  //!< Return value of the ::wake_on_lan() function
    int  last_error;                    //!< Value of `WSAGetLastError()`/`errno` check enum ::wake_on_lan_errors_e of wake_on_lan_t::return_value
} wake_on_lan_t;


/*---------------------------------------------------------------------*
 *  public: extern variables
 *---------------------------------------------------------------------*/

//! @brief Array for conversion the enum error ::wake_on_lan_errors_e to a character string
//! @details The text is defined here @ref wake_on_lan_error_messages
extern const char * wake_on_lan_errors[];


/*---------------------------------------------------------------------*
 *  public: function prototypes
 *---------------------------------------------------------------------*/

//! @brief Sends a magic packet/Wake-On-LAN (WOL) packet to a network card of a computer to wake up the PC
//! @param[out] wol Pointer to the structure ::wake_on_lan_t to get more information about the execution, can be NULL if not necessary
//! @param ip_v4_cstr IPv4 in dotted decimal notation
//! @param port Port number
//! @param mac_cstr MAC in the standard hex format with or without colon notation
//! @return The return value is defined by the enum ::wake_on_lan_errors_e
wake_on_lan_errors_t wake_on_lan(wake_on_lan_t * wol, const char * ip_v4_cstr, uint16_t port, const char * mac_cstr);


/*---------------------------------------------------------------------*
 *  public: static inline functions
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  eof
 *---------------------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* INC_WAKE_ON_LAN_H_ */
