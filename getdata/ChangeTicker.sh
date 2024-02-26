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

shopt -s xpg_echo

if [ "$2" = '' ]
then
	echo "ChangeTicker.sh  old  new"
	exit 1
fi

echo "New name: \c"
read NewName
#echo "$NewName"

echo "select Stype, Stype2, Sdj, Ssp500, Srussell, Snasdaq, Sdomfor, Scik from stock where Sticker = '$1'" > script
echo "  into @STYPE, @STYPE2, @SDJ, @SSP500, @SRUSSELL, @SNASDAQ, @SDOMFOR, @SCIK;" >> script

echo "select Ssector, Sindustry, Sexchange from stock where Sticker = '$1'" >> script
echo "  into @SSECTOR, @SINDUSTRY, @SEXCHANGE;" >> script

echo "select Slast, Sclose, Shigh52, Sdate52 from stock where Sticker = '$1'" >> script
echo "  into @SLAST, @SCLOSE, @SHIGH52, @SDATE52;" >> script


echo "insert into stock (Sticker,Sname,Stype, Stype2, Sdj, Ssp500, Srussell, Snasdaq, Sdomfor, Scik)" >> script
echo " values ('$2','$NewName', @STYPE, @STYPE2, @SDJ, @SSP500, @SRUSSELL, @SNASDAQ, @SDOMFOR, @SCIK) ;" >> script

echo "update stock set Ssector = @SSECTOR, Sindustry = @SINDUSTRY where Sticker = '$2';" >> script
echo "update stock set Sexchange = @SEXCHANGE where Sticker = '$2';" >> script
echo "update stock set Slast = @SLAST, Sclose = @SCLOSE, Shigh52 = @SHIGH52, Sdate52 = @SDATE52 where Sticker = '$2';" >> script

echo "select * from stock where Sticker = '$1';" >> script
echo "select * from stock where Sticker = '$2';" >> script

mysql -D invest < script

echo "Press ENTER TO CONTINUE: MOVE CHILD RECORDS AND DELETING OLD STOCK $1"
read x

echo "update average     set Aticker = '$2' where Aticker = '$1' ;" > script
echo "update history     set Hticker = '$2' where Hticker = '$1' ;" >> script
echo "update crypto      set Cticker = '$2' where Cticker = '$1' ;" >> script
echo "update dividend    set Dticker = '$2' where Dticker = '$1' ;" >> script
echo "update portfolio   set Pticker = '$2' where Pticker = '$1' ;" >> script
echo "update temp        set Tticker = '$2' where Tticker = '$1' ;" >> script
echo "update watchlist   set Wticker = '$2' where Wticker = '$1' ;" >> script
echo "update fundamental set Fticker = '$2' where Fticker = '$1' ;" >> script
echo "delete from stock where Sticker = '$1' ;" >> script

#echo "update stock set Slast = (select max(Hdate) from history where history.Hticker = stock.Sticker) where Sticker = '$2';" >> script

#echo "update stock set Sclose = (select Hclose from history where history.Hticker = stock.Sticker and history.Hdate = stock.Slast) where Sticker = '$2';" >> script

#echo "update stock set Shigh52 = (select max(Hhigh) from history where history.Hticker = stock.Sticker and history.Hdate > date_sub(stock.Slast, interval 52 week) ) where Sticker = '$2';" >> script

#echo "update stock set Sdate52 = (select max(Hdate) from history where history.Hticker = stock.Sticker and history.Hhigh = stock.Shigh52  ) where Sticker = '$2';" >> script

#cat script
mysql -D invest < script

getdata -one $2 -2yr 

UpdateAverage -ticker $2 Z


