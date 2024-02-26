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


TIME=`date +%s`

getfundSEC.py IBM 150.00 0000051143 CY1970Q1 $TIME
exit
getfundSEC.py CLX 0000021076 CY1970Q1 $TIME
getfundSEC.py PG 0000080424 CY1970Q1 $TIME
getfundSEC.py KR 0000056873 CY1970Q1 $TIME
getfundSEC.py FOX 0001754301 CY1970Q1 $TIME

