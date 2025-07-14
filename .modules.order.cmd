cmd_/home/bhien/edr/modules.order := {   echo /home/bhien/edr/edr_module.ko; :; } | awk '!x[$$0]++' - > /home/bhien/edr/modules.order
