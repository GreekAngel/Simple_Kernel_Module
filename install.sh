#!/usr/bin/env bash

echo '[+] Compiling the module...'
echo ''
make
echo ''
echo 'Injecting the module...'
echo ''
insmod module.ko
echo ''
echo 'Module has been injected.'
echo ''
echo 'Use cat /dev/module and tail -f /var/log/kern.log'
echo ''
