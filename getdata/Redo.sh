#!/bin/sh -x

if [ "$1" = '' ]
then	
	echo "Redo.sh yyyy-mm-dd"
	exit 1
fi

DATE=$1

rm -f /var/local/tmp/getdata*

echo "delete from history where Hdate = '$DATE';" > script
echo "show warnings;" >> script

echo "delete from average where Adate = '$DATE';" >> script
echo "show warnings;" >> script

echo "update stock set Slast = (select max(Hdate) from history where history.Hticker = stock.Sticker);" >> script
echo "show warnings;" >> script


mysql -D invest < script


