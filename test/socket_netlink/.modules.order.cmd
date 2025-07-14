cmd_/home/bhien/edr/test/socket_netlink/modules.order := {   echo /home/bhien/edr/test/socket_netlink/netlink.ko; :; } | awk '!x[$$0]++' - > /home/bhien/edr/test/socket_netlink/modules.order
