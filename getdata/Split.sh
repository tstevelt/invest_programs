#!/bin/sh


#Field	Type	Null	Key	Default	Extra
#Hid	int(11)	NO	PRI	NULL	auto_increment
#Hticker	varchar(20)	NO	MUL	NULL	
#Hdate	date	NO		NULL	
#Hopen	double	NO		0	
#Hhigh	double	NO		0	
#Hlow	double	NO		0	
#Hclose	double	NO		0	
#Hvolume	int(11)	NO		0	

if [ "$3" = '' ]
then
	echo "USAGE: Split.sh TICKER DATE RATIO"
	exit 1
fi

TICKER=$1
DATE=$2
RATIO=$3

echo "update history set Hopen = Hopen / $RATIO, Hclose = Hclose / $RATIO, Hhigh = Hhigh / $RATIO, Hlow = Hlow / $RATIO,  Hvolume = Hvolume * $RATIO where Hticker = '$TICKER' and Hdate < '$DATE'" | mysql -D invest


UpdateAverage -ticker $TICKER A




