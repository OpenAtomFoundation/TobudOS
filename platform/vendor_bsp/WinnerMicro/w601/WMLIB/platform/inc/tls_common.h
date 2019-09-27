#ifndef TLS_COMMON_H
#define TLS_COMMON_H

#include <stddef.h>
#include "wm_type_def.h"
#include "wm_config.h"

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

#define BIT(x) (1UL << (x))

#define time_after(a, b)  ((long)(b) - (long)(a) < 0)
#define time_before(a,b)	time_after(b,a)

/*
 *	These are the defined Ethernet Protocol ID's.
 */

#define ETH_P_LOOP	0x0060		/* Ethernet Loopback packet	*/
#define ETH_P_PUP	0x0200		/* Xerox PUP packet		*/
#define ETH_P_PUPAT	0x0201		/* Xerox PUP Addr Trans packet	*/
#define ETH_P_IP	0x0800		/* Internet Protocol packet	*/
#define ETH_P_X25	0x0805		/* CCITT X.25			*/
#define ETH_P_ARP	0x0806		/* Address Resolution packet	*/
#define	ETH_P_BPQ	0x08FF		/* G8BPQ AX.25 Ethernet Packet	[ NOT AN OFFICIALLY REGISTERED ID ] */
#define ETH_P_IEEEPUP	0x0a00		/* Xerox IEEE802.3 PUP packet */
#define ETH_P_IEEEPUPAT	0x0a01		/* Xerox IEEE802.3 PUP Addr Trans packet */
#define ETH_P_DEC       0x6000          /* DEC Assigned proto           */
#define ETH_P_DNA_DL    0x6001          /* DEC DNA Dump/Load            */
#define ETH_P_DNA_RC    0x6002          /* DEC DNA Remote Console       */
#define ETH_P_DNA_RT    0x6003          /* DEC DNA Routing              */
#define ETH_P_LAT       0x6004          /* DEC LAT                      */
#define ETH_P_DIAG      0x6005          /* DEC Diagnostics              */
#define ETH_P_CUST      0x6006          /* DEC Customer use             */
#define ETH_P_SCA       0x6007          /* DEC Systems Comms Arch       */
#define ETH_P_TEB	0x6558		/* Trans Ether Bridging		*/
#define ETH_P_RARP      0x8035		/* Reverse Addr Res packet	*/
#define ETH_P_ATALK	0x809B		/* Appletalk DDP		*/
#define ETH_P_AARP	0x80F3		/* Appletalk AARP		*/
#define ETH_P_8021Q	0x8100          /* 802.1Q VLAN Extended Header  */
#define ETH_P_IPX	0x8137		/* IPX over DIX			*/
#define ETH_P_IPV6	0x86DD		/* IPv6 over bluebook		*/
#define ETH_P_PAUSE	0x8808		/* IEEE Pause frames. See 802.3 31B */
#define ETH_P_SLOW	0x8809		/* Slow Protocol. See 802.3ad 43B */
#define ETH_P_WCCP	0x883E		/* Web-cache coordination protocol
					 * defined in draft-wilson-wrec-wccp-v2-00.txt */
#define ETH_P_PPP_DISC	0x8863		/* PPPoE discovery messages     */
#define ETH_P_PPP_SES	0x8864		/* PPPoE session messages	*/
#define ETH_P_MPLS_UC	0x8847		/* MPLS Unicast traffic		*/
#define ETH_P_MPLS_MC	0x8848		/* MPLS Multicast traffic	*/
#define ETH_P_ATMMPOA	0x884c		/* MultiProtocol Over ATM	*/
#define ETH_P_ATMFATE	0x8884		/* Frame-based ATM Transport
					 * over Ethernet
					 */
#define ETH_P_PAE	0x888E		/* Port Access Entity (IEEE 802.1X) */
#define ETH_P_WAPI	0x88B4		/* WAPI authentication */
#define ETH_P_AOE	0x88A2		/* ATA over Ethernet		*/
#define ETH_P_TIPC	0x88CA		/* TIPC 			*/
#define ETH_P_FCOE	0x8906		/* Fibre Channel over Ethernet  */
#define ETH_P_EDSA	0xDADA		/* Ethertype DSA [ NOT AN OFFICIALLY REGISTERED ID ]  */

#define ETH_HLEN	14		/* Total octets in header.	 */

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif
#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif
#ifndef ETH_P_ALL
#define ETH_P_ALL 0x0003
#endif
#ifndef ETH_P_80211_ENCAP
#define ETH_P_80211_ENCAP 0x890d /* TDLS comes under this category */
#endif
#ifndef ETH_P_PAE
#define ETH_P_PAE 0x888E /* Port Access Entity (IEEE 802.1X) */
#endif /* ETH_P_PAE */
#ifndef ETH_P_EAPOL
#define ETH_P_EAPOL ETH_P_PAE
#endif /* ETH_P_EAPOL */
#ifndef ETH_P_RSN_PREAUTH
#define ETH_P_RSN_PREAUTH 0x88c7
#endif /* ETH_P_RSN_PREAUTH */
#ifndef ETH_P_RRB
#define ETH_P_RRB 0x890D
#endif /* ETH_P_RRB */

#define PRINTF_FORMAT(a,b) __attribute__ ((format (printf, (a), (b))))
#define STRUCT_PACKED __attribute__ ((packed))

#define broadcast_ether_addr (const u8 *) "\xff\xff\xff\xff\xff\xff"

/**
 * compare_ether_addr - Compare two Ethernet addresses
 * @addr1: Pointer to a six-byte array containing the Ethernet address
 * @addr2: Pointer other six-byte array containing the Ethernet address
 *
 * Compare two ethernet addresses, returns 0 if equal
 */
static __inline unsigned compare_ether_addr(const u8 *addr1, const u8 *addr2)
{
	return !((addr1[0] == addr2[0]) && (addr1[1] == addr2[1]) && (addr1[2] == addr2[2]) &&   \
		(addr1[3] == addr2[3]) && (addr1[4] == addr2[4]) && (addr1[5] == addr2[5]));
}

static __inline int is_zero_ether_addr(const u8 *a)
{
	return !(a[0] | a[1] | a[2] | a[3] | a[4] | a[5]);
}

static __inline int is_broadcast_ether_addr(const u8 *a)
{
	return (a[0] & a[1] & a[2] & a[3] & a[4] & a[5]) == 0xff;
}

static __inline int is_multicast_ether_addr(const u8 *addr)
{
	return (0x01 & addr[0]);
}

static __inline unsigned short swap_16(unsigned short v)
{
	return ((v & 0xff) << 8) | (v >> 8);
}

static __inline unsigned int swap_32(unsigned int v)
{
	return ((v & 0xff) << 24) | ((v & 0xff00) << 8) |
		((v & 0xff0000) >> 8) | (v >> 24);
}

#define le_to_host16(n) (n)
#define host_to_le16(n) (n)
#define be_to_host16(n) swap_16(n)
#define host_to_be16(n) swap_16(n)
#define le_to_host32(n) (n)
#define le_to_host64(n) (n)
#define be_to_host32(n) swap_32(n)
#define host_to_be32(n) swap_32(n)

static __inline u16 get_unaligned_le16(const u8 *p)
{
	return p[0] | p[1] << 8;    
}

static __inline u32 get_unaligned_le32(const u8 *p)
{
	return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;    
}

static __inline void put_unaligned_le16(u16 val, u8 *p)
{
	*p++ = val;
	*p++ = val >> 8; 
}

static __inline void put_unaligned_le32(u32 val, u8 *p)
{
	put_unaligned_le16(val >> 16, p + 2);
	put_unaligned_le16(val, p); 
}

static __inline u16 get_unaligned_be16(const u8 *p)
{
	return be_to_host16(get_unaligned_le16(p));
}

static __inline u32 get_unaligned_be32(const u8 *p)
{
	return be_to_host32(get_unaligned_le32(p));
}

static __inline void put_unaligned_be16(u16 val, u8 *p)
{
	*p++ = val >> 8;
	*p++ = val;
}

static __inline void put_unaligned_be32(u32 val, u8 *p)
{
	put_unaligned_be16(val >> 16, p);
	put_unaligned_be16(val, p + 2); 
}


#ifdef __CHECKER__
#define __force __attribute__((force))
#define __bitwise __attribute__((bitwise))
#else
#define __force
#define __bitwise
#endif

typedef u16 __bitwise be16;
typedef u16 __bitwise le16;
typedef u32 __bitwise be32;
typedef u32 __bitwise le32;
typedef u64 __bitwise be64;
typedef u64 __bitwise le64;

/*
 *	This is an Ethernet frame header.
 */
struct ethhdr {
	unsigned char	h_dest[ETH_ALEN];	/* destination eth addr	*/
	unsigned char	h_source[ETH_ALEN];	/* source ether addr	*/
    unsigned short  h_proto;		/* packet type ID field	*/
} __attribute__((packed));



typedef struct {
	volatile int counter;
} atomic_t;

struct kref {
	atomic_t refcount;
};

//static __inline int atomic_sub_return(int i, atomic_t *v)
//{
//	u32  cpu_sr;
//	int val;

//    cpu_sr = tls_os_set_critical();
//	val = v->counter;
//	v->counter = val -= i;
//    tls_os_release_critical(cpu_sr);

//	return val;
//}

//static __inline int atomic_add_return(int i, atomic_t *v)
//{
//	u32  cpu_sr;
//	int val;

//    cpu_sr = tls_os_set_critical();
//	val = v->counter;
//	v->counter = val += i;
//    tls_os_release_critical(cpu_sr);

//	return val;
//}


//#define atomic_set(v,i)					(((v)->counter) = (i))
//#define atomic_dec_and_test(v)			(atomic_sub_return(1, v) == 0)
//#define atomic_inc(v)					(void) atomic_add_return(1, v)
//#define atomic_dec(v)	            	(void) atomic_sub_return(1, v)

#endif /* end of TLS_COMMON_H */
