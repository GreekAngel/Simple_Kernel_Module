#!/usr/bin/env bash

echo ''
echo 'Unloading kernel module...'
echo ''
rmmod module
echo ''
echo 'Module has been unloaded...'
echo ''
echo 'Removing compiled files...'
echo ''
make clean
echo ''
echo 'Compiled files has been removed.'
echo ''
