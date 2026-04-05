#!/bin/bash
echo "Lanzando EMZ-OS..."
qemu-system-i386 -drive format=raw,file=os.img > /dev/null 2>&1 &
disown
exit