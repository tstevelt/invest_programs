/*----------------------------------------------------------------------------
	Program : UpdateAverage/getargs.c
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

#include    "UpdateAverage.h"

static int Usage ()
{
	printf ( "USAGE: UpdateAverage -index index   mode [options]\n" );
	printf ( "USAGE: UpdateAverage -ticker TICKER mode [options]\n" );
	SindexUsage ( 0 );
	printf ( "  mode = A for all, Z for zeros\n" );
	printf ( "options:\n" );
	printf ( "  -col xxx   = only one column {price|volume|rsi|stddev|ctb}\n" );
	printf ( "  -p         = pacifier\n" );
	printf ( "  -d1 or -d2 = debug level, no db update\n" );
	exit ( 1 );
}

char StockWhereClause[1024];

void getargs ( int argc, char *argv[] )
{
	int		xa;
	char	StockIndex;

	if ( argc < 4 )
	{
		Usage ();
	}

	Debug = 0;
	Columns = COLUMNS_ALL;
	Pacifier = 0;

	for ( xa = 1; xa < argc; xa++ )
	{
		if ( xa + 2 < argc && nsStrcmp ( argv[xa], "-index" ) == 0 )
		{
			xa++;
			StockIndex = toupper ( argv[xa][0] );
			ValidStockIndex ( StockIndex, 0, (int(*)()) Usage );
			SetWhereClause ( StockWhereClause, StockIndex );
			xa++;
			RunMode = toupper ( argv[xa][0] );
		}
		else if ( xa + 2 < argc && nsStrcmp ( argv[xa], "-ticker" ) == 0 )
		{
			xa++;
			sprintf ( StockWhereClause, "Sticker = '%s'", argv[xa] );
			xa++;
			RunMode = toupper ( argv[xa][0] );
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-col" ) == 0 )
		{
			xa++;
			if ( nsStrcmp ( argv[xa], "price" ) == 0 )
			{
				Columns = COLUMNS_PRICE;
			}
			else if ( nsStrcmp ( argv[xa], "volume" ) == 0 )
			{
				Columns = COLUMNS_VOLUME;
			}
			else if ( nsStrcmp ( argv[xa], "rsi" ) == 0 )
			{
				Columns = COLUMNS_RSI;
			}
			else if ( nsStrcmp ( argv[xa], "stddev" ) == 0 )
			{
				Columns = COLUMNS_STDDEV;
			}
			else if ( nsStrcmp ( argv[xa], "ctb" ) == 0 )
			{
				Columns = COLUMNS_CTB;
			}
			else
			{
				printf ( "Unknown -one %s\n", argv[xa] );
				exit ( 1 );
			}
		}
		else if ( nsStrcmp ( argv[xa], "-p"  ) == 0 )
		{
			Pacifier = 1;
		}
		else if (( nsStrcmp ( argv[xa], "-d"  ) == 0 ) ||
				 ( nsStrcmp ( argv[xa], "-d1" ) == 0 ))
		{
			Debug = 1;
		}
		else if ( nsStrcmp ( argv[xa], "-d2" ) == 0 )
		{
			Debug = 2;
			printf ( "sizeof HistoryArray %ld\n", sizeof(HistoryArray) );
		}
		else
		{
			Usage ();
		}
	}

	switch ( RunMode )
	{
		case MODE_ALL:
		case MODE_ZERO:
			break;
		default:
			Usage ();
			break;
	}
}
