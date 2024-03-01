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

DATE=`date +%s`

# echo "select Sticker, Sclose, Scik from stock where Scik like '00%' and Stype = 'E'" | mysql -D invest


getfundETF.py AAAU      19.18 0001708646 CY197100 $DATE
exit
getfundETF.py AMJ       25.03 0000019617 CY197100 $DATE
getfundETF.py AMND    38.8718 0001114446 CY197100 $DATE
getfundETF.py AMTR    50.4184 0001114446 CY197100 $DATE
getfundETF.py AMUB      15.74 0001114446 CY197100 $DATE
getfundETF.py ASGI      16.22 0001793855 CY197100 $DATE
getfundETF.py ATMP    21.0802 0000312070 CY197100 $DATE
getfundETF.py BAR       19.15 0001690437 CY197100 $DATE
getfundETF.py BDCZ    17.9788 0001114446 CY197100 $DATE
getfundETF.py BDRY        5.7 0001610940 CY197100 $DATE
getfundETF.py BNO        28.9 0001472494 CY197100 $DATE
getfundETF.py GLD      181.49 0001222333 CY197100 $DATE


