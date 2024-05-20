/*----------------------------------------------------------------------------
	Program : getsplits/getargs.c
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

#include	"getsplits.h"

static void Usage ()
{
	printf ( "USAGE: getsplits -all           [options]\n" );
	printf ( "USAGE: getsplits -active        [options]\n" );
	printf ( "USAGE: getsplits -member ID     [options]\n" );
	printf ( "USAGE: getsplits -ticker TICKER [options]\n" );
	printf ( " -all    = all stocks\n" );
	printf ( " -active = stocks in all portfolios\n" );
	printf ( " -member = stocks in one member portfolio\n" );
	printf ( " -ticker = one stock\n" );
	printf ( "Options:\n" );
	printf ( "-range x  = range to retrieve. default 2w\n" );
	printf ( "-d1       = debug\n" );
	printf ( "-d2       = debug, more verbose\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 2 )
	{
		Usage ();
	}

	Range = "2w";
	Days = 14;
	Debug = 0;

	for ( xa = 1; xa < argc; xa++ )
	{
		if ( nsStrcmp ( argv[xa], "-active" ) == 0 )
		{
			RunMode = MODE_ACTIVE;
		}
		else if ( nsStrcmp ( argv[xa], "-all" ) == 0 )
		{
			RunMode = MODE_ALL;
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-member" ) == 0 )
		{
			xa++;
			xportfolio.xpmember = nsAtol ( argv[xa] );
			RunMode = MODE_MEMBER;
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-ticker" ) == 0 )
		{
			xa++;
			sprintf ( OneTicker, "%s", argv[xa] );
			RunMode = MODE_ONE;
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-range" ) == 0 )
		{
			xa++;
			Range = argv[xa];

			/*----------------------------------------------------------
				Days are multiples of week, because EachStock()
				compensates for running on Saturday or Sunday.
			----------------------------------------------------------*/
			if ( strcmp ( "1w", Range ) == 0 )
			{
				Days = 7;
			}
			else if ( strcmp ( "2w", Range ) == 0 )
			{
				Days = 14;
			}
			else if ( strcmp ( "3w", Range ) == 0 )
			{
				Days = 21;
			}
			else if ( strcmp ( "1m", Range ) == 0 )
			{
				Days = 7*4;
			}
			else if ( strcmp ( "3m", Range ) == 0 )
			{
				Days = 7*4*3;
			}
			else if ( strcmp ( "6m", Range ) == 0 )
			{
				Days = 7*4*6;
			}
			else if ( strcmp ( "1y", Range ) == 0 )
			{
				Days = 7*52;
			}
			else if ( strcmp ( "2y", Range ) == 0 )
			{
				Days = 7*52*2;
			}
			else if ( strcmp ( "5y", Range ) == 0 )
			{
				Days = 7*52*5;
			}
			else
			{
				printf ( "Valid raanges are 1w 2w 3w 1m 3m 6m 1y 2y 5y\n" );
				exit ( 1 );
			}
		}
		else if ( nsStrcmp ( argv[xa], "-d1" ) == 0 )
		{
			Debug = 1;
		}
		else if ( nsStrcmp ( argv[xa], "-d2" ) == 0 )
		{
			Debug = 2;
		}

		else
		{
			Usage ();
		}
	}
}
