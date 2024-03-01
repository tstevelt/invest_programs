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
	echo "USAGE: GetMissingFromTiingo.sh date"
	exit 1
fi

DATE=$1

echo "select Sticker, Sname from stock where Stype != 'C' and Stype != 'B' and (Slast < '$DATE' or Sclose is null or Shigh52 is null);" | mysql -D invest | grep -v Sticker | sort -u > report.txt

echo "select Sticker from stock where Stype != 'C' and Stype != 'B' and (Slast < '$DATE' or Sclose is null or Shigh52 is null);" | mysql -D invest | awk '{print $2}' | grep -v Sticker | sort -u > tickers.txt

for i in `cat tickers.txt`
do
	echo "===== $i ====="
	getdata -one $i -fmt csv -tiingo
	if [ $? = 0 ]
	then
		UpdateAverage -ticker $i Z
	fi
done

