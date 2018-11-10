/**
 * This file is part of netlib
 * which is licensed under the zlib license
 *
 * github.com/univrsal/netlib
 * netlib is a small network library
 * heavily based on SDL_net without
 * the dependency on SDL
 *
 * Documentation is directly taken from SDL_net
 * I take no credit for this code, I only
 * reformatted it for my needs and made slight changes
 */

#ifndef NETLIB_H
#define NETLIB_H

#include "util.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct netlib_version
{
	uint8_t major;
	uint8_t minor;
	uint8_t patch;
} netlib_version;

/* Printable format: "%d.%d.%d", MAJOR, MINOR, PATCHLEVEL */
#define NETLIB_MAJOR_VERSION   0
#define NETLIB_MINOR_VERSION   2
#define NETLIB_PATHLEVEL	   0

/* This macro can be used to fill a version structure with the compile-time
   version of the netlib library.
*/
#define NETLIB_VERSION(X)                          \
{                                                  \
    (X)->major = NETLIB_MAJOR_VERSION;             \
    (X)->minor = NETLIB_MINOR_VERSION;             \
    (X)->patch = NETLIB_PATHLEVEL;                 \
}

/* This function gets the version of the dynamically linked netlib library.
   it should NOT be used to fill a version structure, instead you should
   use the NETLIB_VERSION() macro.
*/
extern DECLSPEC const netlib_version* NETLIB_CALL netlib_get_version(void);

/* Setup and close method. Call before other usage */
extern DECLSPEC int  NETLIB_CALL netlib_init(void);
extern DECLSPEC void NETLIB_CALL netlib_quit(void);

typedef struct
{
	uint32_t host;            /* 32-bit IPv4 host address */
	uint16_t port;            /* 16-bit protocol port */
} ip_address;

#ifndef INADDR_ANY
#define INADDR_ANY			0x00000000
#endif
#ifndef INADDR_NONE
#define INADDR_NONE			0xFFFFFFFF
#endif
#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK		0x7f000001
#endif
#ifndef INADDR_BROADCAST
#define INADDR_BROADCAST	0xFFFFFFFF
#endif

/* Resolve a host name and port to an IP address in network form.
   If the function succeeds, it will return 0.
   If the host couldn't be resolved, the host portion of the returned
   address will be INADDR_NONE, and the function will return -1.
   If 'host' is NULL, the resolved host will be set to INADDR_ANY.
*/
extern DECLSPEC int NETLIB_CALL netlib_resolve_host(ip_address* address, const char* host, uint16_t port);

/* Resolve an ip address to a host name in canonical form.
   If the ip couldn't be resolved, this function returns NULL,
   otherwise a pointer to a static data containing the hostname
   is returned.  Note that this function is not thread-safe.
*/
extern DECLSPEC const char * NETLIB_CALL netlib_resolve_ip(const ip_address* ip);

/* Get the addresses of network interfaces on this system.
   This returns the number of addresses saved in 'addresses'
*/
extern DECLSPEC int NETLIB_CALL netlib_get_local_addresses(ip_address* addresses, int maxcount);

/* === TCP API === */

typedef struct _tcp_socket *tcp_socket;

/* Open a TCP network socket
   If ip.host is INADDR_NONE or INADDR_ANY, this creates a local server
   socket on the given port, otherwise a TCP connection to the remote
   host and port is attempted. The address passed in should already be
   swapped to network byte order (addresses returned from
   netlib_resolve_host() are already in the correct form).
   The newly created socket is returned, or NULL if there was an error.
*/
extern DECLSPEC tcp_socket NETLIB_CALL netlib_tcp_open(ip_address* ip);

/* Accept an incoming connection on the given server socket.
   The newly created socket is returned, or NULL if there was an error.
*/
extern DECLSPEC tcp_socket NETLIB_CALL netlib_tcp_accept(tcp_socket server);

/* Get the IP address of the remote system associated with the socket.
   If the socket is a server socket, this function returns NULL.
*/
extern DECLSPEC ip_address* NETLIB_CALL netlib_tcp_get_peer_address(tcp_socket sock);

/* Send 'len' bytes of 'data' over the non-server socket 'sock'
   This function returns the actual amount of data sent.  If the return value
   is less than the amount of data sent, then either the remote connection was
   closed, or an unknown socket error occurred.
*/
extern DECLSPEC int NETLIB_CALL netlib_tcp_send(tcp_socket sock, const void* data, int len);

/* Receive up to 'maxlen' bytes of data over the non-server socket 'sock',
   and store them in the data pointed to by 'data'.
   This function returns the actual amount of data received.  If the return
   value is less than or equal to zero, then either the remote connection was
   closed, or an unknown socket error occurred.
*/
extern DECLSPEC int NETLIB_CALL netlib_tcp_recv(tcp_socket sock, void* data, int maxlen);

/* Close a TCP network socket */
extern DECLSPEC void NETLIB_CALL netlib_tcp_close(tcp_socket sock);

/* === UDP API === */

/* The maximum channels on a a UDP socket */
#define NETLIB_MAX_UDPCHANNELS  32
/* The maximum addresses bound to a single UDP socket channel */
#define NETLIB_MAX_UDPADDRESSES 4

typedef struct _udp_socket* udp_socket;

typedef struct 
{
	int channel;        /* The src/dst channel of the packet */
	uint8_t *data;      /* The packet data */
	int len;            /* The length of the packet data */
	int maxlen;         /* The length of the data data */
	int status;         /* packet status after sending */
	ip_address address; /* The source/dest address of an incoming/outgoing packet */
} udp_packet;

/* Allocate/resize/free a single UDP packet 'length' bytes long.
   The new packet is returned, or NULL if the function ran out of memory.
*/
extern DECLSPEC udp_packet* NETLIB_CALL netlib_alloc_packet(int size);
extern DECLSPEC int NETLIB_CALL netlib_resize_packet(udp_packet* packet, int newsize);
extern DECLSPEC void NETLIB_CALL netlib_free_packet(udp_packet* packet);

/* Allocate/Free a UDP packet vector (array of packets) of 'howmany' packets,
   each 'length' bytes long.
   A pointer to the first packet in the array is returned, or NULL if the
   function ran out of memory.
*/
extern DECLSPEC udp_packet ** NETLIB_CALL netlib_alloc_packets(int howmany, int size);
extern DECLSPEC void NETLIB_CALL netlib_free_packets(udp_packet** packets);

/* Open a UDP network socket
   If 'port' is non-zero, the UDP socket is bound to a local port.
   The 'port' should be given in native byte order, but is used
   internally in network (big endian) byte order, in addresses, etc.
   This allows other systems to send to this socket via a known port.
*/
extern DECLSPEC udp_socket NETLIB_CALL netlib_udp_open(uint16_t port);

/* Set the percentage of simulated packet loss for packets sent on the socket. */
extern DECLSPEC void NETLIB_CALL netlib_udp_set_packet_loss(udp_socket sock, int percent);

/* Bind the address 'address' to the requested channel on the UDP socket.
   If the channel is -1, then the first unbound channel that has not yet
   been bound to the maximum number of addresses will be bound with
   the given address as it's primary address.
   If the channel is already bound, this new address will be added to the
   list of valid source addresses for packets arriving on the channel.
   If the channel is not already bound, then the address becomes the primary
   address, to which all outbound packets on the channel are sent.
   This function returns the channel which was bound, or -1 on error.
*/
extern DECLSPEC int NETLIB_CALL netlib_udp_bind(udp_socket sock, int channel, const ip_address* address);

/* Unbind all addresses from the given channel */
extern DECLSPEC void NETLIB_CALL netlib_udp_unbind(udp_socket sock, int channel);

/* Get the primary IP address of the remote system associated with the
   socket and channel.  If the channel is -1, then the primary IP port
   of the UDP socket is returned -- this is only meaningful for sockets
   opened with a specific port.
   If the channel is not bound and not -1, this function returns NULL.
*/
extern DECLSPEC ip_address* NETLIB_CALL netlib_udp_get_peer_address(udp_socket sock, int channel);

/* Send a vector of packets to the the channels specified within the packet.
   If the channel specified in the packet is -1, the packet will be sent to
   the address in the 'src' member of the packet.
   Each packet will be updated with the status of the packet after it has
   been sent, -1 if the packet send failed.
   This function returns the number of packets sent.
*/
extern DECLSPEC int NETLIB_CALL netlib_udp_send_packets(udp_socket sock, udp_packet** packets, int npackets);

/* Send a single packet to the specified channel.
   If the channel specified in the packet is -1, the packet will be sent to
   the address in the 'src' member of the packet.
   The packet will be updated with the status of the packet after it has
   been sent.
   This function returns 1 if the packet was sent, or 0 on error.
   
   NOTE:
   The maximum length of the packet is limited by the MTU (Maximum Transfer Unit)
   of the transport medium.  It can be as low as 250 bytes for some PPP links,
   and as high as 1500 bytes for ethernet.
*/
extern DECLSPEC int NETLIB_CALL netlib_udp_send(udp_socket sock, int channel, udp_packet* packet);

/* Receive a vector of pending packets from the UDP socket.
   The returned packets contain the source address and the channel they arrived
   on.  If they did not arrive on a bound channel, the the channel will be set
   to -1.
   The channels are checked in highest to lowest order, so if an address is
   bound to multiple channels, the highest channel with the source address
   bound will be returned.
   This function returns the number of packets read from the network, or -1
   on error.  This function does not block, so can return 0 packets pending.
*/
extern DECLSPEC int NETLIB_CALL netlib_udp_recv_packets(udp_socket sock, udp_packet** packets);

/* Receive a single packet from the UDP socket.
   The returned packet contains the source address and the channel it arrived
   on.  If it did not arrive on a bound channel, the the channel will be set
   to -1.
   The channels are checked in highest to lowest order, so if an address is
   bound to multiple channels, the highest channel with the source address
   bound will be returned.
   This function returns the number of packets read from the network, or -1
   on error.  This function does not block, so can return 0 packets pending.
*/
extern DECLSPEC int NETLIB_CALL netlib_udp_recv(udp_socket sock, udp_packet* packet);

/* Close a UDP network socket */
extern DECLSPEC void NETLIB_CALL netlib_udp_close(udp_socket sock);

/* === Hooks === */

typedef struct _netlib_socket_set* netlib_socket_set;

/* Any network socket can be safely cast to this socket type */
typedef struct _netlib_generic_socket
{
	int ready;
}* netlib_generic_socket;

/* Allocate a socket set for use with netlib_check_socket_set()
   This returns a socket set for up to 'maxsockets' sockets, or NULL if
   the function ran out of memory.
*/
extern DECLSPEC netlib_socket_set NETLIB_CALL netlib_alloc_socket_set(int maxsockets);

/* Add a socket to a set of sockets to be checked for available data */
extern DECLSPEC int NETLIB_CALL netlib_add_socket(netlib_socket_set set, netlib_generic_socket sock);

FORCE_INLINE int netlib_tcp_add_socket(netlib_socket_set set, tcp_socket sock)
{
	return netlib_add_socket(set, (netlib_generic_socket)sock);
}

FORCE_INLINE int netlib_udp_add_socket(netlib_socket_set set, udp_socket sock)
{
	return netlib_add_socket(set, (netlib_generic_socket)sock);
}

/* Remove a socket from a set of sockets to be checked for available data */
extern DECLSPEC int NETLIB_CALL netlib_del_socket(netlib_socket_set set, netlib_generic_socket sock);

FORCE_INLINE int netlib_tcp_del_socket(netlib_socket_set set, tcp_socket sock)
{
	return netlib_del_socket(set, (netlib_generic_socket)sock);
}

FORCE_INLINE int netlib_udp_del_socket(netlib_socket_set set, udp_socket sock)
{
	return netlib_del_socket(set, (netlib_generic_socket)sock);
}

/* This function checks to see if data is available for reading on the
   given set of sockets.  If 'timeout' is 0, it performs a quick poll,
   otherwise the function returns when either data is available for
   reading, or the timeout in milliseconds has elapsed, which ever occurs
   first.  This function returns the number of sockets ready for reading,
   or -1 if there was an error with the select() system call.
*/
extern DECLSPEC int NETLIB_CALL netlib_check_socket_set(netlib_socket_set set, uint32_t timeout);

/* After calling netlib_check_socket_set(), you can use this function on a
   socket that was in the socket set, to find out if data is available
   for reading.
*/

#define netlib_socket_ready(sock) _netlib_socket_ready((netlib_generic_socket)(sock))

FORCE_INLINE int _netlib_socket_ready(netlib_generic_socket sock)
{
	return (sock != NULL) && (sock->ready);
}

/* Free a set of sockets allocated by netlib_alloc_socket_set() */
extern DECLSPEC void NETLIB_CALL netlib_free_socket_set(netlib_socket_set set);

/* === Error reporting === */

extern DECLSPEC void NETLIB_CALL netlib_set_error(const char *fmt, ...);
extern DECLSPEC const char* NETLIB_CALL netlib_get_error(void);

/* === Functions to read/write network data */

/* Write a 16/32-bit value to network packet data */
#define netlib_write16(value, areap) _netlib_write16(value, areap)
#define netlib_write32(value, areap) _netlib_write32(value, areap)

/* Read a 16/32-bit value from network packet data */
#define netlib_read16(areap) _netlib_read16(areap)
#define netlib_read32(areap) _netlib_read32(areap)

FORCE_INLINE void _netlib_write16(uint16_t value, void* areap)
{
	uint8_t *area = (uint8_t*)areap;
	area[0] = (value >> 8) & 0xFF;
	area[1] = value & 0xFF;
}

FORCE_INLINE void _netlib_write32(uint32_t value, void* areap)
{
	uint8_t *area = (uint8_t*)areap;
	area[0] = (value >> 24) & 0xFF;
	area[1] = (value >> 16) & 0xFF;
	area[2] = (value >> 8) & 0xFF;
	area[3] = value & 0xFF;
}

FORCE_INLINE uint16_t _netlib_read16(void* areap)
{
	uint8_t *area = (uint8_t*)areap;
	return ((uint16_t)area[0]) << 8 | ((uint16_t)area[1]);
}

FORCE_INLINE uint32_t _netlib_read32(const void *areap)
{
	const uint8_t *area = (const uint8_t*)areap;
	return ((uint32_t)area[0]) << 24 | ((uint32_t)area[1]) << 16 | ((uint32_t)area[2]) << 8 | ((uint32_t)area[3]);
}

FORCE_INLINE uint32_t netlib_swap_BE32(uint32_t val)
{
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val << 16) | (val >> 16);
}

FORCE_INLINE uint16_t netlib_swap_BE16(uint16_t val)
{
	return (val >> 8) | (val << 8);
}

/* === Buffer API (This is not part of SDL_net) === */

typedef struct
{
	uint8_t* data;
	uint8_t length;
	uint8_t read_pos;
	uint8_t write_pos;
} netlib_byte_buf;

extern DECLSPEC netlib_byte_buf* NETLIB_CALL netlib_alloc_byte_buf(uint8_t size);

extern DECLSPEC void NETLIB_CALL netlib_free_byte_buf(netlib_byte_buf* buf);

extern DECLSPEC int NETLIB_CALL netlib_tcp_send_buf(tcp_socket sock, netlib_byte_buf* buf);

/* Only sends bytes up to write_pos */
extern DECLSPEC int NETLIB_CALL netlib_tcp_send_buf_smart(tcp_socket sock, netlib_byte_buf* buf);

/* Returns amount of bytes received, might be lesser than length of buffer
 * if netlib_tcp_send_buf_smart was used
 */
extern DECLSPEC int NETLIB_CALL netlib_tcp_recv_buf(tcp_socket sock, netlib_byte_buf* buf);

/* === Writing to a data === */

extern DECLSPEC int NETLIB_CALL netlib_write_uint8(netlib_byte_buf* buf, uint8_t val);

extern DECLSPEC int NETLIB_CALL netlib_write_uint16(netlib_byte_buf* buf, uint16_t val);

extern DECLSPEC int NETLIB_CALL netlib_write_uint32(netlib_byte_buf* buf, uint32_t val);

#define netlib_write_int8(buf, val) netlib_write_uint8(buf, (uint8_t) val)

#define netlib_write_int16(buf, val) netlib_write_uint16(buf, (uint16_t) val)

#define netlib_write_int32(buf, val) netlib_write_uint32(buf, (uint32_t) val)

extern DECLSPEC int NETLIB_CALL netlib_write_float(netlib_byte_buf* buf, float val);

/* === Reading from a data === */

extern DECLSPEC int NETLIB_CALL netlib_read_uint8(netlib_byte_buf* buf, uint8_t* val);

extern DECLSPEC int NETLIB_CALL netlib_read_uint16(netlib_byte_buf* buf, uint16_t* val);

extern DECLSPEC int NETLIB_CALL netlib_read_uint32(netlib_byte_buf* buf, uint32_t* val);

#define netlib_read_int8(buf, val) netlib_read_uint8(buf, (uint8_t*) val)

#define netlib_read_int16(buf, val) netlib_read_uint16(buf, (uint16_t*) val)

#define netlib_read_int32(buf, val) netlib_read_uint32(buf, (uint32_t*) val)

extern DECLSPEC int NETLIB_CALL netlib_read_float(netlib_byte_buf* buf, float* val);
#ifdef __cplusplus
}
#endif

#endif