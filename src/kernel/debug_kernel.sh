#!/bin/sh

objcopy --only-keep-debug MoltarOS.kernel kernel.sym
nm $1 | grep " T " | awk '{ print $1" "$3 }' > $1.sym
