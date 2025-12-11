#!/usr/bin/env bash

gcc -DDEPTH=$1 -DBREADTH=$2 -DLIBRARY=$3 \
    main.c library_interface.c tree.c \
    libraries/lib/parson.c -Ilibraries/include -Llibraries/lib \
    -lmxml4 -lfyaml -ljansson -lcjson -lmsgpack-c #-lflatcc

./a.out
