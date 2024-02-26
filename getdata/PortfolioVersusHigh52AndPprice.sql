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
select Sticker, Sname, Shigh52, Pshares, Hclose, 100*Hclose/Shigh52 as PCNT, Pprice, 100*Hclose/Pprice
  from stock, history, portfolio 
 where Sticker = Pticker  
   and Sticker = Hticker 
   and Pmember = 501 
   and Hdate = '2021-09-20' 
 order by PCNT ;



