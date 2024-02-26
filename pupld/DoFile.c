/*----------------------------------------------------------------------------
	Program : pupld/DoFile.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Read .csv file and insert records in portfolio table.
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

#include	"pupld.h"

static int DebugDoFile = 0;

int DoFile ( char *tempfn )
{
	FILE	*fp;
	char	xbuffer[1024];
	int		ContentCount;
	int		lineno;
	int		Affected, ImportCount, AcctErrorCount;
#define		MAXTOKS		5
	char	*tokens[MAXTOKS];
	int		tokcnt;
    long	MemberID = 0;
	char	ImportReplace = 'A';
	int		Verbose = 0;
	int		PortfolioCount = 0;

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

			sprintf ( WhereClause, "id = %ld", MemberID );
			LoadMember ( &MySql, WhereClause, &xmember, 0 );
			SetMemberLimits ( xmember.xmrole[0] );

			sprintf ( WhereClause, "Pmember = %ld", MemberID );
			PortfolioCount = dbySelectCount ( &MySql, "portfolio", WhereClause, LogFileName );
			if ( DebugDoFile )
			{
				printf ( "Current portfolio count %d, limit %d\n", PortfolioCount, PortfolioLimit );
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
				snprintf ( Statement, sizeof(Statement), "delete from portfolio where Pmember = %ld", MemberID );

				Affected = dbyDelete ( "pupld", &MySql, Statement, 0, LogFileName );

				if ( Affected > 0 )
				{
					printf ( "Deleted %d stocks from portfolio\n", Affected );
				}
				else
				{
					printf ( "Nothing deleted from portfolio\n" );
				}

				PortfolioCount -= Affected;
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
				printf ( "Verbose %s\n", Verbose ? "yes" : "no" );
			}
		}
		else if ( ContentCount == 6 )
		{
			tokcnt = GetTokensD ( xbuffer, ",|\r\n", tokens, MAXTOKS );

			if ( tokcnt < 4 )
			{
				continue;
			}

			snprintf ( WhereClause, sizeof(WhereClause), "Pmember = %ld and Pticker = '%s'", MemberID, tokens[0] );
			if ( dbySelectCount ( &MySql, "portfolio", WhereClause, LogFileName ) > 0 )
			{
				printf ( "%s is already in portfolio\n",  tokens[0] );
				continue;
			}

			snprintf ( WhereClause, sizeof(WhereClause), "Sticker = '%s'", tokens[0] );
			snprintf ( xportfolio.xpdate, sizeof(xportfolio.xpdate), "%s", tokens[1] );
			xportfolio.xpshares = nsAtof ( tokens[2] );
			xportfolio.xpprice = nsAtof ( tokens[3] );

			if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) != 1 )
			{
				printf ( "Unknown ticker symbol %s.\n", tokens[0] );
				AcctErrorCount++;
				continue;
			}
			else if ( Verbose )
			{
				printf ( "%s\n", xstock.xsname );
			}

			if ( PortfolioCount >= PortfolioLimit )
			{
				printf ( "Portfolio full, contains %d records\n", PortfolioCount );
				break;
			}

			snprintf ( Statement, sizeof(Statement),
"insert into portfolio ( Pmember, Pticker, Pdate, Pshares, Pprice, Pbenchmark, Palerttype )  \
 values (  %ld, '%s', '%s', %.4f, %.2f, 'SPY', '%c' )",
						MemberID, xstock.xsticker, xportfolio.xpdate, xportfolio.xpshares, xportfolio.xpprice, ALERT_BENCHMARK );


			Affected = dbyInsert ( "pupld", &MySql, Statement, 0, LogFileName );

			if ( Affected == 1 )
			{
				ImportCount++;
				PortfolioCount++;
			}
			else
			{
				printf ( "  ERROR: %s NOT IMPORTED\n", xstock.xsticker );
				printf ( "%s\n", Statement );
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
