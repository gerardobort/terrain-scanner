#!/bin/sh
export IN="$1"
export OUT=`echo "$1" | sed 's/ /_/g'`
echo "IN=$IN"
echo "OUT=$OUT"
convert "$IN" -scale $2! $OUT
