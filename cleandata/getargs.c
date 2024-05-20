/*----------------------------------------------------------------------------
	Program : cleandata/getargs.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Get command line args
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

static void Usage ()
{
	printf ( "USAGE: cleandata -stock  date [-d]\n" );
	printf ( "       Deletes stocks and history where Slast < date (if not in portfolio)\n" );
	printf ( "USAGE: cleandata -history {equal|less} date [-d]\n" );
	printf ( "       Deletes history and average records\n" );
	printf ( "USAGE: cleandata -report {all | TICKER} date [-d]\n" );
	printf ( "       date = history should be loaded up to and including this date.\n" );
	printf ( "format date as yyyy-mm-dd\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 3 )
	{
		Usage ();
	}

	RunMode = 0;
	for ( xa = 1; xa < argc; xa++ )
	{
		if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-stock" ) == 0 )
		{
			RunMode = MODE_STOCK;
			xa++;
			SelectDate = argv[xa];
		}
		else if ( xa + 2 < argc && nsStrcmp ( argv[xa], "-history" ) == 0 )
		{
			RunMode = MODE_HISTORY;

			xa++;
			if ( nsStrcmp ( argv[xa], "equal" ) == 0 )
			{
				nsStrcpy ( ActionCode, "=" );
			}
			else if ( nsStrcmp ( argv[xa], "less" ) == 0 )
			{
				nsStrcpy ( ActionCode, "<" );
			}
			else
			{
				printf ( "Unknown action\n" );
				exit ( 1 );
			}

			xa++;
			SelectDate = argv[xa];
		}
		else if ( xa + 2 < argc && nsStrcmp ( argv[xa], "-report" ) == 0 )
		{
			xa++;
			if ( nsStrcmp ( argv[xa], "all" ) == 0 )
			{
				RunMode = MODE_REPORT_ALL;
			}
			else
			{
				RunMode = MODE_REPORT_ONE;
				Ticker = argv[xa];
			}

			xa++;
			SelectDate = argv[xa];
		}
		else if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
		else
		{
			printf ( "Unknown/Missing arg\n" );
			exit ( 1 );
		}
	}

	sprintf ( Today, "%s", TodayDate() );
	StrToDatevalFmt ( Today, DATEFMT_YYYY_MM_DD, &dvToday );

}
