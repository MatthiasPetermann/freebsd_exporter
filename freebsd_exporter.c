/*-
 * Copyright (c) 2023 Matthias Petermann <mp@petermann-it.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * This program retrieves system metrics such as disk I/O, network I/O,
 * RAM and filesystem usage, as well as CPU load from the running system
 * and exposes them in the format of Prometheus metrics. It is designed
 * to be integrated into inetd, providing a lightweight, NetBSD-focused
 * alternative to the node_exporter.
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <sys/sysctl.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <sys/mount.h>
#include <sys/disk.h>
#include <syslog.h>
#include <unistd.h>
//#include <uvm/uvm_extern.h>
#include <getopt.h>

#include <devstat.h>
#include <fcntl.h>
#include <libgeom.h>
#include <limits.h>

#include "freebsd_exporter.h"
#include "version.h"

void print_filesystem_metric(const char* metric, const char* device, const char* type, const char* mountpoint, long value) {
    printf("freebsd_fs_%s_bytes{device=\"%s\",type=\"%s\",mountpoint=\"%s\"} %ld\n", metric, device, type, mountpoint, value);
}

void print_disk_io_metric(const char* device, long long unsigned int rbytes, long long unsigned int wbytes) {
    printf("freebsd_dk_read_bytes{device=\"%s\"} %llu\n", device, rbytes);
    printf("freebsd_dk_write_bytes{device=\"%s\"} %llu\n", device, wbytes);
}

void print_load_metric(const char* metric, double value) {
    printf("freebsd_load%s %lf\n", metric, value);
}

void print_network_metric(const char* interface, unsigned long long rxbytes, unsigned long long txbytes, unsigned long long errors) {
    printf("freebsd_netif_rx_bytes{interface=\"%s\"} %llu\n", interface, rxbytes);
    printf("freebsd_netif_tx_bytes{interface=\"%s\"} %llu\n", interface, txbytes);
    printf("freebsd_netif_errors{interface=\"%s\"} %llu\n", interface, errors);
}

void print_memory_metric(const char* metric, long value) {
    printf("freebsd_mem_%s_bytes %ld\n", metric, value);
}

void retrieve_disk_space_metrics() {
    struct statfs* fsinfo;
    int numfs;
    int i;

    numfs = getmntinfo(&fsinfo, MNT_WAIT);
    if (numfs == -1) {
        log_message(LOG_ERR, "Could not determine mounted filesystems.");
        return;
    }

    for (i = 0; i < numfs; i++) {  
        print_filesystem_metric("size", fsinfo[i].f_mntfromname, fsinfo[i].f_fstypename, 
			fsinfo[i].f_mntonname, fsinfo[i].f_blocks * fsinfo[i].f_bsize);
        print_filesystem_metric("used", fsinfo[i].f_mntfromname, fsinfo[i].f_fstypename, 
			fsinfo[i].f_mntonname, (fsinfo[i].f_blocks - fsinfo[i].f_bfree) * fsinfo[i].f_bsize);
        print_filesystem_metric("free", fsinfo[i].f_mntfromname, fsinfo[i].f_fstypename, 
			fsinfo[i].f_mntonname, fsinfo[i].f_bavail * fsinfo[i].f_bsize);
    }
}

void retrieve_cpu_load_metrics() {
    double loadavg[3];

    if (getloadavg(loadavg, 3) != -1) {
        print_load_metric("1", loadavg[0]);
        print_load_metric("5", loadavg[1]);
        print_load_metric("15", loadavg[2]);
    } else {
        log_message(LOG_ERR, "loadavg failed.");
    }
}

void retrieve_network_interface_metrics() {
    struct ifaddrs* ifap, * ifa;

    if (getifaddrs(&ifap) == 0) {
        for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == NULL || !(ifa->ifa_flags & IFF_UP))
                continue;

            if (ifa->ifa_addr->sa_family == AF_LINK) {
                struct if_data* ifd = (struct if_data*)ifa->ifa_data;
                print_network_metric(ifa->ifa_name, ifd->ifi_ibytes, ifd->ifi_obytes, ifd->ifi_ierrors + ifd->ifi_oerrors);
            }
        }
        freeifaddrs(ifap);
    } else {
        log_message(LOG_ERR, "Could not get network interfaces.");
    }
}

//void retrieve_memory_metrics() {
    //int pagesize;
    //struct uvmexp_sysctl u;

    //// getpagesize() is obsolete, see manpage
    //pagesize = sysconf(_SC_PAGESIZE);
    //int mib[2];
    //size_t size = sizeof(struct uvmexp_sysctl);
    //mib[0] = CTL_VM;
    //mib[1] = VM_UVMEXP2;
    //if (sysctl(mib, 2, &u, &size, NULL, 0) == -1) {
        //log_message(LOG_ERR, "sysctl failed.");
        //return;
    //}
    //print_memory_metric("size", u.npages * pagesize);
    //print_memory_metric("free", u.free * pagesize);
    //print_memory_metric("active", u.active * pagesize);
    //print_memory_metric("inactive", u.inactive * pagesize);
    //print_memory_metric("paging", u.paging * pagesize);
    //print_memory_metric("wired", u.wired * pagesize);
    //print_memory_metric("swap_size", u.swpages * pagesize);
    //print_memory_metric("swap_used", u.swpginuse * pagesize);
//}

void retrieve_disk_io_metrics() {
   

}

void log_message(int priority, const char* message) {
    if (option_syslog) {
        syslog(priority, "%s", message);
    } else {
        fprintf(stderr, "%s\n", message);
    }
}

void print_help() {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("Options:\n");
    printf("  --help              Display this help message\n");
    printf("  --no-http-header    Disable HTTP headers\n");
    printf("  --no-syslog         Disable logging messages using syslog\n");
    printf("  --version           Print the version information\n");
}

void print_version() {
    printf("%s  version %d.%d.%d\n", program_name, MAJOR_VERSION,
        MINOR_VERSION, PATCH_VERSION );
}

int main(int argc, char *argv[]) {

    struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"no-http-header", no_argument, NULL, 1},
        {"no-syslog", no_argument, NULL, 2},
        {"version", no_argument, NULL, 3},
        {NULL, 0, NULL, 0}
    };

    int option;
    while ((option = getopt_long(argc, argv, "h", long_options, NULL)) != -1) {
        switch (option) {
            case 'h':
                print_help();
                return 0;
            case 1:
                option_http_header = 0;
                break;
            case 2:
                option_syslog = 0;
                break;
            case 3:
                print_version();
                return 0;
            case '?':
                fprintf(stderr, "Unknown option: %s\n", argv[optind - 1]);
                return 1;
            default:
                break;
        }
    }

    if (option_syslog) {
        openlog(program_name, LOG_PID, LOG_USER);
    }

    if (option_http_header) {
        printf("HTTP/1.1 200 OK\r\n");
        printf("Content-Type: text/plain\r\n\r\n");
    }

    retrieve_disk_space_metrics();
    retrieve_cpu_load_metrics();
    retrieve_network_interface_metrics();
    //retrieve_memory_metrics();
    retrieve_disk_io_metrics();

    if(option_syslog) {
        closelog();
    }
    return 0;
}
