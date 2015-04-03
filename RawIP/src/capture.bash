#!/bin/bash
#
# In The Name Of God
# ========================================
# [] File Name : capture.bash
#
# [] Creation Date : 03-04-2015
#
# [] Last Modified : Sat 04 Apr 2015 12:14:52 AM IRDT
#
# [] Created By : Parham Alvani (parham.alvani@gmail.com)
# =======================================

tcpdump -i lo -s 65535 -w $(date +"%F_%H:%m:%S").pcap
