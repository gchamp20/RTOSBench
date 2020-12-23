#!/usr/bin/env bash

usage() {
	echo "Usage: deploy-sd [-k|--keep-mount] [DEVICE]"
}

read_mountpoint() {
	lsblk --noheadings --output MOUNTPOINT "$1"
}

mount_to_tempdir() {
	MOUNTPOINT=$(mktemp --directory)
	sudo mount -t vfat "$1" "${MOUNTPOINT}" -o umask=000
	echo "${MOUNTPOINT}"
}

deploy() {
	MOUNTPOINT="${1}"
	FILE="${2}"
	cp --verbose "${FILE}" "${MOUNTPOINT}"
	sync
}

while [[ $# -gt 0 ]]
do
	key="${1}"
	case $key in
	-h|--help)
		usage
		exit 0
		;;
	-k|--keep-mount)
		shift
		KEEP_MOUNT=1
		;;
	*)
		POSITIONAL+=("$1")
		shift
		;;
	esac
done

set -- "${POSITIONAL[@]}"
if [[ -n $1 ]]; then
	DEVICE_TO_DEPLOY=$1
else
	usage
	exit 1
fi

if [[ -n $2 ]]; then
	FILE_TO_DEPLOY=$2
else
	usage
	exit 1
fi


if [ ! -b "${DEVICE_TO_DEPLOY}" ]; then
	echo "${DEVICE_TO_DEPLOY} is not a block device, cannot format."
fi

MOUNTPOINT=$(read_mountpoint "${DEVICE_TO_DEPLOY}")
if [ -z "${MOUNTPOINT}" ]; then
	MOUNTPOINT=$(mount_to_tempdir "${DEVICE_TO_DEPLOY}")
fi

if [ -z "${KEEP_MOUNT}" ]; then
	trap 'sudo umount ${MOUNTPOINT}; rm -rf ${MOUNTPOINT}' err exit
else
	echo "${DEVICE_TO_DEPLOY} mounted to ${MOUNTPOINT}"
fi

deploy "${MOUNTPOINT}" "${FILE_TO_DEPLOY}"
