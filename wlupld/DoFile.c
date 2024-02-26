/*----------------------------------------------------------------------------
	Program : wlupld/DoFile.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Read .csv file and insert records in the watchlist table.
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

#include	"wlupld.h"

static int DebugDoFile = 0;

int DoFile ( char *tempfn )
{
	FILE	*fp;
	char	xbuffer[1024];
	int		ContentCount;
	int		lineno;
	int		Affected, ImportCount, AcctErrorCount;
#define		MAXTOKS		3
	char	*tokens[MAXTOKS];
	int		tokcnt;
    long	MemberID = 0;
	char	ImportReplace = 'A';
	int		Verbose = 0;
	int		WatchlistCount = 0;

	printf ( "<pre>\n" );

	/*--------------------------------------------------------------
		open the file
	--------------------------------------------------------------*/
	if (( fp = fopen ( tempfn, "r" )) == (FILE *)0 )
	{
		printf ( "Can not open file %s, errno %d<br>\n", tempfn, errno );
		return ( -1 );
	}

	/*--------------------------------------------------------------
		read first part of file to find account number and 
		parse function.  break and go in to second while loop
		to read the actual data.
	--------------------------------------------------------------*/
	lineno = 0;
	ContentCount = 0;
	ImportCount = AcctErrorCount = 0;
	lineno = 0;

	while ( fgets ( xbuffer, sizeof(xbuffer), fp ) != (char *)0 )
	{
		lineno++;

		TrimRight ( xbuffer );

		if ( nsStrlen ( xbuffer ) == 0 )
		{
			continue;
		}

		if ( nsStrncmp ( xbuffer, "Content-", 8 ) == 0 )
		{
			ContentCount++;
			continue;
		}

		if ( nsStrncmp ( xbuffer, "--------", 8 ) == 0 )
		{
			if ( ContentCount == 6 )
			{
				break;
			}
			else
			{
				continue;
			}
		}

		if ( ContentCount == 2 )
		{
			MemberID = nsAtol ( xbuffer );
			if ( DebugDoFile )
			{
				printf ( "MemberID %s\n", xbuffer );
			}

			sprintf ( Statement, "id = %ld", MemberID );
			LoadMember ( &MySql, Statement, &xmember, 0 );
			SetMemberLimits ( xmember.xmrole[0] );

			sprintf ( Statement, "Wmember = %ld", MemberID );
			WatchlistCount = dbySelectCount ( &MySql, "watchlist", Statement, LogFileName );
			if ( DebugDoFile )
			{
				printf ( "Current watchlist count %d, limit %d\n", WatchlistCount, WatchlistLimit );
			}
		}
		else if ( ContentCount == 3 )
		{
			ImportReplace = xbuffer[0];
			if ( DebugDoFile )
			{
				printf ( "ImportReplace %s\n", xbuffer );
			}

			if ( ImportReplace == 'R' )
			{
				snprintf ( Statement, sizeof(Statement), "delete from watchlist where Wmember = %ld", MemberID );

				Affected = dbyDelete ( "wlupld", &MySql, Statement, 0, LogFileName );

				if ( Affected > 0 )
				{
					printf ( "Deleted %d stocks from watchlist\n", Affected );
				}
				else
				{
					printf ( "Nothing deleted from watchlist\n" );
				}

				WatchlistCount -= Affected;
			}

		}
		else if ( ContentCount == 4 )
		{
			if ( xbuffer[0] == 'Y' )
			{
				Verbose = 1;
			}
			else
			{
				Verbose = 0;
			}

			if ( DebugDoFile )
			{
				printf ( "Verbose %s\n", Verbose ? "yse" : "no" );
			}
		}
		else if ( ContentCount == 6 )
		{
			tokcnt = GetTokensD ( xbuffer, ",|\t\n\r", tokens, MAXTOKS );

			if ( tokcnt < 1 )
			{
				continue;
			}

			sprintf ( Statement, "Wmember = %ld and Wticker = '%s'", MemberID, tokens[0] );
			if ( dbySelectCount ( &MySql, "watchlist", Statement, LogFileName ) > 0 )
			{
				printf ( "%s is already in watch list\n", tokens[0] );
				continue;
			}

			snprintf ( Statement, sizeof(Statement), "Sticker = '%s'", tokens[0] );

			if ( LoadStock ( &MySql, Statement, &xstock, 0, 0 ) != 1 )
			{
				printf ( "Unknown ticker symbol %s.\n", tokens[0] );
				AcctErrorCount++;
				continue;
			}
			else if ( Verbose )
			{
				printf ( "%s\n", xstock.xsname );
			}

			if ( WatchlistCount >= WatchlistLimit )
			{
				printf ( "Watchlist full, contains %d records\n", WatchlistCount );
				break;
			}

			snprintf ( Statement, sizeof(Statement),
					"insert into watchlist ( Wticker, Wmember, Walerttype )  values ( '%s', %ld, '%c' )",
							xstock.xsticker, MemberID, ALERT_NONE );

			Affected = dbyInsert ( "wlupld", &MySql, Statement, 0, LogFileName );

			if ( Affected == 1 )
			{
				ImportCount++;
				WatchlistCount++;
			}
			else
			{
				printf ( "  ERROR: %s NOT IMPORTED\n", xstock.xsticker );
				AcctErrorCount++;
			}
		}
	}

	if ( ContentCount != 6 )
	{
		printf ( "File does not contain 5 Content- statements.\n" );
	}

	printf ( "imported %d stocks, %d errors", ImportCount, AcctErrorCount );

	printf ( "</pre>\n" );

	return ( 0 );
}
