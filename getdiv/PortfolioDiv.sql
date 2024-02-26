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
-- 
-- +---------+------------------+------+-----+---------+----------------+
-- | Field   | Type             | Null | Key | Default | Extra          |
-- +---------+------------------+------+-----+---------+----------------+
-- | id      | int(11) unsigned | NO   | PRI | NULL    | auto_increment |
-- | Dticker | varchar(20)      | NO   | MUL |         |                |
-- | Dexdate | date             | NO   |     | NULL    |                |
-- | Damount | double           | NO   |     | 0       |                |
-- | Dcurr   | char(4)          | NO   |     | USD     |                |
-- +---------+------------------+------+-----+---------+----------------+
-- 

select Dticker, Dexdate, Damount
  from dividend, stock, portfolio
 where Dticker = Sticker
   and Pticker = Sticker
   and Pmember = 501
 order by Dticker, Dexdate ;

