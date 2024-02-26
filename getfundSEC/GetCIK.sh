#!/bin/sh  -x
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
	echo "GetCIK.sh  symbol"
	exit 1
fi

SYMBOL=$1

curl -s -H 'User-Agent:tstevelt@silverhammersoftware.com' 'https://www.sec.gov/files/company_tickers.json' > CIK.json

# JsonTree CIK.json > CIK.TXT

cat CIK.json | sed 's/}/\n/g' > CIK.TXT

grep $SYMBOL CIK.TXT




 

