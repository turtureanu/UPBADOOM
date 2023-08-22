#/bin/sh

# Prevent terminating the PBA program
stty intr undef
stty kill undef
stty quit undef
stty susp undef
stty swtch undef

clear
/usr/sbin/upbadoom
