# Introduction

The freebsd_exporter retrieves system metrics such as disk I/O, network I/O,
RAM and filesystem usage, as well as CPU load from the running system and
exposes them in the format of Prometheus metrics. It is designed to be
integrated into inetd, providing a lightweight, FreeBSD-focused alternative
to the node_exporter. It can serve as a complement or replacement for 
prometheus_sysctl_exporter, as it primarily focuses on general performance
characteristics at a high level.

# Usage

## Installation

```
$ fetch https://forge.petermann-it.de/mpeterma/freebsd_exporter/archive/0.9.0.tar.gz
$ mv 0.9.0.tar.gz freebsd_exporter-0.9.0.tar.gz
$ tar xvfz freebsd_exporter-0.9.0.tar.gz
$ cd freebsd_exporter
$ make
$ doas make install
```

## Configuration

```
$ doas vi /etc/services

    freebsd_exporter 9100/tcp

$ doas services_mkdb
$ doas vi /etc/inetd.conf

    freebsd_exporter stream  tcp     nowait:600      nobody  /usr/libexec/freebsd_exporter    freebsd_exporter

$ doas service inetd restart
```

## Test

 - Point Browser to: http://localhost:9100/metrics

# Example

```
HTTP/1.1 200 OK
Content-Type: text/plain

freebsd_fs_size_bytes{device="zroot/ROOT/default",type="zfs",mountpoint="/"} 449003569152
freebsd_fs_used_bytes{device="zroot/ROOT/default",type="zfs",mountpoint="/"} 17363234816
freebsd_fs_free_bytes{device="zroot/ROOT/default",type="zfs",mountpoint="/"} 431640334336
freebsd_fs_size_bytes{device="devfs",type="devfs",mountpoint="/dev"} 1024
freebsd_fs_used_bytes{device="devfs",type="devfs",mountpoint="/dev"} 0
freebsd_fs_free_bytes{device="devfs",type="devfs",mountpoint="/dev"} 1024
freebsd_fs_size_bytes{device="/dev/gpt/efiboot0",type="msdosfs",mountpoint="/boot/efi"} 272531456
freebsd_fs_used_bytes{device="/dev/gpt/efiboot0",type="msdosfs",mountpoint="/boot/efi"} 1392640
freebsd_fs_free_bytes{device="/dev/gpt/efiboot0",type="msdosfs",mountpoint="/boot/efi"} 271138816
freebsd_fs_size_bytes{device="zroot/var/mail",type="zfs",mountpoint="/var/mail"} 431640522752
freebsd_fs_used_bytes{device="zroot/var/mail",type="zfs",mountpoint="/var/mail"} 188416
freebsd_fs_free_bytes{device="zroot/var/mail",type="zfs",mountpoint="/var/mail"} 431640334336
freebsd_fs_size_bytes{device="zroot/home",type="zfs",mountpoint="/home"} 457131032576
freebsd_fs_used_bytes{device="zroot/home",type="zfs",mountpoint="/home"} 25490698240
freebsd_fs_free_bytes{device="zroot/home",type="zfs",mountpoint="/home"} 431640334336
freebsd_fs_size_bytes{device="zdata",type="zfs",mountpoint="/zdata"} 675632885760
freebsd_fs_used_bytes{device="zdata",type="zfs",mountpoint="/zdata"} 267745734656
freebsd_fs_free_bytes{device="zdata",type="zfs",mountpoint="/zdata"} 407887151104
freebsd_fs_size_bytes{device="zroot/tmp",type="zfs",mountpoint="/tmp"} 431640539136
freebsd_fs_used_bytes{device="zroot/tmp",type="zfs",mountpoint="/tmp"} 204800
freebsd_fs_free_bytes{device="zroot/tmp",type="zfs",mountpoint="/tmp"} 431640334336
freebsd_fs_size_bytes{device="zroot",type="zfs",mountpoint="/zroot"} 431640432640
freebsd_fs_used_bytes{device="zroot",type="zfs",mountpoint="/zroot"} 98304
freebsd_fs_free_bytes{device="zroot",type="zfs",mountpoint="/zroot"} 431640334336
freebsd_fs_size_bytes{device="zroot/bastille",type="zfs",mountpoint="/usr/local/bastille"} 431640473600
freebsd_fs_used_bytes{device="zroot/bastille",type="zfs",mountpoint="/usr/local/bastille"} 139264
freebsd_fs_free_bytes{device="zroot/bastille",type="zfs",mountpoint="/usr/local/bastille"} 431640334336
freebsd_fs_size_bytes{device="zroot/var/log",type="zfs",mountpoint="/var/log"} 431641075712
freebsd_fs_used_bytes{device="zroot/var/log",type="zfs",mountpoint="/var/log"} 741376
freebsd_fs_free_bytes{device="zroot/var/log",type="zfs",mountpoint="/var/log"} 431640334336
freebsd_fs_size_bytes{device="zroot/var/crash",type="zfs",mountpoint="/var/crash"} 431640432640
freebsd_fs_used_bytes{device="zroot/var/crash",type="zfs",mountpoint="/var/crash"} 98304
freebsd_fs_free_bytes{device="zroot/var/crash",type="zfs",mountpoint="/var/crash"} 431640334336
freebsd_fs_size_bytes{device="zroot/usr/src",type="zfs",mountpoint="/usr/src"} 432495665152
freebsd_fs_used_bytes{device="zroot/usr/src",type="zfs",mountpoint="/usr/src"} 855330816
freebsd_fs_free_bytes{device="zroot/usr/src",type="zfs",mountpoint="/usr/src"} 431640334336
freebsd_fs_size_bytes{device="zroot/usr/ports",type="zfs",mountpoint="/usr/ports"} 431640432640
freebsd_fs_used_bytes{device="zroot/usr/ports",type="zfs",mountpoint="/usr/ports"} 98304
freebsd_fs_free_bytes{device="zroot/usr/ports",type="zfs",mountpoint="/usr/ports"} 431640334336
freebsd_fs_size_bytes{device="zroot/var/audit",type="zfs",mountpoint="/var/audit"} 431640432640
freebsd_fs_used_bytes{device="zroot/var/audit",type="zfs",mountpoint="/var/audit"} 98304
freebsd_fs_free_bytes{device="zroot/var/audit",type="zfs",mountpoint="/var/audit"} 431640334336
freebsd_fs_size_bytes{device="zroot/var/tmp",type="zfs",mountpoint="/var/tmp"} 431640432640
freebsd_fs_used_bytes{device="zroot/var/tmp",type="zfs",mountpoint="/var/tmp"} 98304
freebsd_fs_free_bytes{device="zroot/var/tmp",type="zfs",mountpoint="/var/tmp"} 431640334336
freebsd_fs_size_bytes{device="zdata/virtualbox",type="zfs",mountpoint="/zdata/virtualbox"} 697744617472
freebsd_fs_used_bytes{device="zdata/virtualbox",type="zfs",mountpoint="/zdata/virtualbox"} 289857466368
freebsd_fs_free_bytes{device="zdata/virtualbox",type="zfs",mountpoint="/zdata/virtualbox"} 407887151104
freebsd_fs_size_bytes{device="zroot/bastille/logs",type="zfs",mountpoint="/var/log/bastille"} 431640436736
freebsd_fs_used_bytes{device="zroot/bastille/logs",type="zfs",mountpoint="/var/log/bastille"} 102400
freebsd_fs_free_bytes{device="zroot/bastille/logs",type="zfs",mountpoint="/var/log/bastille"} 431640334336
freebsd_fs_size_bytes{device="zroot/bastille/backups",type="zfs",mountpoint="/usr/local/bastille/backups"} 431640432640
freebsd_fs_used_bytes{device="zroot/bastille/backups",type="zfs",mountpoint="/usr/local/bastille/backups"} 98304
freebsd_fs_free_bytes{device="zroot/bastille/backups",type="zfs",mountpoint="/usr/local/bastille/backups"} 431640334336
freebsd_fs_size_bytes{device="zroot/bastille/cache",type="zfs",mountpoint="/usr/local/bastille/cache"} 431640440832
freebsd_fs_used_bytes{device="zroot/bastille/cache",type="zfs",mountpoint="/usr/local/bastille/cache"} 106496
freebsd_fs_free_bytes{device="zroot/bastille/cache",type="zfs",mountpoint="/usr/local/bastille/cache"} 431640334336
freebsd_fs_size_bytes{device="zroot/bastille/jails",type="zfs",mountpoint="/usr/local/bastille/jails"} 431640432640
freebsd_fs_used_bytes{device="zroot/bastille/jails",type="zfs",mountpoint="/usr/local/bastille/jails"} 98304
freebsd_fs_free_bytes{device="zroot/bastille/jails",type="zfs",mountpoint="/usr/local/bastille/jails"} 431640334336
freebsd_fs_size_bytes{device="zroot/bastille/templates",type="zfs",mountpoint="/usr/local/bastille/templates"} 431640432640
freebsd_fs_used_bytes{device="zroot/bastille/templates",type="zfs",mountpoint="/usr/local/bastille/templates"} 98304
freebsd_fs_free_bytes{device="zroot/bastille/templates",type="zfs",mountpoint="/usr/local/bastille/templates"} 431640334336
freebsd_fs_size_bytes{device="zroot/bastille/releases",type="zfs",mountpoint="/usr/local/bastille/releases"} 431640440832
freebsd_fs_used_bytes{device="zroot/bastille/releases",type="zfs",mountpoint="/usr/local/bastille/releases"} 106496
freebsd_fs_free_bytes{device="zroot/bastille/releases",type="zfs",mountpoint="/usr/local/bastille/releases"} 431640334336
freebsd_fs_size_bytes{device="zroot/bastille/cache/13.2-RELEASE",type="zfs",mountpoint="/usr/local/bastille/cache/13.2-RELEASE"} 431840571392
freebsd_fs_used_bytes{device="zroot/bastille/cache/13.2-RELEASE",type="zfs",mountpoint="/usr/local/bastille/cache/13.2-RELEASE"} 200237056
freebsd_fs_free_bytes{device="zroot/bastille/cache/13.2-RELEASE",type="zfs",mountpoint="/usr/local/bastille/cache/13.2-RELEASE"} 431640334336
freebsd_fs_size_bytes{device="zroot/bastille/cache/14.0-RELEASE",type="zfs",mountpoint="/usr/local/bastille/cache/14.0-RELEASE"} 431840485376
freebsd_fs_used_bytes{device="zroot/bastille/cache/14.0-RELEASE",type="zfs",mountpoint="/usr/local/bastille/cache/14.0-RELEASE"} 200151040
freebsd_fs_free_bytes{device="zroot/bastille/cache/14.0-RELEASE",type="zfs",mountpoint="/usr/local/bastille/cache/14.0-RELEASE"} 431640334336
freebsd_fs_size_bytes{device="zroot/bastille/releases/13.2-RELEASE",type="zfs",mountpoint="/usr/local/bastille/releases/13.2-RELEASE"} 432167903232
freebsd_fs_used_bytes{device="zroot/bastille/releases/13.2-RELEASE",type="zfs",mountpoint="/usr/local/bastille/releases/13.2-RELEASE"} 527568896
freebsd_fs_free_bytes{device="zroot/bastille/releases/13.2-RELEASE",type="zfs",mountpoint="/usr/local/bastille/releases/13.2-RELEASE"} 431640334336
freebsd_fs_size_bytes{device="zroot/bastille/jails/redmine",type="zfs",mountpoint="/usr/local/bastille/jails/redmine"} 431640453120
freebsd_fs_used_bytes{device="zroot/bastille/jails/redmine",type="zfs",mountpoint="/usr/local/bastille/jails/redmine"} 118784
freebsd_fs_free_bytes{device="zroot/bastille/jails/redmine",type="zfs",mountpoint="/usr/local/bastille/jails/redmine"} 431640334336
freebsd_fs_size_bytes{device="zroot/bastille/releases/14.0-RELEASE",type="zfs",mountpoint="/usr/local/bastille/releases/14.0-RELEASE"} 432094195712
freebsd_fs_used_bytes{device="zroot/bastille/releases/14.0-RELEASE",type="zfs",mountpoint="/usr/local/bastille/releases/14.0-RELEASE"} 453861376
freebsd_fs_free_bytes{device="zroot/bastille/releases/14.0-RELEASE",type="zfs",mountpoint="/usr/local/bastille/releases/14.0-RELEASE"} 431640334336
freebsd_fs_size_bytes{device="zroot/bastille/jails/redmine/root",type="zfs",mountpoint="/usr/local/bastille/jails/redmine/root"} 433119682560
freebsd_fs_used_bytes{device="zroot/bastille/jails/redmine/root",type="zfs",mountpoint="/usr/local/bastille/jails/redmine/root"} 1479348224
freebsd_fs_free_bytes{device="zroot/bastille/jails/redmine/root",type="zfs",mountpoint="/usr/local/bastille/jails/redmine/root"} 431640334336
freebsd_fs_size_bytes{device="zvault",type="zfs",mountpoint="/zvault"} 611834281984
freebsd_fs_used_bytes{device="zvault",type="zfs",mountpoint="/zvault"} 59946000384
freebsd_fs_free_bytes{device="zvault",type="zfs",mountpoint="/zvault"} 551888281600
freebsd_fs_size_bytes{device="zvault/virtualbox",type="zfs",mountpoint="/zvault/virtualbox"} 816602763264
freebsd_fs_used_bytes{device="zvault/virtualbox",type="zfs",mountpoint="/zvault/virtualbox"} 264714481664
freebsd_fs_free_bytes{device="zvault/virtualbox",type="zfs",mountpoint="/zvault/virtualbox"} 551888281600
freebsd_fs_size_bytes{device="zvault/iso-images",type="zfs",mountpoint="/zvault/iso-images"} 633746821120
freebsd_fs_used_bytes{device="zvault/iso-images",type="zfs",mountpoint="/zvault/iso-images"} 81858539520
freebsd_fs_free_bytes{device="zvault/iso-images",type="zfs",mountpoint="/zvault/iso-images"} 551888281600
freebsd_fs_size_bytes{device="zvault/divers",type="zfs",mountpoint="/zvault/divers"} 551888379904
freebsd_fs_used_bytes{device="zvault/divers",type="zfs",mountpoint="/zvault/divers"} 98304
freebsd_fs_free_bytes{device="zvault/divers",type="zfs",mountpoint="/zvault/divers"} 551888281600
freebsd_fs_size_bytes{device="map auto_nfsv4",type="autofs",mountpoint="/net"} 0
freebsd_fs_used_bytes{device="map auto_nfsv4",type="autofs",mountpoint="/net"} 0
freebsd_fs_free_bytes{device="map auto_nfsv4",type="autofs",mountpoint="/net"} 0
freebsd_fs_size_bytes{device="/usr/local/bastille/releases/13.2-RELEASE",type="nullfs",mountpoint="/usr/local/bastille/jails/redmine/root/.bastille"} 432167903232
freebsd_fs_used_bytes{device="/usr/local/bastille/releases/13.2-RELEASE",type="nullfs",mountpoint="/usr/local/bastille/jails/redmine/root/.bastille"} 527568896
freebsd_fs_free_bytes{device="/usr/local/bastille/releases/13.2-RELEASE",type="nullfs",mountpoint="/usr/local/bastille/jails/redmine/root/.bastille"} 431640334336
freebsd_fs_size_bytes{device="devfs",type="devfs",mountpoint="/usr/local/bastille/jails/redmine/root/dev"} 1024
freebsd_fs_used_bytes{device="devfs",type="devfs",mountpoint="/usr/local/bastille/jails/redmine/root/dev"} 0
freebsd_fs_free_bytes{device="devfs",type="devfs",mountpoint="/usr/local/bastille/jails/redmine/root/dev"} 1024
freebsd_load1 0.150879
freebsd_load5 0.244141
freebsd_load15 0.217773
freebsd_netif_rx_bytes{interface="em0"} 1545910498
freebsd_netif_tx_bytes{interface="em0"} 125040503
freebsd_netif_errors{interface="em0"} 0
freebsd_netif_rx_bytes{interface="lo0"} 400
freebsd_netif_tx_bytes{interface="lo0"} 400
freebsd_netif_errors{interface="lo0"} 0
freebsd_netif_rx_bytes{interface="bastille0"} 2781249
freebsd_netif_tx_bytes{interface="bastille0"} 2781249
freebsd_netif_errors{interface="bastille0"} 0
freebsd_mem_bufspace_bytes 57856
freebsd_mem_size_bytes 32944029696
freebsd_mem_active_bytes 1513656320
freebsd_mem_inactive_bytes 5786017792
freebsd_mem_laundry_bytes 458391552
freebsd_mem_wired_bytes 3690967040
freebsd_mem_free_bytes 21445959680
freebsd_mem_swap_size_bytes 16777088
freebsd_mem_swap_used_bytes 0
freebsd_device_read_bytes{device="nda0"} 1274534400
freebsd_device_write_bytes{device="nda0"} 8025157632
freebsd_device_read_bytes{device="nda1"} 517731328
freebsd_device_write_bytes{device="nda1"} 62984192
freebsd_device_read_bytes{device="ada0"} 8393728
freebsd_device_write_bytes{device="ada0"} 64901120
freebsd_device_read_bytes{device="ada1"} 7439360
freebsd_device_write_bytes{device="ada1"} 64901120
freebsd_device_read_bytes{device="pass0"} 512
freebsd_device_write_bytes{device="pass0"} 0
freebsd_device_read_bytes{device="pass1"} 512
freebsd_device_write_bytes{device="pass1"} 0
freebsd_device_read_bytes{device="pass6"} 291
freebsd_device_write_bytes{device="pass6"} 0
freebsd_device_read_bytes{device="pass7"} 291
freebsd_device_write_bytes{device="pass7"} 0
freebsd_device_read_bytes{device="pass8"} 291
freebsd_device_write_bytes{device="pass8"} 0
```

# Credits

For parts of the code, such as handling sysctls and accessing devstat, 
implementations from the FreeBSD base system served as inspiration and a
template. In particular, the tools iostat and top were used as references.
