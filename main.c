#include <ctype.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define OPTS "hrp"
/*
 * h for help
 * r for setting a random valid address
 */
#define MAC_LEN 6

enum mode {HELP, USAGE};
static void usage(enum mode m);
static int is_mac(const char *mac, unsigned char hex_addr[MAC_LEN]);

int main(int argc, char *argv[]) {
    int status = 0;
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    unsigned char addr[MAC_LEN];
    // parse options
    int opt;
    while((opt = getopt(argc, argv, OPTS)) != -1) {
        switch (opt) {
            case 'h':
                usage(HELP);
                return 0;
            case 'r':
                // not implemented yet
                break;
            case 'p':
                // not implemented yet
                break;
            default:
                return 1;
        }
    }
    // skip options
    argc -= optind;
    argv += optind;
    // check root
    if(geteuid() != 0) {
        fprintf(stderr, "This program must be run as root\n");
        return 1;
    }
    // standard usage
    if(argc != 2) {
        usage(USAGE);
        return 1;
    }
    // check MAC address
    const char *ifname = argv[0];
    const char *new_mac = argv[1];
    if(!is_mac(new_mac, addr)) {
        fprintf(stderr, "Invalid MAC address!\n");
        return 1;
    }
    // open a socket for ioctl calls
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1) {
        perror("socket");
        return 1;
    }
    // copy the interface name
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
    // check the interface status
    if(ioctl(fd, SIOCGIFFLAGS, &ifr) == -1) {
        perror("ioctl SIOCGIFFLAGS");
        close(fd);
        return 1;
    }
    // bring the interface down
    if(ifr.ifr_flags & IFF_UP) {
        ifr.ifr_flags &= ~IFF_UP;
        if(ioctl(fd, SIOCSIFFLAGS, &ifr) == -1) {
            perror("ioctl SIOCSIFFLAGS");
            close(fd);
            return 1;
        }
    }
    // read the current MAC address (stored in ifr.ifr_hwaddr.sa_data)
    if(ioctl(fd, SIOCGIFHWADDR, &ifr) == -1) {
        perror("ioctl SIOCGIFHWADDR");
        status = 1;
        goto up;
    }
    printf("Current MAC address: ");
    for(size_t i = 0; i < MAC_LEN; i++)
        printf("%02x%s", (unsigned char)ifr.ifr_hwaddr.sa_data[i], i < MAC_LEN - 1 ? ":" : "");
    puts("");
    // set the new address
    memset(&ifr.ifr_hwaddr, 0, sizeof(ifr.ifr_hwaddr));
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    memcpy(ifr.ifr_hwaddr.sa_data, addr, MAC_LEN);
    if(ioctl(fd, SIOCSIFHWADDR, &ifr) == -1) {
        perror("ioctl SIOCSIFHWADDR");
        status = 1;
        goto up;
    }
    printf("New MAC address: ");
    for(size_t i = 0; i < MAC_LEN; i++)
        printf("%02x%s", addr[i], i < MAC_LEN - 1 ? ":" : "");
    puts("");
    // bring the interface up
up:
    if(ioctl(fd, SIOCGIFFLAGS, &ifr) == -1) {
        perror("ioctl SIOCGIFFLAGS");
        close(fd);
        return 1;
    }
    ifr.ifr_flags |= IFF_UP;
    if(ioctl(fd, SIOCSIFFLAGS, &ifr) == -1) {
        perror("ioctl SIOCSIFFLAGS");
        close(fd);
        return 1;
    }
    close(fd);
    return status;
}

static void usage(enum mode m) {
    if(m == HELP)
        printf("Change your interface MAC address temporarily\n"
            "Options:\n"
            "\t-h  show this help\n"
            "\t-r  set a random valid MAC address\n");
    else
        fprintf(stderr, "Usage: chgmac [options] <interface> <new_mac>\n");
}

static int is_mac(const char *mac, unsigned char hex_addr[MAC_LEN]) {
    size_t mac_len = strlen(mac);
    if(mac_len != 17)
        return 0;
    for(size_t i = 0; i < mac_len; i++) {
        if(i % 3 == 2) {
            if(mac[i] != ':')
                return 0;
            continue;
        }
        else if(!isxdigit((unsigned char)mac[i]))
            return 0;
    }
    sscanf(mac, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx", &hex_addr[0], &hex_addr[1], &hex_addr[2], &hex_addr[3], &hex_addr[4], &hex_addr[5]);
    return 1;
}
