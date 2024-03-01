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


if [ "$2" = '' ]
then
	echo "TestFundamentalUpdate.sh  ticker date"
	exit 1
fi

TICKER=$1
DATE=$2

rm -f script
#echo "select Fticker, Fmktcap, Fdividend, Fyield, Fepsttm, Fpettm, Fepsfwd, Fpefwd, Fbvps, Fpb, Fupdated" > script
#echo "  from fundamental where Fticker = '$TICKER';" >> script
#echo "select * from history where Hticker = '$TICKER' and Hdate = '$DATE';" >> script
#echo "select * from average where Aticker = '$TICKER' and Adate = '$DATE';" >> script
echo "delete from history where Hticker = '$TICKER' and Hdate = '$DATE';" >> script
echo "delete from average where Aticker = '$TICKER' and Adate = '$DATE';" >> script
echo "update stock set Slast = (select max(Hdate) from history where Hticker = '$TICKER') where Sticker = '$TICKER';" >> script
mysql -D invest < script 

echo "getdata ..."
getdata -one $TICKER -fmt csv

echo "update average ..."
UpdateAverage -ticker $TICKER Z

rm -f script
echo "select Fticker, Fmktcap, Fdividend, Fyield, Fepsttm, Fpettm, Fepsfwd, Fpefwd, Fbvps, Fpb, Fupdated" > script
echo "  from fundamental where Fticker = '$TICKER';" >> script
echo "select * from history where Hticker = '$TICKER' and Hdate = '$DATE';" >> script
echo "select * from average where Aticker = '$TICKER' and Adate = '$DATE';" >> script
#mysql -D invest < script 
