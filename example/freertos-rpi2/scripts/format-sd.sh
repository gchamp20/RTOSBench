#!/usr/bin/env bash

usage() {
	echo "Usage: format-sd [-n|--no-prompt] [DEVICE]"
}

mount_to_tempdir() {
	MOUNTPOINT=$(mktemp --directory)
	sudo mount -t vfat "$1" "${MOUNTPOINT}" -o umask=000
	echo "${MOUNTPOINT}"
}

while [[ $# -gt 0 ]]
do
	key="$1"
	case $key in
	-h|--help)
		usage
		exit 0
		;;
	-n|--no-prompt)
		SKIP_PROMPT=1
		shift
		;;
	*)
		POSITIONAL+=("$1")
		shift
		;;
	esac
done

set -- "${POSITIONAL[@]}"
if [[ -n $1 ]]; then
	DEVICE_TO_FORMAT=$1
else
	usage
	exit 1
fi

if [ ! -b "${DEVICE_TO_FORMAT}" ]; then
	echo "${DEVICE_TO_FORMAT} is not a block device, cannot format."
fi

if [ -z ${SKIP_PROMPT} ]; then
	echo "Will format ${DEVICE_TO_FORMAT} to fat32. Continue? [y/N]"
	read -rsn 1 answer
	if [ ! "${answer,,}" = "y" ]; then
		echo "No"
		exit 0
	fi
fi

sudo umount "${DEVICE_TO_FORMAT}"
sudo mkfs.vfat -F 32 "${DEVICE_TO_FORMAT}"

MOUNTPOINT=$(mount_to_tempdir "${DEVICE_TO_FORMAT}")
trap 'sudo umount ${MOUNTPOINT}' err exit

wget --quiet -O "${MOUNTPOINT}"/bootcode.bin https://github.com/raspberrypi/firmware/raw/master/boot/bootcode.bin
wget --quiet -O "${MOUNTPOINT}"/start.elf https://github.com/raspberrypi/firmware/raw/master/boot/start.elf
