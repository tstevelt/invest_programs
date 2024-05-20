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
	echo "USAGE: LoadNasdaq.sh  file"
	exit 1
fi

FILE=$1

echo "update stock set Snasdaq = 'N' where Snasdaq = 'Y';" | mysql -D invest

rm -f script

for i in `cat $FILE`
do
	echo "update stock set Snasdaq = 'Y' where Sticker = '$i';" >> script
done

mysql -D invest < script


	
