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


if [ "$2" = '' ]
then
	echo "USAGE: CheckPortfolio.sh memberID date"
	exit 1
fi

MEMBER=$1
DATE=$2


echo "select Pticker from portfolio, stock where Pticker = Sticker and Stype != 'C' and Stype != 'B' and Pmember = $MEMBER ;" | mysql -D invest | awk '{print $2}' | grep -v Pticker > Pticker.txt

for i in `cat Pticker.txt`
do
	echo "==== $i ===="
	cleandata -report $i $DATE
done

