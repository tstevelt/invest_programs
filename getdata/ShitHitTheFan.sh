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
	echo "ShitHitTheFan.sh YESTER_DATE"
	exit 1
fi

echo "select Sticker from stock where Slast < '$1';" | mysql -D invest | awk -F '|' '{print $2}' | grep -v Sticker > tickers.txt

for i in `cat tickers.txt`
do
	getdata -one $i -fmt json -q
done

echo "Done with getdata, check Admin page"
echo "Press [Enter] to continue, or [Break] to quit"
read x


/usr/local/bin/GetDataAndUpdate.sh -noget

