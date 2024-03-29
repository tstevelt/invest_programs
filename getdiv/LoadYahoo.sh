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

if [ "$1" = '' ]
then
	echo "LoadYahooDividends.sh TICKER"
	exit 1
fi

TICKER=$1

# getStock.py $TICKER 10y 1d

cat $TICKER.csv | sed "s/^/$TICKER,/" > $TICKER.dat

echo "load data local infile '$TICKER.dat' into table dividend " >> script
#echo "ignore 1 lines " >> script
echo "fields terminated by ',' " >> script
echo "lines terminated by '\n' " >> script
echo "(Dticker, Dexdate, Damount ) ;" >> script

echo "show warnings ;" >> script

echo "select count(*) from dividend where Dticker = '$TICKER';" >> script

mysql -D invest < script

#rm -f script

