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


echo "select Sticker, Scik, Sname" > script
echo "  from stock" >> script
echo " where Stype = 'E'" >> script
echo " order by Sname;" >> script

mysql -D invest < script > x1

cat x1 | awk '{print $6}' | textp -u | sort -u > x2

for i in `cat x2`
do
	echo "$i `grep -w -i -c $i x1`" 
done > x3

sort -rn -k 2 x3 > x4

vi x4


