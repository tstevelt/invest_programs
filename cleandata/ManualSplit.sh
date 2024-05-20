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
	echo "USAGE: ManualSplit.sh ticker ratio"
	exit 1
fi

TICKER=$1
RATIO=$2

echo "update history set Hopen = Hopen / $RATIO, Hlow = Hlow / $RATIO, Hhigh = Hhigh / $RATIO, Hclose = Hclose / $RATIO" > script
echo " where Hticker = '$TICKER';" >> script

mysql -D invest <  script

UpdateAverage -ticker $TICKER A


