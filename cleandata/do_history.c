/*----------------------------------------------------------------------------
	Program : cleandata/do_history.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Delete selected history, average and market records, by date.
	Return  : 
----------------------------------------------------------------------------*/
//     Invest extras
// 
//     Copyright (C)  2019 - 2024 Tom Stevelt
// 
//     This program is free software: you can redistribute it and/or modify
//     it under the terms of the GNU Affero General Public License as
//     published by the Free Software Foundation, either version 3 of the
//     License, or (at your option) any later version.
// 
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU Affero General Public License for more details.
// 
//     You should have received a copy of the GNU Affero General Public License
//     along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include	"cleandata.h"


void do_history ()
{
	long		Affected = 0;

	sprintf ( Statement, "delete from history where Hdate %s '%s'", ActionCode, SelectDate );
	Affected = dbyDelete ( "cleandata", &MySql, Statement, 0, LogFileName );
	printf ( "Deleted %ld history records\n", Affected );

	sprintf ( Statement, "delete from average where Adate %s '%s'", ActionCode, SelectDate );
	Affected = dbyDelete ( "cleandata", &MySql, Statement, 0, LogFileName );
	printf ( "Deleted %ld average records\n", Affected );

	sprintf ( Statement, "delete from market where MKTdate %s '%s'", ActionCode, SelectDate );
	Affected = dbyDelete ( "cleandata", &MySql, Statement, 0, LogFileName );
	printf ( "Deleted %ld market records\n", Affected );
}
