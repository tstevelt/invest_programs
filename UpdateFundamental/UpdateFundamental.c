/*----------------------------------------------------------------------------
	Program : UpdateFundamental.c
	Author  : Tom Stevelt
	Date    : 11/08/2023
	Synopsis: Update price sensitive fields. Run in morning after 'getdata'

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		06/10/2024	Added -owned option

----------------------------------------------------------------------------*/
//     Programs called by invest.cgi
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
#include	"UpdateFundamental.h"

int main ( int argc, char *argv[] )
{
	FILE	*fp;
	char	xbuffer[102400];
#define		MAXTOKS		50000
	char	*tokens[MAXTOKS];

// printf ( "sizeof long %ld\n", sizeof(long) );

	time ( &StartTime );

	getargs ( argc, argv );

	StartMySQL ( &MySql, "invest" );

	// dbyCommitInit ( &MySql );

	StockCount = ErrorCount = AgeCount = UpdateCount = InsertCount = 0;

	if ( StockIndex == 'F' )
	{
		if (( fp = fopen ( InputFileName, "r" )) == (FILE *)0 )
		{
			printf ( "Cannot open %s for input\n", InputFileName );
			exit ( 1 );
		}

		while ( fgets ( xbuffer, sizeof(xbuffer), fp ) != (char *)0 )
		{
			if ( GetTokensA ( xbuffer, " \t\r\n", tokens, 2 ) < 1 )
			{
				continue;
			}
			sprintf ( WhereClause, "Sticker = '%s'", tokens[0] );
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 1 );
		}

		nsFclose ( fp );
	}
	else if ( StockIndex == 'T' )
	{
		sprintf ( WhereClause, "Sticker = '%s'", Ticker );
		LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 0 );
	}
	else if ( StockIndex == 'P' )
	{
		sprintf ( WhereClause, 
			"Stype != '%c' and Stype != '%c' and Stype != '%c' and Stype != '%c' and Stype != '%c'",
						STYPE_BOND, STYPE_ETF, STYPE_CRYPTO, STYPE_INDEX, STYPE_OTHER);
		strcat ( WhereClause, " and (select count(*) from  portfolio where Pticker = Sticker) > 0" );
		LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 0 );
	}
	else
	{
		sprintf ( WhereClause, 
			"Stype != '%c' and Stype != '%c' and Stype != '%c' and Stype != '%c' and Stype != '%c'",
						STYPE_BOND, STYPE_ETF, STYPE_CRYPTO, STYPE_INDEX, STYPE_OTHER);
		LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 0 );
	}

	fprintf ( stderr, "StockCount %d, ErrorCount %d, Fix Yield %d, UpdateCount %d, InsertCount %d\n",
			StockCount, ErrorCount, AgeCount, UpdateCount, InsertCount );

	time ( &EndTime );

	// dbyCommitClose ( &MySql );

	fprintf ( stderr, "Total Time %ld\n", EndTime - StartTime );

	return ( 0 );
}
