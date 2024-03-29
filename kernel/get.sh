#!/bin/bash

echo "getting the latest version of the linux kernel"

git clone --depth 1 https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git
# git clone --progress -j 0 https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git

echo "start with a default configuration"

cd linux || return
make x86_64_defconfig
