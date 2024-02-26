/*----------------------------------------------------------------------------
	Program : getdiv/getargs.c
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

#include	"getdiv.h"

static void Usage ()
{
	printf ( "USAGE: getdiv -active        [options]\n" );
	printf ( "USAGE: getdiv -cuts          [options]\n" );
	printf ( "USAGE: getdiv -update        [options]\n" );
	printf ( "USAGE: getdiv -ticker TICKER [options]\n" );
	printf ( "USAGE: getdiv -member ID     [options]\n" );
	printf ( " -active = stocks in portfolios\n" );
	printf ( " -cuts   = check all portfolios for last dividend cuts.  No DB update.\n" );
	printf ( " -update = any stock where dividend count > 0 and Slast current\n" );
	printf ( " -ticker = one stock\n" );
	printf ( " -member = one member\n" );
	printf ( "Options:\n" );
	printf ( " -d1  = debug\n" );
	printf ( " -d2  = debug, more verbose\n" );
	printf ( " -all = get all dividends for each stock\n" );
	exit ( 1 );
}

void getargs ( int argc, char *argv[] )
{
	int		xa, xs;

	if ( argc < 2 )
	{
		Usage ();
	}

	if ( nsStrcmp ( argv[1], "-active" ) == 0 )
	{
		RunMode = MODE_ACTIVE;
		xs = 2;
	}
	else if ( nsStrcmp ( argv[1], "-cuts" ) == 0 )
	{
		RunMode = MODE_CUTS;
		xs = 2;
	}
	else if ( nsStrcmp ( argv[1], "-update" ) == 0 )
	{
		RunMode = MODE_UPDATE;
		xs = 2;
	}
	else if ( argc >= 3 && nsStrcmp ( argv[1], "-ticker" ) == 0 )
	{
		RunMode = MODE_ONE;
		sprintf ( OneTicker, "%s", argv[2] );
		xs = 3;
	}
	else if ( argc >= 3 && nsStrcmp ( argv[1], "-member" ) == 0 )
	{
		RunMode = MODE_MEMBER;
		xmember.xid = nsAtol ( argv[2] );
		xs = 3;
	}
	else
	{
		Usage ();
	}

	Debug = 0;
	IgnoreCutoff = 0;

	for ( xa = xs; xa < argc; xa++ )
	{
		if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
		else if ( nsStrcmp ( argv[xa], "-d1" ) == 0 )
		{
			Debug = 1;
		}
		else if ( nsStrcmp ( argv[xa], "-d2" ) == 0 )
		{
			Debug = 2;
		}
		else if ( nsStrcmp ( argv[xa], "-all" ) == 0 )
		{
			IgnoreCutoff = 1;
		}
		else
		{
			printf ( "Unknown arg\n" );
			exit ( 1 );
		}
	}

	sprintf ( Today, "%s", TodayDate() );
	StrToDatevalFmt ( Today, DATEFMT_YYYY_MM_DD, &dvToday );

	sprintf ( FiveYearsAgo, "%04d-%02d-%02d", 
		dvToday.year4 - 5, dvToday.month, dvToday.day );


}
