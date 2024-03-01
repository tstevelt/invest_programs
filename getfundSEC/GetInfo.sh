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

##
## see: https://www.sec.gov/edgar/sec-api-documentation
## note: site has rate limit of 10/second
##

if [ "$1" = '' ]
then
	echo "GetInfo.sh  CIK"
	exit 1
fi

CIK=$1

LENGTH=`echo -n $CIK | wc -c`

if [ ! "$LENGTH" = '10' ]
then
	echo "CIK must be ten characters. Include leading ZEROS"
	exit 1
fi
 
curl -s --user-agent tstevelt@silverhammersoftware.com "https://data.sec.gov/api/xbrl/companyfacts/CIK$CIK.json" > $CIK.json

# | sed 's/}/\n/g' > $CIK.txt

# JsonTree $CIK.json




