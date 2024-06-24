/*----------------------------------------------------------------------------
	Program : getdata/getargs.c
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

#include	"getdata.h"

static void Usage ()
{
	printf ( "USAGE: getdata -all        [options]\n" );
	printf ( "USAGE: getdata -owned      [options]\n" );
	printf ( "USAGE: getdata -null       [options]\n" );
	printf ( "USAGE: getdata -one TICKER [options]\n" );
	printf ( " -all    = all stocks where stock.Slast in past 30 days.\n" );
	printf ( " -owned  = stocks held in portfolios\n" );
	printf ( " -null   = where Slast is NULL\n" );
	printf ( " -one    = one stock, regardless of stock.Slast.\n" );
	printf ( "Options:\n" );
	printf ( " -fmt x     = csv or json, default is csv\n" );
	printf ( " period     = default is previous day\n" );
	printf ( "              others are -ohlc, cannot use csv format\n" );
	printf ( "              -1mo -2yr -3yr -5yr -10yr\n" );
	printf ( "              -past yyyymmdd\n" );
	printf ( " -flag file = create file if Missing Benchmark, Portfolio or too many SP500 data.\n" );
	printf ( " -subj file = create email subject file.\n" );
	printf ( " -slast     = ignore stock.Slast.  Use this to load old data or reverse file.\n" );
	printf ( " -tiingo    = use Tiingo instead of IEX. Hard coded to Slast. Not with -all.\n" );
	printf ( " -d     = debug\n" );
	printf ( " -q     = quiet\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa, Year4, Month, Day;
	DATEVAL	dvToday, dvYesterday;

	if  ( argc < 2 )
	{
		Usage ();
	}

	Debug = 0;
	Quiet = 0;
	RunMode = 0;
	Period = PERIOD_PREVIOUS;
	CheckSlast = 1;
	FlagFile = NULL;
	SubjectFile = NULL;
	Format = FORMAT_CSV;
	UseTiingo = 0;

	for ( xa = 1; xa < argc; xa++ )
	{
		if ( nsStrcmp ( argv[xa], "-all" ) == 0 )
		{
			RunMode = MODE_ALL;
		}
		else if ( nsStrcmp ( argv[xa], "-owned" ) == 0 )
		{
			RunMode = MODE_OWNED;
		}
		else if ( nsStrcmp ( argv[xa], "-null" ) == 0 )
		{
			RunMode = MODE_NULL;
			CheckSlast = 0;
		}
		else if ( nsStrcmp ( argv[xa], "-slast" ) == 0 )
		{
			CheckSlast = 0;
		}
		else if ( nsStrcmp ( argv[xa], "-ohlc" ) == 0 )
		{
			Period = PERIOD_OHLC;
			Format = FORMAT_JSON;
		}
		else if ( nsStrcmp ( argv[xa], "-1mo" ) == 0 )
		{
			Period = PERIOD_ONE_MONTH;
		}
		else if ( nsStrcmp ( argv[xa], "-2yr" ) == 0 )
		{
			Period = PERIOD_TWO_YEAR;
		}
		else if ( nsStrcmp ( argv[xa], "-3yr" ) == 0 )
		{
			Period = PERIOD_THREE_YEAR;
		}
		else if ( nsStrcmp ( argv[xa], "-5yr" ) == 0 )
		{
			Period = PERIOD_FIVE_YEAR;
		}
		else if ( nsStrcmp ( argv[xa], "-10yr" ) == 0 )
		{
			Period = PERIOD_TEN_YEAR;
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-past" ) == 0 )
		{
			Period = PERIOD_PAST;
			xa++;
			sprintf ( PastDate, "%s", argv[xa] );

			CheckSlast = 0;
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-one" ) == 0 )
		{
			RunMode = MODE_ONE;
			xa++;
			snprintf ( xstock.xsticker, sizeof(xstock.xsticker), "%s", argv[xa] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-fmt" ) == 0 )
		{
			xa++;
			if ( nsStrcmp ( argv[xa], "json" ) == 0 )
			{
				Format = FORMAT_JSON;
			}
			else if ( nsStrcmp ( argv[xa], "csv" ) == 0 )
			{
				Format = FORMAT_CSV;
			}
			else
			{
				Usage ();
			}
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-flag" ) == 0 )
		{
			xa++;
			FlagFile = argv[xa];
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-subj" ) == 0 )
		{
			xa++;
			SubjectFile = argv[xa];
		}
		else if ( nsStrcmp ( argv[xa], "-tiingo" ) == 0 )
		{
			UseTiingo = 1;
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
			printf ( "Unknown arg %d %s\n", xa, argv[xa] );
			exit ( 1 );
		}
	}

	if ( RunMode == 0 || Format == 0 )
	{
		Usage ();
	}

	if ( Period == PERIOD_OHLC && Format == FORMAT_CSV )
	{
		Usage ();
	}

	sprintf ( TempFileName, "%s/getdata_%d.csv", TEMPDIR, getpid() );

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

	/*----------------------------------------------------------
		set Month Ago Date
	----------------------------------------------------------*/
	Year4  = nsAtoi (  Today );
	Month = nsAtoi ( &Today[5] );
	Day   = nsAtoi ( &Today[8] );
	if ( Month == 1 )
	{
		Month = 12;
		Year4--;
	}
	else
	{
		Month--;
		if ( Month == 2 && Day > 28 )
		{
			Month = 3;
			Day = 1;
		}
	}

	sprintf ( MonthAgoDate, "%04d-%02d-%02d", Year4, Month, Day );

	/*----------------------------------------------------------
		set Year Ago Date
	----------------------------------------------------------*/
	Year4  = nsAtoi (  Today );
	Month = nsAtoi ( &Today[5] );
	Day   = nsAtoi ( &Today[8] );
	Year4--;
	if ( Month == 2 && Day == 29 )
	{
		Month = 3;
		Day = 1;
	}
	sprintf ( YearAgoDate, "%04d-%02d-%02d", Year4, Month, Day );

	if ( RunMode == MODE_ALL && UseTiingo == 1 )
	{
		printf ( "Tiingo free account does not support ALL\n" );
		exit ( 1 );
	}

	if ( Debug )
	{
		printf ( "Today set to %s\n", Today );
		printf ( "Yesterday %s\n", Yesterday );
		printf ( "MonthAgoDate %s\n", MonthAgoDate );
		printf ( "YearAgoDate %s\n", YearAgoDate );
	}

}
