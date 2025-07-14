cmd_/home/bhien/edr/Module.symvers := sed 's/\.ko$$/\.o/' /home/bhien/edr/modules.order | scripts/mod/modpost -m -a  -o /home/bhien/edr/Module.symvers -e -i Module.symvers   -T -
