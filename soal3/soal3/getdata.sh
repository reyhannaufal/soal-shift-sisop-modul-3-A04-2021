#!/bin/bash

FILE=$1
while read LINE; do
	`echo "$LINE"`
	#`ls -l $LINE | awk '{print $5 " " $9}' | sort -nr | awk '{print $1/1024"M " $2}' > getstat`
done < $FILE
