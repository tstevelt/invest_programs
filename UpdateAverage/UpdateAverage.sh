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



echo "History"
echo "select Hdate, count(*) from history group by Hdate " | mysql -D invest | tail -20

echo "Average"
echo "select Adate, count(*) from average group by Adate " | mysql -D invest | tail -20

exit 0


#USAGE: UpdateAverage -index index   mode [-d#]
#USAGE: UpdateAverage -ticker TICKER mode [-d#]
#index:
# S = S&P 500
# D = Dow Jones Industrials
# N = Nasdaq 100
# M = Midcap (Russell 1000 ex S&P 500)
# 1 = Russell 1000
# 2 = Russell 2000
# 3 = Russell 3000
# A = All stocks
# O = Not indexed
# mode = A for all, Z for zeros
# -d1 or -d2 = debug level, no db update

UpdateAverage -index A Z 




