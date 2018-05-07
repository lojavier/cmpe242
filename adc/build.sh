#!/bin/sh

DIR=`pwd`

make clean

rsync -avz --delete --exclude 'build.sh' ${DIR} pi@192.168.1.242:~/cmpe242/