#!/bin/bash

if [ $# -lt 2 ]; then
    echo "usage: ./filter.sh dir file [files]"
    exit -1
fi

__base_dir=$1; shift

if [ ! -d "$__base_dir" ]; then
	echo need "\"$__base_dir\""
	exit
fi

cd "$__base_dir"

while [ -n "$*" ];
do
	find . -name $1 | awk -F "/" '{
		printf("%s", $0);
		sub(/proj/," ", $3);
		i = 0;
		while (sub(/\./, " ", $3)) {
			i ++;
		}
		printf("%s", $3);
		for (; i < 3; i ++) {
			printf(" %d", 0);
		}
		printf("\n");
	}' | sort -n -k1 -k2 -k3 -k4 | awk -F " " '{print $1}' | xargs ls -l -h -U
	shift
done

