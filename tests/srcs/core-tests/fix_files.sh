#! /bin/sh
sed -i 's:module \$.* binary:module binary:g' *.bin.wast
sed -i 's:module binary \":module binary\n \":g' *.bin.wast
sed -i 's:(module binary):(module binary\n):g' *.bin.wast
