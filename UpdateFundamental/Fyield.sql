--     Programs called by invest.cgi
-- 
--     Copyright (C)  2019 - 2024 Tom Stevelt
-- 
--     This program is free software: you can redistribute it and/or modify
--     it under the terms of the GNU Affero General Public License as
--     published by the Free Software Foundation, either version 3 of the
--     License, or (at your option) any later version.
-- 
--     This program is distributed in the hope that it will be useful,
--     but WITHOUT ANY WARRANTY; without even the implied warranty of
--     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--     GNU Affero General Public License for more details.
-- 
--     You should have received a copy of the GNU Affero General Public License
--     along with this program.  If not, see <https://www.gnu.org/licenses/>.
select Ssp500, avg(Fyield) 
  from fundamental,stock 
 where Fticker = Sticker 
   and Sdj = 'N' 
   and Fyield > 0 
 group by Ssp500 ;


select Fticker, Fyield, Fdividend, Fshares from fundamental where Fyield > 1000.0;

