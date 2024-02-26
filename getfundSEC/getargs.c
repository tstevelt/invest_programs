/*----------------------------------------------------------------------------
	Program : getfundSEC/getargs.c
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

#include	"getfundSEC.h"

static int Usage ()
{
	printf ( "USAGE: getfundSEC index          [options]\n" );
	printf ( "USAGE: getfundSEC -file filename [options]\n" );
	printf ( "USAGE: getfundSEC -ticker ticker [options]\n" );
	printf ( "USAGE: getfundSEC -old [-delete]\n" );
	printf ( " S = S&P 500\n" );
	printf ( " D = Dow Jones Industrials\n" );
	printf ( " N = Nasdaq 100\n" );
	printf ( " M = Midcap (Russell 1000 ex S&P 500)\n" );
	printf ( " 1 = Russell 1000\n" );
	printf ( " 2 = Russell 2000\n" );
	printf ( " 3 = Russell 3000\n" );
	printf ( " O = Other (not in any index)\n" );
	printf ( " A = All stocks\n" );
	printf ( " E = Only ETF\n" );
	printf ( "Options:\n" );
	printf ( " -script file = use file instead of getfundSEC.py\n" );
	printf ( " -ignore      = passes ancient Fframe CY197001\n" );
	printf ( " -v           = verbose\n" );
	printf ( " -nodb        = no db update, statements to stdout\n" );
	printf ( " -d [#]       = debug, no db update\n" );

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
	ReportOld = 0;
	DeleteOld = 0;
	Python_Script = "getfundSEC.py";
	
	for ( xa = 1; xa < argc; xa++ )
	{
		if ( argv[xa][0] != '-' && StockIndex == '?' )
		{
			StockIndex = argv[xa][0];
			if ( StockIndex != 'E' && StockIndex != 'O' )
			{
				ValidStockIndex ( StockIndex, 1, (int(*)()) Usage );
			}
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
			StockIndex = 'x';
			Ticker = argv[xa];
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-script" ) == 0 )
		{
			xa++;
			Python_Script = argv[xa];
		}
		else if ( nsStrcmp ( argv[xa], "-nodb" ) == 0 )
		{
			UpdateDB = 0;
		}
		else if ( nsStrcmp ( argv[xa], "-old" ) == 0 )
		{
			ReportOld = 1;
			StockIndex = 'A';
			if ( xa + 1 < argc && nsStrcmp ( argv[xa+1], "-delete" ) == 0 )
			{
				xa++;
				DeleteOld = 1;
			}
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
