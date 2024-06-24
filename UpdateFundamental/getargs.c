/*----------------------------------------------------------------------------
	Program : UpdateFundamental/getargs.c
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

#include	"UpdateFundamental.h"


static int Usage ()
{
	printf ( "USAGE: UpdateFundamental -all           [options]\n" );
	printf ( "USAGE: UpdateFundamental -owned         [options]\n" );
	printf ( "USAGE: UpdateFundamental -file filename [options]\n" );
	printf ( "USAGE: UpdateFundamental -ticker ticker [options]\n" );
	printf ( "Options:\n" );
	printf ( " -nodb   = no db update, statements to stdout\n" );
	printf ( " -v      = verbose\n" );
	printf ( " -d [#]  = debug, no db update\n" );

	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 2 )
	{
		Usage ();
	}

	Ignore = 0;
	Debug = 0;
	Verbose = 0;
	StockIndex = '?';
	UpdateDB = 1;
	
	for ( xa = 1; xa < argc; xa++ )
	{
		if ( nsStrcmp ( argv[xa], "-all" ) == 0 )
		{
			StockIndex = 'A';
		}
		else if ( nsStrcmp ( argv[xa], "-owned" ) == 0 )
		{
			StockIndex = 'P';
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-file" ) == 0 )
		{
			xa++;
			StockIndex = 'F';
			InputFileName = argv[xa];
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-ticker" ) == 0 )
		{
			xa++;
			StockIndex = 'T';
			Ticker = argv[xa];
		}
		else if ( nsStrcmp ( argv[xa], "-nodb" ) == 0 )
		{
			UpdateDB = 0;
		}
		else if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			if (( Debug = nsAtoi(argv[xa+1])) == 0 )
			{
				Debug = 1;
			}
			else
			{
				xa++;
			}
			UpdateDB = 0;
		}
		else if ( nsStrcmp ( argv[xa], "-v" ) == 0 )
		{
			Verbose = 1;
		}
		else if ( nsStrcmp ( argv[xa], "-ignore" ) == 0 )
		{
			Ignore = 1;
		}
		else
		{
			Usage ();
		}
	}

	switch ( StockIndex )
	{
		case '?':
			Usage ();
			break;
	}
}
