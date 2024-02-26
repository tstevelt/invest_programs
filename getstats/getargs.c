/*----------------------------------------------------------------------------
	Program : getstats/getargs.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Get command line args.
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

#include	"getstats.h"

static void Usage ()
{
	printf ( "USAGE: getstats -all         [options]\n" );
	printf ( "USAGE: getstats -indexed     [options]\n" );
	printf ( "USAGE: getstats -index INDEX [options]\n" );
	printf ( "USAGE: getstats -one TICKER  [options]\n" );
	printf ( "USAGE: getstats -report      [options]\n" );
	printf ( " -all     = all stocks (about 2000+ at IEX have zero shares).\n" );
	printf ( " -indexed = all stocks in one or more index.\n" );
	printf ( " -index   = stocks in index.\n" );
	printf ( " -one     = one stock.\n" );
	printf ( " -report = print report after data updated.\n" );
	printf ( "Options:\n" );
	printf ( " -d     = debug\n" );
	printf ( " -q     = quiet\n" );
	printf ( " -neg   = skip negative earnings in -report.\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;
	DATEVAL	dvToday, dvYesterday;

	if  ( argc < 2 )
	{
		Usage ();
	}

	Debug = 0;
	Quiet = 0;
	RunMode = 0;
	SkipNegatives = 0;

	for ( xa = 1; xa < argc; xa++ )
	{
		if ( nsStrcmp ( argv[xa], "-indexed" ) == 0 )
		{
			RunMode = MODE_INDEXED;
		}
		else if ( nsStrcmp ( argv[xa], "-all" ) == 0 )
		{
			RunMode = MODE_ALL;
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-index" ) == 0 )
		{
			RunMode = MODE_INDEX;
			xa++;
			StockIndex = argv[xa][0];
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-one" ) == 0 )
		{
			RunMode = MODE_ONE;
			xa++;
			snprintf ( xstock.xsticker, sizeof(xstock.xsticker), "%s", argv[xa] );
		}
		else if ( nsStrcmp ( argv[xa], "-report" ) == 0 )
		{
			RunMode = MODE_REPORT;
			Quiet = 1;
		}
		else if ( nsStrcmp ( argv[xa], "-neg" ) == 0 )
		{
			SkipNegatives = 1;
		}
		else if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
		else if ( nsStrcmp ( argv[xa], "-q" ) == 0 )
		{
			Quiet = 1;
		}
		else
		{
			printf ( "Unknown arg\n" );
			exit ( 1 );
		}
	}

	if ( RunMode == 0 )
	{
		Usage ();
	}

	sprintf ( TempFileName, "%s/getstats_%d.csv", TEMPDIR, getpid() );

	/*----------------------------------------------------------
		set Today
	----------------------------------------------------------*/
	sprintf ( Today, "%s", fmtGetTodayDate ( DATEFMT_YYYY_MM_DD ) );

	/*----------------------------------------------------------
		set Yesterday
	----------------------------------------------------------*/
	dvToday.year4  = nsAtoi (  Today );
	dvToday.month = nsAtoi ( &Today[5] );
	dvToday.day   = nsAtoi ( &Today[8] );

	DateAdd ( &dvToday, -1, &dvYesterday );

	sprintf ( Yesterday, "%04d-%02d-%02d", dvYesterday.year4, dvYesterday.month, dvYesterday.day );

	if ( Debug )
	{
		printf ( "Today set to %s\n", Today );
		printf ( "Yesterday %s\n", Yesterday );
	}

}
