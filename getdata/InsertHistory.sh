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

#+---------+-------------+------+-----+---------+-------+
#| Field   | Type        | Null | Key | Default | Extra |
#+---------+-------------+------+-----+---------+-------+
#| Hticker | varchar(20) | NO   | PRI | NULL    |       |
#| Hdate   | date        | NO   | PRI | NULL    |       |
#| Hopen   | double      | NO   |     | 0       |       |
#| Hhigh   | double      | NO   |     | 0       |       |
#| Hlow    | double      | NO   |     | 0       |       |
#| Hclose  | double      | NO   |     | 0       |       |
#| Hvolume | int(11)     | NO   |     | 0       |       |
#+---------+-------------+------+-----+---------+-------+


if [ "$4" = '' ]
then
	echo "USAGE: InsertHistory.sh TICKER  DATE  PRICE  VOLUME"
	exit 1
fi

TICKER=$1
DATE=$2
PRICE=$3
VOLUME=$4

DATA=/var/local/invest/history.dat
SCRIPT=/var/local/tmp/fix_slast.sql

echo "$TICKER|$DATE|$PRICE|$PRICE|$PRICE|$PRICE|$VOLUME" > $DATA
#cat $DATA
mysql -D invest < ../sql/LoadHistory.sql

echo "update stock set Slast = (select max(Hdate) from history where history.Hticker = stock.Sticker) where Sticker = '$TICKER';" > $SCRIPT
echo "update stock set Sclose = (select Hclose from history where history.Hticker = stock.Sticker and history.Hdate = stock.Slast) where Sticker = '$TICKER';" >> $SCRIPT
echo "update stock set Shigh52 = (select max(Hhigh) from history where history.Hticker = stock.Sticker and history.Hdate > date_sub(stock.Slast, interval 52 week) ) where Sticker = '$TICKER';" >> $SCRIPT
echo "update stock set Sdate52 = (select max(Hdate) from history where history.Hticker = stock.Sticker and history.Hhigh = stock.Shigh52 ) where Sticker = '$TICKER';" >> $SCRIPT
echo "show warnings ;" >> $SCRIPT
#cat $SCRIPT
mysql -D invest < $SCRIPT

UpdateAverage -ticker $TICKER Z 

CheckTicker.sh $TICKER



