#!/bin/sh

if [ "$1" = '' ]
then
	echo "DeleteNotOwned.sh ticker"
	exit 1
fi

TICKER=$1

COUNT=`echo "select count(*) from portfolio where Pticker = '$TICKER';" | mysql -D invest | textp -g 4 4 | awk '{print $2}'`
if [ ! "$COUNT" = '0' ]
then
	echo "Stock $TICKER is in portfolio"
	exit 1 
fi

DeleteChild ()
{
	Table=$1
	Field=$2
	Ticker=$3

	echo "delete from $Table where $Field = '$Ticker';"  | mysql -D invest
}

echo "$TICKER"
DeleteChild average Aticker $TICKER
DeleteChild crypto Cticker $TICKER
DeleteChild dividend Dticker $TICKER
# DeleteChild fundamental Fticker $TICKER
DeleteChild history Hticker $TICKER
DeleteChild portfolio Pticker $TICKER
DeleteChild watchlist Wticker $TICKER

