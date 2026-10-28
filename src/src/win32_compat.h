/**************************************************************************
 **
 ** sngrep - SIP Messages flow viewer
 **
 ** Windows (MinGW-w64) compatibility layer
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ****************************************************************************/
/**
 * @file win32_compat.h
 *
 * @brief Replacement for the BSD/Linux network headers (netinet/ *, arpa/inet.h)
 *
 * Only the structures and constants used by sngrep are provided. All
 * structures follow the BSD member naming and are laid out for little endian
 * hosts (the only ones Windows runs on).
 */
#ifndef SNGREP_WIN32_COMPAT_H
#define SNGREP_WIN32_COMPAT_H

#ifdef _WIN32

// Keep windows.h from defining SB_TOP, SB_BOTTOM... (clash with scrollbar.h)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOUSER
#define NOUSER
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>

typedef uint32_t in_addr_t;

#ifndef SIGHUP
#define SIGHUP  1
#endif
#ifndef SIGQUIT
#define SIGQUIT 3
#endif
#ifndef R_OK
#define R_OK 4
#endif
#ifndef F_OK
#define F_OK 0
#endif

#ifndef SIGUSR1
#define SIGUSR1 10
#endif
#define IP6F_OFF_MASK 0xf8ff

static inline char *
strsep(char **stringp, const char *delim)
{
    char *start = *stringp, *p;
    if (start == NULL)
        return NULL;
    if ((p = strpbrk(start, delim)) != NULL) {
        *p = '\0';
        *stringp = p + 1;
    } else {
        *stringp = NULL;
    }
    return start;
}

static inline char *
strcasestr(const char *haystack, const char *needle)
{
    size_t len = strlen(needle);
    for (; *haystack; haystack++)
        if (strncasecmp(haystack, needle, len) == 0)
            return (char *) haystack;
    return len ? NULL : (char *) haystack;
}

typedef unsigned char  u_int8_t;
typedef unsigned short u_int16_t;
typedef unsigned int   u_int32_t;
typedef unsigned long long u_int64_t;

#ifndef IPPROTO_IPIP
#define IPPROTO_IPIP 4
#endif
#ifndef IPPROTO_ESP
#define IPPROTO_ESP 50
#endif
#ifndef IPPROTO_FRAGMENT
#define IPPROTO_FRAGMENT 44
#endif

#define ETHER_ADDR_LEN 6
#define ETHERTYPE_IP   0x0800
#define ETHERTYPE_IPV6 0x86dd

#define IP_RF      0x8000
#define IP_DF      0x4000
#define IP_MF      0x2000
#define IP_OFFMASK 0x1fff

#pragma pack(push, 1)

struct ether_header {
    u_int8_t  ether_dhost[ETHER_ADDR_LEN];
    u_int8_t  ether_shost[ETHER_ADDR_LEN];
    u_int16_t ether_type;
};

struct ip {
    u_int8_t  ip_hl:4;
    u_int8_t  ip_v:4;
    u_int8_t  ip_tos;
    u_int16_t ip_len;
    u_int16_t ip_id;
    u_int16_t ip_off;
    u_int8_t  ip_ttl;
    u_int8_t  ip_p;
    u_int16_t ip_sum;
    struct in_addr ip_src, ip_dst;
};

struct udphdr {
    u_int16_t uh_sport;
    u_int16_t uh_dport;
    u_int16_t uh_ulen;
    u_int16_t uh_sum;
};

struct tcphdr {
    u_int16_t th_sport;
    u_int16_t th_dport;
    u_int32_t th_seq;
    u_int32_t th_ack;
    u_int8_t  th_x2:4;
    u_int8_t  th_off:4;
    u_int8_t  th_flags;
    u_int16_t th_win;
    u_int16_t th_sum;
    u_int16_t th_urp;
};

#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20

struct ip6_hdr {
    union {
        struct ip6_hdrctl {
            u_int32_t ip6_un1_flow;
            u_int16_t ip6_un1_plen;
            u_int8_t  ip6_un1_nxt;
            u_int8_t  ip6_un1_hlim;
        } ip6_un1;
        u_int8_t ip6_un2_vfc;
    } ip6_ctlun;
    struct in6_addr ip6_src;
    struct in6_addr ip6_dst;
};
#define ip6_nxt ip6_ctlun.ip6_un1.ip6_un1_nxt

struct ip6_frag {
    u_int8_t  ip6f_nxt;
    u_int8_t  ip6f_reserved;
    u_int16_t ip6f_offlg;
    u_int32_t ip6f_ident;
};

#pragma pack(pop)

#endif /* _WIN32 */

#endif /* SNGREP_WIN32_COMPAT_H */
