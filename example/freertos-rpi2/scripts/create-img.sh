#!/usr/bin/env bash

EXE_NAME=kernel7.elf
START_ADDR=0x00008000

#arm-none-eabi-objcopy -R -S --strip-debug -O binary "$EXE_NAME" "$EXE_NAME.bin" || exit 1

#gzip -9 >"$EXE_NAME.gz" < "$EXE_NAME.bin"

#mkimage \
#  -A arm -O rtems -T kernel -a $START_ADDR -e $START_ADDR -n "RTEMS" \
#  -d "$EXE_NAME.gz" "$EXE_NAME.img"

qemu-system-arm -M raspi2 \
  -serial null -serial stdio \
  -kernel "$EXE_NAME.img"
