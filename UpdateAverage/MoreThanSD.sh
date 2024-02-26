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
	echo "USAGE: MoreThanSD.sh ticker NumberOfStdDev"
	exit 1
fi

TICKER=$1
BAND=$2

echo "select count(*) from average where Aticker = '$TICKER'" | mysql -D invest

echo "select 'ABOVE', count(*) from average, history where Aticker = Hticker and Adate = Hdate and Aticker = '$TICKER' and Hhigh > Ama50 + $BAND * Astddev" | mysql -D invest

echo "select 'BELOW', count(*) from average, history where Aticker = Hticker and Adate = Hdate and Aticker = '$TICKER' and Hlow < Ama50 - $BAND * Astddev" | mysql -D invest
