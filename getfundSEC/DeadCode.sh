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
	echo "DeadCode.sh prog.py"
	exit 1
fi

if [ ! -f $1 ]
then
	echo "program $1 not found"
	exit 1
fi

for function in `grep def $1 | awk '{print $2}' | sed 's/(.*$//'`
do
	#echo "=== $function ==="
	#grep -w $function $1
	COUNT=`grep -wc $function $1`
	if [ "$COUNT" = '1' ]
	then
		echo "$function not used"
	fi
done

