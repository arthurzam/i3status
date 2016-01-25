// vim:ts=4:sw=4:expandtab
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <net/if.h>

#include "i3status.h"

static struct ifaddrs *ifaddr = NULL;

/*
 * Call this function to update the ifaddrs,
 * so that we can cache this structure between calls for wireless, ethernet, etc.
 */
void update_network_ifaddrs()
{
    if (ifaddr != NULL)
        freeifaddrs(ifaddr);
    getifaddrs(&ifaddr);
}

/*
 * Return the IP address for the given interface or "no IP" if the
 * interface is up and running but hasn't got an IP address yet
 *
 */
const char *get_ip_addr(const char *interface) {
    static char part[512];
    socklen_t len = sizeof(struct sockaddr_in);
    memset(part, 0, sizeof(part));

    struct ifaddrs *addrp;
    bool found = false;

    if (ifaddr == NULL)
        return NULL;

    /* Skip until we are at the AF_INET address of interface */
    for (addrp = ifaddr;

         (addrp != NULL &&
          (strcmp(addrp->ifa_name, interface) != 0 ||
           addrp->ifa_addr == NULL ||
           addrp->ifa_addr->sa_family != AF_INET));

         addrp = addrp->ifa_next) {
        /* Check if the interface is down */
        if (strcmp(addrp->ifa_name, interface) != 0)
            continue;
        found = true;
        if ((addrp->ifa_flags & IFF_RUNNING) == 0) {
            return NULL;
        }
    }

    if (addrp == NULL) {
        return (found ? "no IP" : NULL);
    }

    int ret;
    if ((ret = getnameinfo(addrp->ifa_addr, len, part, sizeof(part), NULL, 0, NI_NUMERICHOST)) != 0) {
        fprintf(stderr, "i3status: getnameinfo(): %s\n", gai_strerror(ret));
        return "no IP";
    }
    return part;
}
