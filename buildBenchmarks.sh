#!/bin/sh

for PROG in adpcm_decode adpcm_encode aes basicmath blowfish crc fft limits qsort randmath rc4
do
	echo "Building mibench2 benchmark $PROG"
	cd $PROG
	make GFE_TARGET=$1 clean
	make GFE_TARGET=$1
	cp main.elf ../$PROG.elf
	make GFE_TARGET=$1 clean
	cd ..
	echo '`-> Done'
done


