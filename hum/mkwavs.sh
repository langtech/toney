#! /bin/bash

TARGETS_FILE=./kirikiri.targets
AUDIO_FILE=../samples/kirikiri.wav

cat $TARGETS_FILE | (
    cntr=0
    while read a b c; do
	outfile=$cntr-`echo $c | sed -e 's/"//g' -e 's/:X//'`.wav
	./synth $AUDIO_FILE $a $b $outfile
	cntr=$((cntr + 1))
    done
)