#!/bin/bash
#
# In The Name Of God
# ========================================
# [] File Name : capture.bash
#
# [] Creation Date : 03-04-2015
#
# [] Last Modified : Fri 03 Apr 2015 11:58:13 PM IRDT
#
# [] Created By : Parham Alvani (parham.alvani@gmail.com)
# =======================================

dumpcap -i lo -w $(date +"%F_%H:%m:%S").pcap
