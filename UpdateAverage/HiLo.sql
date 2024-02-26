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

select 'RUSSELL 1000 OVERSOLD BY BOLLINGER AND RSI';

select Sticker, Sname, Arsi, Format ( 100.0 * (Sclose-Ama50) / Ama50, 2)  'Percent'
  from stock, average
 where Sticker = Aticker
   and Srussell = '1'
   and Adate = '2023-09-29'
   and Sclose < Ama50 - Astddev * 2.0 
   and Arsi < 30.0
;

select 'RUSSELL 1000 OVERBOUGHT BY BOLLINGER AND RSI';

select Sticker, Sname, Arsi, Format ( 100.0 * (Sclose-Ama50) / Ama50, 2)  'Percent'
  from stock, average
 where Sticker = Aticker
   and Srussell = '1'
   and Adate = '2023-09-29'
   and Sclose > Ama50 - Astddev * 2.0 
   and Arsi > 70.0
;

