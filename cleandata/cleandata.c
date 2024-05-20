/*----------------------------------------------------------------------------
	Program : cleandata.c
	Author  : Tom Stevelt
	Date    : 02/07/2019
	Synopsis: Check database for weird data.

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		07/13/2021	Get private api keys with GetInvestCfg().
	tms		11/01/2022	Added fundamentals table.  Skip Stype C and B.
	tms		02/04/2024	Changed report to use fully adjusted data
	tms		05/16/2024	Changed report back to historical data

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

#define		MAIN
#include	"cleandata.h"

int main ( int argc, char *argv[] )
{
	getargs ( argc, argv );

	GetInvestCfg ( 0 );

	StartMySQL ( &MySql, "invest" );

	switch ( RunMode )
	{
		case MODE_STOCK:
			do_stock ();
			break;

		case MODE_HISTORY:
			do_history ();
			break;

		case MODE_REPORT_ALL:
		case MODE_REPORT_ONE:
			do_report ();
			printf ( "Flagged %d stocks\n", FlagCount );
			break;

	}

	return ( 0 );
}
