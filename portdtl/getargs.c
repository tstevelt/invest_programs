/*----------------------------------------------------------------------------
	Program : portdtl/getargs.c
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

#include	"portdtl.h"

static void Usage ()
{
	printf ( "USAGE: portdtl -port   ID\n" );
	printf ( "       portdtl -member memberID Ticker\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 4 )
	{
		Usage ();
	}

	RunMode = 0;
	Format = RPT_FORMAT_HTML;

	for ( xa = 1; xa < argc; xa++ )
	{
		if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-port" ) == 0 )
		{
			xa++;
			xportfolio.xid = nsAtol ( argv[xa] );
			RunMode = TABLE_portfolio;
		}
		else if ( xa + 2 < argc && nsStrcmp ( argv[xa], "-member" ) == 0 )
		{
			xa++;
			xmember.xid = nsAtol ( argv[xa] );

			xa++;
			sprintf ( xstock.xsticker, "%s", argv[xa] );

			RunMode = TABLE_member;
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-fmt" ) == 0 )
		{
			xa++;

			/* ignore */
#ifdef STUFF
			if ( nsStrcmp ( argv[xa], "txt" ) == 0 )
			{
				Format = RPT_FORMAT_TEXT;
			}
			else if ( nsStrcmp ( argv[xa], "html" ) == 0 )
			{
				Format = RPT_FORMAT_HTML;
			}
			else
			{
				Usage ();
			}
#endif
		}
		else
		{
			Usage ();
		}
	}
	if ( RunMode == 0 )
	{
		Usage ();
	}
}
