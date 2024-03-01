#!/bin/sh
#     Programs called by invest.cgi
# 
#     Copyright (C)  2019 - 2024 Tom Stevelt
# 
#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU Affero General Public License as
#     published by the Free Software Foundation, either version 3 of the
#     License, or (at your option) any later version.
# 
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU Affero General Public License for more details.
# 
#     You should have received a copy of the GNU Affero General Public License
#     along with this program.  If not, see <https://www.gnu.org/licenses/>.

if [ "$1" = '' -o ! -f "$1" ]
then
	echo "USAGE: GoList.sh  tickerlist"
	exit 1
fi

LIST=$1

COUNT=0

for TICKER in `cat $LIST`
do
	COUNT=`expr $COUNT + 1`
	Fundamentals -g $TICKER > $TICKER.raw
	Fundamentals -p $TICKER > $TICKER.txt
	if [ $COUNT = 1 ]
	then
		Fundamentals -c $TICKER -f > GoList.csv
	else
		Fundamentals -c $TICKER -f | tail -1 | grep -v GRSPCT >> GoList.csv
	fi
done

