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
#include <getopt.h>

#include <devstat.h>
#include <inttypes.h>
#include <fcntl.h>
#include <paths.h>

#include "freebsd_exporter.h"
#include "version.h"

#define GETSYSCTL(name, var) getsysctl(name, &(var), sizeof(var))

static void getsysctl(const char *name, void *ptr, size_t len);
static int swapmode(int *retavail, int *retfree);

static kvm_t *kd;

void print_filesystem_metric(const char* metric, const char* device, const char* type, const char* mountpoint, long value) {
    printf("freebsd_fs_%s_bytes{device=\"%s\",type=\"%s\",mountpoint=\"%s\"} %ld\n", metric, device, type, mountpoint, value);
}

void print_disk_io_metric(const char* device, u_int64_t rbytes, u_int64_t wbytes) {
    printf("freebsd_device_read_bytes{device=\"%s\"} %" PRIu64 "\n", device, rbytes);
    printf("freebsd_device_write_bytes{device=\"%s\"} %" PRIu64 "\n", device, wbytes);
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

static void
getsysctl(const char *name, void *ptr, size_t len)
{
	size_t nlen = len;

	if (sysctlbyname(name, ptr, &nlen, NULL, 0) == -1) {
		log_message(LOG_ERR, "Sysctl failed.");
		return;
	}
	if (nlen != len) {
		log_message(LOG_ERR, "Value length invalid.");
		return;
	}
}


static int
swapmode(int *retavail, int *retfree)
{
	int n;
	struct kvm_swap swapary[1];
	static int pagesize = 0;
	static unsigned long swap_maxpages = 0;

	*retavail = 0;
	*retfree = 0;

#define CONVERT(v)	((quad_t)(v) * pagesize / 1024)

	n = kvm_getswapinfo(kd, swapary, 1, 0);
	if (n < 0 || swapary[0].ksw_total == 0)
		return (0);

	if (pagesize == 0)
		pagesize = getpagesize();
	if (swap_maxpages == 0)
		GETSYSCTL("vm.swap_maxpages", swap_maxpages);

	/* ksw_total contains the total size of swap all devices which may
	   exceed the maximum swap size allocatable in the system */
	if ( swapary[0].ksw_total > swap_maxpages )
		swapary[0].ksw_total = swap_maxpages;

	*retavail = CONVERT(swapary[0].ksw_total);
	*retfree = CONVERT(swapary[0].ksw_total - swapary[0].ksw_used);

#undef CONVERT

	n = (int)(swapary[0].ksw_used * 100.0 / swapary[0].ksw_total);
	return (n);
}

void retrieve_memory_metrics() {
    int pagesize;
    static int swapavail = 0;
	static int swapfree = 0;
	static long bufspace = 0;
	static uint64_t page_count;
	static uint64_t active_count;
	static uint64_t inactive_count;
	static uint64_t laundry_count;
	static uint64_t wire_count;
	static uint64_t free_count;
    
    pagesize = getpagesize();
    GETSYSCTL("vfs.bufspace", bufspace);
    GETSYSCTL("vm.stats.vm.v_page_count", page_count);
    GETSYSCTL("vm.stats.vm.v_active_count", active_count);
    GETSYSCTL("vm.stats.vm.v_inactive_count", inactive_count);
    GETSYSCTL("vm.stats.vm.v_laundry_count", laundry_count);
    GETSYSCTL("vm.stats.vm.v_wire_count", wire_count);
    GETSYSCTL("vm.stats.vm.v_free_count", free_count);    
	swapmode(&swapavail, &swapfree);
    
    print_memory_metric("bufspace", bufspace);
    print_memory_metric("size", page_count * pagesize);
    print_memory_metric("active", active_count * pagesize);
    print_memory_metric("inactive", inactive_count * pagesize);
    print_memory_metric("laundry", laundry_count * pagesize);
    print_memory_metric("wire", wire_count * pagesize);
    print_memory_metric("free", free_count * pagesize);
    print_memory_metric("swap_size", swapavail);
    print_memory_metric("swap_used", swapavail-swapfree);
}

void retrieve_disk_io_metrics() {
    struct statinfo cur;
	int num_devices, dn;
  
  	long double transfers_per_second, transfers_per_second_read;
	long double transfers_per_second_write;
	long double kb_per_transfer, mb_per_second, mb_per_second_read;
	long double mb_per_second_write;
	u_int64_t total_bytes, total_transfers, total_blocks;
	u_int64_t total_bytes_read, total_transfers_read;
	u_int64_t total_bytes_write, total_transfers_write;
	long double busy_pct, busy_time;
	u_int64_t queue_len;
	long double blocks_per_second, total_duration;
	long double ms_per_other, ms_per_read, ms_per_write, ms_per_transaction;
	char *devicename;

	cur.dinfo = (struct devinfo *)calloc(1, sizeof(struct devinfo));
	if(cur.dinfo == NULL) {
		log_message(LOG_ERR, "Could not allocate memory for device info.");
		return;
	}
  
	if (devstat_getdevs(NULL, &cur) == -1){
		log_message(LOG_ERR, "Could not get devices.");
		free(cur.dinfo);
		return;
	}
  
	num_devices = cur.dinfo->numdevs;
      
    for (dn = 0; dn < num_devices; dn++) {		
		if (asprintf(&devicename, "%s%d", 
			cur.dinfo->devices[dn].device_name,
			cur.dinfo->devices[dn].unit_number) == -1) {
				log_message(LOG_ERR, "Could not format device name.");
				continue;
		}
				
		if (devstat_compute_statistics(&cur.dinfo->devices[dn],
		    NULL, 0,
		    DSM_TOTAL_BYTES, &total_bytes,
		    DSM_TOTAL_BYTES_READ, &total_bytes_read,
		    DSM_TOTAL_BYTES_WRITE, &total_bytes_write,
		    DSM_TOTAL_TRANSFERS, &total_transfers,
		    DSM_TOTAL_TRANSFERS_READ, &total_transfers_read,
		    DSM_TOTAL_TRANSFERS_WRITE, &total_transfers_write,
		    DSM_TOTAL_BLOCKS, &total_blocks,
		    DSM_KB_PER_TRANSFER, &kb_per_transfer,
		    DSM_TRANSFERS_PER_SECOND, &transfers_per_second,
		    DSM_TRANSFERS_PER_SECOND_READ, &transfers_per_second_read,
		    DSM_TRANSFERS_PER_SECOND_WRITE, &transfers_per_second_write,
		    DSM_MB_PER_SECOND, &mb_per_second,
		    DSM_MB_PER_SECOND_READ, &mb_per_second_read,
		    DSM_MB_PER_SECOND_WRITE, &mb_per_second_write,
		    DSM_BLOCKS_PER_SECOND, &blocks_per_second,
		    DSM_MS_PER_TRANSACTION, &ms_per_transaction,
		    DSM_MS_PER_TRANSACTION_READ, &ms_per_read,
		    DSM_MS_PER_TRANSACTION_WRITE, &ms_per_write,
		    DSM_MS_PER_TRANSACTION_OTHER, &ms_per_other,
		    DSM_BUSY_PCT, &busy_pct,
		    DSM_QUEUE_LENGTH, &queue_len,
		    DSM_TOTAL_DURATION, &total_duration,
		    DSM_TOTAL_BUSY_TIME, &busy_time,
		    DSM_NONE) != 0) {
				log_message(LOG_ERR, "Could not get device statistics.");
				continue;
		}
		
		if(total_bytes_read>0 || total_bytes_write > 0) {
			print_disk_io_metric(devicename, total_bytes_read, total_bytes_write);
		}		
		free(devicename);
	}
	free(cur.dinfo);
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
    
    kd = kvm_open(NULL, _PATH_DEVNULL, NULL, O_RDONLY, "kvm_open");
	if (kd == NULL) {
		log_message(LOG_ERR, "Could not open kernel memory file.");
		return (-1);
	}

    retrieve_disk_space_metrics();
    retrieve_cpu_load_metrics();
    retrieve_network_interface_metrics();
    retrieve_memory_metrics();
    retrieve_disk_io_metrics();

    if(option_syslog) {
        closelog();
    }
    return 0;
}
