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
	echo "USAGE: hackDividends.sh TICKER"
	exit 1
fi

TICKER=$1

#https://query1.finance.yahoo.com/v8/finance/chart/IBM
#{'period1': 7223400, 'period2': 1698667374, 'interval': '1d', 'events': 'div,splits'}
#{'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.95 Safari/537.36'}

#curl -s 'https://query1.finance.yahoo.com/v8/finance/chart/IBM?period1=7223400&period2=1698667374&interval=1d&events=div,splits' 

ENDDT=`date +%s`
BEGDT=`expr $ENDDT - \( 86400 \* 365 \* 20 \)`

curl -s "https://query1.finance.yahoo.com/v8/finance/chart/$TICKER?period1=$BEGDT&period2=$ENDDT&interval=1d&events=div,splits"  > $TICKER.json

cat $TICKER.json | sed 's/}/\n/g' | grep -w date | grep -v timestamp > $TICKER.csv



