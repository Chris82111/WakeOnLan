//! @file
//! @brief The wake_on_lan source file.
//! @details The description can be found in the header file


/*---------------------------------------------------------------------*
 *  private: include files
 *---------------------------------------------------------------------*/

#include "wake_on_lan.h"

#include <inttypes.h>
#include <stdbool.h>

// @brief Platform-specific includes and configuration for networking.
// @details This block handles platform-specific headers and settings required for
//          network functionality across Windows and POSIX systems.
#ifdef _WIN32

  // Windows-specific headers and setup
  #include <winsock2.h>

  #ifdef _MSC_VER
    #pragma comment(lib, "ws2_32.lib")
  #endif

#else

  // POSIX headers for Linux/macOS
  #include <arpa/inet.h>
//#include <sys/socket.h>

  #include <errno.h>
  #include <unistd.h>

#endif


/*---------------------------------------------------------------------*
 *  private: definitions
 *---------------------------------------------------------------------*/

//! @brief Function for determining the least significant byte of a number, byte 0 of a zero based index.
//! @details If the number is too large, you will always get 0.
#define GET_BYTE_0(NO) ( ((uint8_t )(NO) >> 0x00) & 0xFF ) //;

//! @brief Function for determining byte 1 of a zero based index.
//! @details If the number is too large, you will always get 0.
#define GET_BYTE_1(NO) ( ((uint16_t)(NO) >> 0x08) & 0xFF ) //;

//! @brief Function for determining byte 2 of a zero based index.
//! @details If the number is too large, you will always get 0.
#define GET_BYTE_2(NO) ( ((uint32_t)(NO) >> 0x10) & 0xFF ) //;

//! @brief Function for determining byte 3 of a zero based index.
//! @details If the number is too large, you will always get 0.
#define GET_BYTE_3(NO) ( ((uint32_t)(NO) >> 0x18) & 0xFF ) //;

//! @brief Function for determining byte 4 of a zero based index.
//! @details If the number is too large, you will always get 0.
#define GET_BYTE_4(NO) ( ((uint64_t)(NO) >> 0x20) & 0xFF ) //;

//! @brief Function for determining byte 5 of a zero based index.
//! @details If the number is too large, you will always get 0.
#define GET_BYTE_5(NO) ( ((uint64_t)(NO) >> 0x28) & 0xFF ) //;

//! @brief Function for determining byte 6 of a zero based index.
//! @details If the number is too large, you will always get 0.
#define GET_BYTE_6(NO) ( ((uint64_t)(NO) >> 0x30) & 0xFF ) //;

//! @brief Function for determining the most significant byte of a number, byte 7 of a zero based index.
//! @details If the number is too large, you will always get 0.
#define GET_BYTE_7(NO) ( ((uint64_t)(NO) >> 0x38) & 0xFF ) //;

// @brief This compile-time check ensures that `strtoumax()` can handle the full range of `uintmax_t`.
// @details If `UINTMAX_MAX` is less than `UINT64_MAX`, it means `uintmax_t` is smaller than 64 bits on this platform,
//          and `strtoumax()` will not work as expected with 64-bit values.
// @note Compilation will fail if this condition is true.
#if ( UINTMAX_MAX < UINT64_MAX )
#error The function strtoumax() can not handle the size of the datatype
#endif


/*---------------------------------------------------------------------*
 *  private: typedefs
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: variables
 *---------------------------------------------------------------------*/

//! @defgroup wake_on_lan_error_messages wake_on_lan error messages
//! @brief Predefined error and status messages for execution reporting.
//! @details These constant strings are used to provide human-readable messages
//!          corresponding to different execution outcomes.
//!          Text is accessible via the array ::wake_on_lan_errors and the corresponding index via ::wake_on_lan_errors_t.
//!
//! These strings provide user-friendly messages for different execution outcomes.
//! @{

//! @brief @ref wake_on_lan_error_messages
static const char no_error[] = "Execution successful\n";

//! @brief @ref wake_on_lan_error_messages
static const char error_1[] = "Unknown error\n";

//! @brief @ref wake_on_lan_error_messages
static const char error_2[] = "Failed to convert IP\n";

//! @brief @ref wake_on_lan_error_messages
static const char error_3[] = "Failed to parse hexadecimal MAC\n";

//! @brief @ref wake_on_lan_error_messages
static const char error_4[] = "WSAStartup failed\n";

//! @brief @ref wake_on_lan_error_messages
static const char error_5[] = "Could not find a usable version of Winsock.dll\n";

//! @brief @ref wake_on_lan_error_messages
static const char error_6[] = "Socket creation failed\n";

//! @brief @ref wake_on_lan_error_messages
static const char error_7[] = "Failed to set socket options\n";

//! @brief @ref wake_on_lan_error_messages
static const char error_8[] = "Failed to send packet\n";

//! @brief @ref wake_on_lan_error_messages
static const char error_9[] = "Failed to close socket\n";

//! @}


/*---------------------------------------------------------------------*
 *  public:  variables
 *---------------------------------------------------------------------*/

const char * wake_on_lan_errors[] =
{
    no_error,
    error_1,
    error_2,
    error_3,
    error_4,
    error_5,
    error_6,
    error_7,
    error_8,
    error_9,
    NULL
};


/*---------------------------------------------------------------------*
 *  private: function prototypes
 *---------------------------------------------------------------------*/

//! @brief Converts the string pointed to by ip, in the standard IPv4 dotted decimal notation, to an int64_t value
//! @details Please note that unlike the `inet_addr()` function, the number is not arranged in
//!          network order (bytes ordered from left to right). User the `htonl()` function to transform the number.
//!
//!          Values specified using IPv4 dotted decimal notation take one of the following forms:
//!
//!          - a.b.c.d  When four parts are specified, each shall be interpreted as a byte of data and assigned, from left to right, to the four bytes of an Internet address.
//!          - a.b.c    When a three-part address is specified, the last part shall be interpreted as a 16-bit quantity and placed in the rightmost two bytes of the network address. This makes the three-part address format convenient for specifying Class B network addresses as "128.net.host".
//!          - a.b      When a two-part address is supplied, the last part shall be interpreted as a 24-bit quantity and placed in the rightmost three bytes of the network address. This makes the two-part address format convenient for specifying Class A network addresses as "net.host".
//!          - a        When only one part is given, the value shall be stored directly in the network address without any byte rearrangement.
//!
//!          Depending on how the base parameter was selected, different number formats are supported.
//!          Base can be selected or the format is selected automatically if 0 is specified.
//!          All numbers supplied as parts in IPv4 dotted decimal notation may be decimal, octal,
//!          or hexadecimal, as specified in the ISO C standard (that is, a leading 0x or 0X implies
//!          hexadecimal; otherwise, a leading '0' implies octal; otherwise, the number is
//!          interpreted as decimal).
//!
//! @param ip IPv4 in dotted decimal notation
//! @param sizeof_ip The maximum length of the `ip` string
//! @param base Select the number format, 0 means that it is automatically selected
//! @return Upon successful completion, the function returns the internet address. Otherwise, it shall return INT64_C(-1).
static int64_t ip_cstr_to_number(const char * ip, size_t sizeof_ip, int base);

//! @brief Converts the string pointed to by mac_cstr, in the standard hex format with or without colon notation, to an int64_t value
//! @param mac_cstr MAC in the standard hex format with or without colon notation
//! @return Upon successful completion, the function returns the MAC address. Otherwise, it shall return INT64_C(-1).
static int64_t mac_cstr_to_number(const char * mac_cstr);


/*---------------------------------------------------------------------*
 *  private: functions
 *---------------------------------------------------------------------*/

static int64_t ip_cstr_to_number(const char * ip, size_t sizeof_ip, int base)
{
    int64_t no = INT64_C(-1);
    uint32_t ip_octets[] = { 0, 0, 0, 0 };
    uint8_t octet = 0;

    char * endptr;
    char c;
    for (size_t i = 0; (i < sizeof_ip) && ((c = ip[i]) != '\0') && (octet < 4); i++)
    {
        if('0' <= c && '9' >= c )
        {
            ip_octets[octet] = strtoumax(ip+i, &endptr, base);
            octet += 1;
            i = endptr - ip;
            if('.' != ip[i])
            {
                break;
            }
        }
    }

    switch(octet)
    {
        case 4: no = (ip_octets[0] << 24) + (ip_octets[1] << 16) + (ip_octets[2] << 8) + (ip_octets[3]); break;
        case 3: no = (ip_octets[0] << 24) + (ip_octets[1] << 16) + (ip_octets[2]); break;
        case 2: no = (ip_octets[0] << 24) + (ip_octets[1]); break;
        case 1: no = (ip_octets[0]); break;
        default: break;
    }

    return no;
}

static int64_t mac_cstr_to_number(const char * mac_cstr)
{
    int64_t no = 0;
    uint8_t hex_letters = 0;

    char c;
    for(size_t i = 0; (i < 17) && (hex_letters < 12) && ((c = mac_cstr[i]) != '\0'); i++)
    {
        if ('0' <= c && '9' >= c)
        {
            no = ( no << 4 ) + (c - '0');
            hex_letters++;
        }
        else if('A' <= c && 'F' >= c)
        {
            no = (no << 4) + (c + 10 - 'A');
            hex_letters++;
        }
        else if ('a' <= c && 'f' >= c)
        {
            no = (no << 4) + (c + 10 - 'a');
            hex_letters++;
        }
    }

    if (12 != hex_letters)
    {
        return INT64_C(-1);
    }
    else
    {
        return no;
    }
}

#if false

typedef struct hex_cstr_s
{
    char cstr[4];
} hex_cstr_t;

static const char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

static bool number_2_mac_cstr(int64_t number, char * mac_cstr, uint8_t sizeof_mac_cstr);

static bool number_2_mac_cstr(int64_t number, char * mac_cstr, uint8_t sizeof_mac_cstr)
{
    int8_t i = 0;

    if(NULL == mac_cstr)
    {
        return false;
    }

    if(INT64_C(-1) == number || 13 > sizeof_mac_cstr)
    {
        for(i = 0; i < 13; i++)
        {
            mac_cstr[i] = '\0';
        }

        return false;
    }

    for(i = 11; i >= 0; i--)
    {
        mac_cstr[i] = hex[number & 0x0F];
        number >>= 4;
    }
    mac_cstr[12] = '\0';

    return true;
}

// the low byte contains the high nibble hex ascii character of the number
// the high byte contains the low nibble hex ascii character of the number
hex_cstr_t number_2_hex_cstr_t(uint8_t number)
{
    hex_cstr_t hex_cstr;
    hex_cstr.cstr[0] = hex[( number >> 4 ) & 0x0F];
    hex_cstr.cstr[1] = hex[( number >> 0 ) & 0x0F];
    hex_cstr.cstr[2] = '\0';
    hex_cstr.cstr[3] = number;
    return hex_cstr;
}

#endif


/*---------------------------------------------------------------------*
 *  public:  functions
 *---------------------------------------------------------------------*/

wake_on_lan_errors_t wake_on_lan(wake_on_lan_t * wol, const char * ip_v4_cstr, uint16_t port, const char * mac_cstr)
{
    wake_on_lan_errors_t return_value = WAKE_ON_LAN_ERRORS_UNKNOWN;

    uint8_t data[6 * ( 1 + 16 )];
    data[0] = 0xFF;
    data[1] = 0xFF;
    data[2] = 0xFF;
    data[3] = 0xFF;
    data[4] = 0xFF;
    data[5] = 0xFF;


#ifdef _WIN32
    SOCKET sockfd = INVALID_SOCKET;
#else
    int sockfd = -1;
#endif

    do{

        int64_t ip_v4;
        ip_v4 = ip_cstr_to_number(ip_v4_cstr, 13, 0);
        if(INT64_C(-1) == ip_v4)
        {
            return_value = WAKE_ON_LAN_ERRORS_IP;
            if(wol) { wol->last_error = -1; }
            break;
        }
        if(wol) { wol->ip_v4 = (uint32_t)ip_v4; }

        int64_t mac = mac_cstr_to_number(mac_cstr);
        if(INT64_C(-1) == mac)
        {
            return_value = WAKE_ON_LAN_ERRORS_MAC;
            if(wol) { wol->last_error = -1; }
            break;
        }
        if(wol) { wol->mac = mac; }


#ifdef _WIN32
        // Initialize Winsock
        WSADATA wsaData;

        // Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
        // The highest version of Windows Sockets specification that the caller can use.
        // The high-order byte specifies the minor version number; the low-order byte specifies the major version number.
        uint8_t major = 2;
        uint8_t minor = 2;
        WORD wVersionRequested = MAKEWORD(major, minor);


        int wsa_startup_result = WSAStartup(wVersionRequested, &wsaData);
        if (0 != wsa_startup_result) {
            return_value = WAKE_ON_LAN_ERRORS_WSA_STARTUP;
            if(wol) { wol->last_error = WSAGetLastError(); }
            break;
        }

        // Confirm that the WinSock DLL supports 2.2.
        // Note that if the DLL supports versions greater than 2.2
        // in addition to 2.2, it will still return 2.2 in wVersion
        // since that is the version we requested.
        uint8_t available_major = LOBYTE(wsaData.wVersion);
        uint8_t available_minor = HIBYTE(wsaData.wVersion);
        if ( available_major != major || available_minor != minor)
        {
            return_value = WAKE_ON_LAN_ERRORS_WINSOCK;
            if(wol) { wol->last_error = available_major << 8 | available_minor; }
            break;
        }
#endif


        // AF_INET    : The Internet Protocol version 4 (IPv4) address family.
        // SOCK_DGRAM : A socket type that supports datagrams, which are connectionless,
        //              unreliable buffers of a fixed (typically small) maximum length.
        //              This socket type uses the User Datagram Protocol (UDP) for the
        //              Internet address family (AF_INET or AF_INET6).
        // IPPROTO_UDP: The User Datagram Protocol (UDP). This is a possible value when
        //              the af parameter is AF_INET or AF_INET6 and the type parameter
        //              is SOCK_DGRAM.
        sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

#ifdef _WIN32
        if (INVALID_SOCKET == sockfd)
        {
            return_value = WAKE_ON_LAN_ERRORS_SOCKET_CREATION;
            if(wol) { wol->last_error = WSAGetLastError(); }
            break;
        }
#else
        if (0 > sockfd)
        {
            return_value = WAKE_ON_LAN_ERRORS_SOCKET_CREATION;
            if(wol) { wol->last_error = errno; }
            break;
        }
#endif



        char mac5 = GET_BYTE_5(mac);
        char mac4 = GET_BYTE_4(mac);
        char mac3 = GET_BYTE_3(mac);
        char mac2 = GET_BYTE_2(mac);
        char mac1 = GET_BYTE_1(mac);
        char mac0 = GET_BYTE_0(mac);

        char index;
        for(size_t i = 1; i <= 16; i++)
        {
            index = i * 6;
            data[index+0] = mac5;
            data[index+1] = mac4;
            data[index+2] = mac3;
            data[index+3] = mac2;
            data[index+4] = mac1;
            data[index+5] = mac0;
        }

        // Set socket options.
        const int optval = 1;

#ifdef _WIN32
        if (SOCKET_ERROR == setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (const char *)(&optval), sizeof(optval)))
        {
            return_value = WAKE_ON_LAN_ERRORS_SOCKET_OPTION;
            if(wol) { wol->last_error = WSAGetLastError(); }
            break;
        }
#else
        if (0 > setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (const char *)(&optval), sizeof(optval)))
        {
            return_value = WAKE_ON_LAN_ERRORS_SOCKET_OPTION;
            if(wol) { wol->last_error = errno; }
            break;
        }
#endif


#ifdef _WIN32
        int sendto_result = SOCKET_ERROR;
#else
        int sendto_result = -1;
#endif

        struct sockaddr_in addr = { 0 };
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(ip_v4);
        addr.sin_port = htons(port);

        size_t data_length = sizeof(data);
        sendto_result = sendto(sockfd, (const char *) data, data_length, 0,
            (struct sockaddr*)(&addr), sizeof(addr));

#ifdef _WIN32
        if (SOCKET_ERROR == sendto_result) {
            return_value = WAKE_ON_LAN_ERRORS_SEND;
            if(wol) { wol->last_error = WSAGetLastError(); }
            break;
        }
#else
        if (0 > sendto_result) {
            return_value = WAKE_ON_LAN_ERRORS_SEND;
            if(wol) { wol->last_error = sendto_result; }
            break;
        }
#endif

        return_value = WAKE_ON_LAN_ERRORS_NONE;

    }while(0);


#ifdef _WIN32
    if(INVALID_SOCKET != sockfd)
    {
        int closesocket_result = closesocket(sockfd);
        if (SOCKET_ERROR == closesocket_result) {
            return_value = WAKE_ON_LAN_ERRORS_SOCKET_CLOSE;
            if(wol) { wol->last_error = WSAGetLastError(); }
        }
    }
    WSACleanup();
#else
    close(sockfd);
#endif


    if(wol) { wol->return_value = return_value; }

    return return_value;
}


/*---------------------------------------------------------------------*
 *  eof
 *---------------------------------------------------------------------*/
