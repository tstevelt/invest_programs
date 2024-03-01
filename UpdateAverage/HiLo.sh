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



SCRIPT=/var/local/tmp/HiLo.script
TEXT=/var/local/tmp/HiLo.txt

echo "select max(Slast) from stock" > $SCRIPT
DATE=`mysql -D invest < $SCRIPT | grep 20 | awk '{print $2}'`

echo "select 'RUSSELL 1000 OVERSOLD BY BOLLINGER AND RSI';"  > $SCRIPT
echo "select Sticker, Sname, Arsi, Format ( 100.0 * (Sclose-Ama50) / Ama50, 2)  'Percent'"  >> $SCRIPT
echo "  from stock, average"  >> $SCRIPT
echo " where Sticker = Aticker"  >> $SCRIPT
echo "   and Srussell = '1'"  >> $SCRIPT
echo "   and Adate = '$DATE'"  >> $SCRIPT
echo "   and Sclose < Ama50 - Astddev * 2.0 "  >> $SCRIPT
echo "   and Arsi < 30.0"  >> $SCRIPT
echo "   and (100.0 * (Sclose-Ama50) / Ama50) < -20.0 ;"  >> $SCRIPT
echo "select 'RUSSELL 1000 OVERBOUGHT BY BOLLINGER AND RSI';"  >> $SCRIPT
echo "select Sticker, Sname, Arsi, Format ( 100.0 * (Sclose-Ama50) / Ama50, 2)  'Percent'"  >> $SCRIPT
echo "  from stock, average"  >> $SCRIPT
echo " where Sticker = Aticker"  >> $SCRIPT
echo "   and Srussell = '1'"  >> $SCRIPT
echo "   and Adate = '$DATE'"  >> $SCRIPT
echo "   and Sclose > Ama50 - Astddev * 2.0 "  >> $SCRIPT
echo "   and Arsi > 70.0"  >> $SCRIPT
echo "   and (100.0 * (Sclose-Ama50) / Ama50) > 20.0 ;"  >> $SCRIPT

mysql -D invest < $SCRIPT > $TEXT

SendAttached -to:tms@stevelt.com -from:tms@stevelt.com -s:HighLowReport -a:$TEXT




