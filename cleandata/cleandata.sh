#!/bin/sh
#     Invest extras
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
	echo "cleandata.sh date"
	echo "  format date as yyyy-mm-dd"
	exit 1
fi 

rm -f cd.*

echo "update stock set Scheck = 'N'" | mysql -D invest

cleandata -report all $1 > cd.res

echo "select Scheck, count(*) from stock group by Scheck" | mysql -D invest

grep 'date gaps' cd.res | grep    index > cd.gaps_index
grep 'date gaps' cd.res | grep -v index > cd.gaps_not

grep 'Check last date' cd.res > cd.slast

grep 'check price break' cd.res > cd.price_break

grep 'mismatch' cd.res | grep    index > cd.data_index
grep 'mismatch' cd.res | grep -v index > cd.data_not

egrep -v 'mismatch|firstgap|date gaps|Check last date|check price break' cd.res > cd.other


