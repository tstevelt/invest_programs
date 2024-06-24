
select Hticker, count(*) from history group by Hticker ;
select Hdate, count(*) from history where Hdate > '2024-06-01' group by Hdate ;


select Aticker, count(*) from average group by Aticker ;
select Adate, count(*) from average where Adate > '2024-06-01' group by Adate ;

