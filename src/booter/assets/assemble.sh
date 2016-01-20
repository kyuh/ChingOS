#!/bin/bash

cat *.data > datas.bin

cp assets_head.template assets.h

files=*.data

for file in $files
do
    filesize=$(stat -c%s "$file")
    filename="${file%.*}"
    echo "  char $filename[$filesize];" >> assets.h
done

cat assets_tail.template >> assets.h


cp assets.h ../assets.h
cp datas.bin ../datas.bin
