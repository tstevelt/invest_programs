/*----------------------------------------------------------------------------
	Program : invest_chk.c
	Author  : Tom Stevelt
	Date    : 05/03/2025
	Synopsis: Check data integrity
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------

----------------------------------------------------------------------------*/
//     Programs called by invest.cgi
// 
//     Copyright (C)  2025 Tom Stevelt
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
#include	"invest_chk.h"

int main ( int argc, char *argv[] )
{
	time_t		stime, etime;
	struct tm	*tm;
	char		tbuffer[30];

	getargs ( argc, argv );

	GetInvestCfg ( 1 );

	time ( &stime );
	tm = localtime ( &stime );
	if ( Debug )
	{
		printf ( "invest_chk: Start %s", asctime(tm) );
	}

	StartMySQL ( &MySql, "invest" );

	switch ( RunMode )
	{
		case MODE_STOCK:
			// sprintf ( WhereClause, "Stype = '%c'",  STYPE_FX );
			LoadStockCB ( &MySql, "", "Sticker", &xstock, (int(*)()) EachStock, 1 );
			break;
	}

	printf ( "Checked %d, Errors %d\n", StockCount, ErrorCount );

	if ( Debug )
	{
		time ( &etime );
		tm = localtime ( &etime );
		asctime_r ( tm, tbuffer );
		TrimRightAndLeft ( tbuffer );
		printf ( "invest_chk: End %s, Total %ld seconds\n", tbuffer, etime - stime );
	}

	return ( 0 );
}
