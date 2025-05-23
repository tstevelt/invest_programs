/*----------------------------------------------------------------------------
	Program : invest_chk/getargs.c
	Author  : Tom Stevelt
	Date    : 2025
	Synopsis: Get command line args.
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

#include	"invest_chk.h"

static void Usage ()
{
	printf ( "USAGE: invest_chk -stocks [options]\n" );
	printf ( "Options:\n" );
	printf ( " -d      = debug\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;
//	int		Year4, Month, Day;
//	DATEVAL	dvToday, dvYesterday;

	if  ( argc < 2 )
	{
		Usage ();
	}

	Debug = 0;
	RunMode = 0;

	for ( xa = 1; xa < argc; xa++ )
	{
		if ( nsStrcmp ( argv[xa], "-stocks" ) == 0 )
		{
			RunMode = MODE_STOCK;
		}
		else if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
		else
		{
			printf ( "Unknown arg %d %s\n", xa, argv[xa] );
			exit ( 1 );
		}
	}

	if ( RunMode == 0 )
	{
		Usage ();
	}
}
