# freebsd_exporter

The freebsd_exporter retrieves system metrics such as disk I/O, network I/O, RAM and filesystem usage, as well as CPU load from the running system and exposes them in the format of Prometheus metrics. It is designed to be integrated into inetd, providing a lightweight, FreeBSD-focused alternative to the node_exporter.