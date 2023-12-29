# freebsd_exporter

The freebsd_exporter retrieves system metrics such as disk I/O, network I/O,
RAM and filesystem usage, as well as CPU load from the running system and
exposes them in the format of Prometheus metrics. It is designed to be
integrated into inetd, providing a lightweight, freebsd-focused alternative
to the node_exporter.

# example

```
freebsd_fs_size_bytes{device="dk0",mountpoint="/"} 14573516800
freebsd_fs_used_bytes{device="dk0",mountpoint="/"} 8648296448
freebsd_fs_free_bytes{device="dk0",mountpoint="/"} 5196546048
freebsd_load1 0.000000
freebsd_load5 0.000000
freebsd_load15 0.000000
freebsd_netif_rx_bytes{interface="xennet0"} 2048990748
freebsd_netif_tx_bytes{interface="xennet0"} 98329050
freebsd_netif_errors{interface="xennet0"} 0
freebsd_netif_rx_bytes{interface="lo0"} 12998
freebsd_netif_tx_bytes{interface="lo0"} 12998
freebsd_netif_errors{interface="lo0"} 0
freebsd_mem_size_bytes 249638912
freebsd_mem_free_bytes 23531520
freebsd_mem_active_bytes 79384576
freebsd_mem_inactive_bytes 48852992
freebsd_mem_paging_bytes 0
freebsd_mem_wired_bytes 16723968
freebsd_mem_swap_size_bytes 2147442688
freebsd_mem_swap_used_bytes 6533120
freebsd_dk_read_bytes{device="xbd0"} 307225201664
freebsd_dk_write_bytes{device="xbd0"} 33737978368
freebsd_dk_read_bytes{device="dk0"} 305721165824
freebsd_dk_write_bytes{device="dk0"} 32393216512
freebsd_dk_read_bytes{device="dk1"} 1503978496
freebsd_dk_write_bytes{device="dk1"} 1344761856
```
