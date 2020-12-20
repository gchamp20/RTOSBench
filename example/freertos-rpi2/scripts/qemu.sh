#!/usr/bin/env bash

QEMU=qemu-system-arm

usage() {
	echo "Usage $(basename ${0}) [ELF FILE]"
}

parse_args() {
	while [[ $# -gt 0 ]]
	do
		key="${1}"
		case $key in
			-h|--help)
				usage
				exit 0
				;;
			*)
				POSITIONAL+=("$1")
				shift
				;;
		esac
	done
}

parse_args "${@}"

if ! command -v "${QEMU}" &> /dev/null
then
	echo "${QEMU} not installed or available." >&2
	exit 1
fi

if [ -n "${1}" ]
then
	elf="${1}"

	if ! [ -e "${elf}" ]
	then
		echo "file ${elf} doesn't exists." >&2
		exit 1
	fi

	if ! file -b ${elf} | grep --quiet -i elf
	then
		echo "$(basename ${elf}) doesn't appear to be an ELF file." >&2
		exit 1
	fi
else
	usage
	exit 1
fi

echo "Starting QEMU (Ctrl-A, X to quit)"
echo ""
"${QEMU}" -M raspi2 -serial null -serial mon:stdio -kernel "${elf}" -nographic
