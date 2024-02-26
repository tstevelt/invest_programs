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

-- select 'Ama5', count(*) from average where Adate > '2022-09-22' and Ama5 <= 0.0001;
-- select 'Ama10', count(*) from average where Adate > '2022-09-22' and Ama10 <= 0.0001;
-- select 'Ama20', count(*) from average where Adate > '2022-09-22' and Ama20 <= 0.0001;
-- select 'Ama50', count(*) from average where Adate > '2022-09-22' and Ama50 <= 0.0001;
-- select 'Ama100', count(*) from average where Adate > '2022-09-22' and Ama100 <= 0.0001;
-- select 'Ama200', count(*) from average where Adate > '2022-09-22' and Ama200 <= 0.0001;
-- select 'Avol50', count(*) from average where Adate > '2022-09-22' and Avol50 <= 0.0001;
-- select 'Arsi', count(*) from average where Adate > '2022-09-22' and Arsi <= 0.0001;
-- select 'Astddev', count(*) from average where Adate > '2022-09-22' and Astddev <= 0.0001;

-- select Aticker, Adate, Arsi  from average where Adate > '2022-09-22' and Arsi <= 0.0001;
-- select Adate, count(*)  from average where Adate > '2022-09-22' and Arsi <= 0.0001 group by Adate;
-- select min(Adate), max(Adate) from average where Adate > '2022-09-22' and Arsi <= 0.0001 ;

select Aticker, Adate, Astddev  from average where Adate > '2022-09-22' and Astddev <= 0.0001;
select Adate, count(*)  from average where Adate > '2022-09-22' and Astddev <= 0.0001 group by Adate;
select min(Adate), max(Adate) from average where Adate > '2022-09-22' and Astddev <= 0.0001 ;

