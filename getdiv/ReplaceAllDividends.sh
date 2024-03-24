#!/bin/sh -x

if [ "$1" = '' ]
then
	echo "USAGE: ReplaceAllDividends.sh ticker"
	exit 1
fi

TICKER=$1

echo "delete from dividend where Dticker = '$TICKER'" | mysql -D invest

getdiv -ticker $TICKER -all


