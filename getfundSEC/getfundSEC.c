/*---------------------------------------------------------------------------
	Program : getfundSEC.c
	Author  : Tom Stevelt
	Date    : 11/06/2023
	Synopsis: Get fundamentals from SEC Edgar and store in DB
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		11/09/2023	Update Fvalue for any modified after StartTime
	tms		11/15/2023	Improve the OLD report.
	tms		11/18/2023	No longer need to update Fvalue. But Add FixStupid.
	tms		11/28/2023	Remove FixStupid, get good data with getfundIEX
	tms		11/28/2023	No longer need getfundETF.py

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
#include	"getfundSEC.h"

int TotalOld = 0;

int main ( int argc, char *argv[] )
{
	FILE	*fp;
	char	xbuffer[102400];
#define		MAXTOKS		50000
	char	*tokens[MAXTOKS];
	int		rv;

// printf ( "sizeof long %ld\n", sizeof(long) );

	time ( &StartTime );

	getargs ( argc, argv );

	StartMySQL ( &MySql, "invest" );

	if ( ReportOld )
	{
		sprintf ( WhereClause, "Fupdated < '%ld'", StartTime - SEVEN_DAYS );
		TotalOld = dbySelectCount ( &MySql, "fundamental", WhereClause, LogFileName );
		rv = LoadFundamentalCB ( &MySql, WhereClause, "Fticker", &xfundamental, (int(*)()) EachFundamental, 0 );
		if ( rv == 0 )
		{
			printf ( "No old fundamentals!\n" );
		}
		exit ( 0 );
	}

	sprintf ( SQL_Script, "/var/local/tmp/getfund_%d.sql", getpid() );
	unlink ( SQL_Script );

	dbyCommitInit ( &MySql );

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
	else if ( StockIndex == 'E' )
	{
		sprintf ( WhereClause, "Scik like '0%%' and Stype = 'E'" );
		LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 0 );
	}
	else if ( StockIndex == 'x' )
	{
		sprintf ( WhereClause, "Sticker = '%s'", Ticker );
		LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 0 );
	}
	else
	{
		SetWhereClause ( WhereClause, StockIndex );
		strcat ( WhereClause, " and Scik like '0%'" );
		LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 0 );
	}

	time ( &EndTime );

	dbyCommitClose ( &MySql );

	if ( StockCount )
	{
		if ( UpdateDB )
		{
			if ( Verbose )
			{
				printf ( "Running update script %s\n", SQL_Script );
				fflush ( stdout );
			}

			sprintf ( cmdline, "mysql -D invest < %s", SQL_Script );
			system ( cmdline );

			StartTime--;
			sprintf ( WhereClause, "Fupdated >= %ld", StartTime );
			UpdateCount = dbySelectCount ( &MySql, "fundamental", WhereClause, LogFileName );

			if ( Verbose )
			{
				printf ( "Not removiing %s\n", SQL_Script );
			}
			else
			{
				unlink ( SQL_Script );
			}
		}
		else
		{
			sprintf ( cmdline, "cat %s", SQL_Script );
			system ( cmdline );
		}
	}

	printf ( "getfundSEC: StockCount %d, UpdateCount %d, ErrorCount %d\n", StockCount, UpdateCount, ErrorCount );
	printf ( "getfundSEC: Total Time %ld, Nap Time %.2f\n", EndTime - StartTime, (double) NapTime / 1000.0 );

	return ( 0 );
}
