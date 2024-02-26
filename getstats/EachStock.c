/*----------------------------------------------------------
	IEX Cloud
----------------------------------------------------------*/
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
#include	"getstats.h"

#undef STUFF

#ifdef STUFF
static	char	cmdline[2048];
static	char	buffer[1024];
#define		MAXTOKS		35
static	char	*tokens[MAXTOKS];
static	int		tokcnt;
#endif


#ifdef STUFF
static int BreakOut ()
{
	return ( -1 );
}
#endif

int EachStock ()
{
#ifdef STUFF
	int		lineno, Expected, xt, xf, rv;
	FILE	*fp;
	time_t	stime, etime;
	char	*Tags[MAXTOKS];
	XSTOCK	NewStock;
	double	NewFloat, NewOutstanding;

	StockCount++;

	if ( Debug )
	{
		printf ( "%s %s %s %c %c %c %c\n", 
				xstock.xsticker, xstock.xslast, xstock.xsexchange,
				xstock.xsdj[0],
				xstock.xssp500[0],
				xstock.xsnasdaq[0],
				xstock.xsrussell[0] );
// return ( 0 );
	}

//	Expected = 30;
	Expected = 4;

	sprintf ( cmdline, 
		"curl -s '%s/stock/%s/stats?token=%s' > %s",
				env_ApiURL, xstock.xsticker, env_ApiKey, TempFileName );

	time ( &stime );

	if ( Debug )
	{
		printf ( "%s\n", cmdline );
	}

	system ( cmdline );
	time ( &etime );
	SourceTime = SourceTime + ( etime - stime );

	if (( fp = fopen ( TempFileName, "r" )) == (FILE *)0 )
	{
		return ( -1 );
	}

	time ( &stime );

	lineno = 0;
	NewFloat = NewOutstanding = 0;
	while ( fgets ( buffer, sizeof(buffer), fp ) != (char *)0 )
	{
		lineno++;

		if ( nsStrncmp ( buffer, "Unknown symbol", 14 ) == 0 )
		{
			// printf ( "Unknown ticker %s\n", xstock.xsticker );
			break;
		}

		if (( tokcnt = JsonTokens  ( buffer, Tags, tokens, MAXTOKS )) < Expected )
		{
//			if ( Debug )
			{
				printf ( "%s tokcnt %d on line %d\n", xstock.xsticker, tokcnt, lineno );
			}
			continue;
		}

		for ( xt = 0, xf = 0; xt < tokcnt && xf < 4; xt++ )
		{
			if ( nsStrcmp ( Tags[xt], "sharesOutstanding" ) == 0 )
			{
				NewOutstanding = nsAtof ( tokens[xt] );
				xf++;
			}
			else if ( nsStrcmp ( Tags[xt], "float" ) == 0 )
			{
				NewFloat = nsAtof ( tokens[xt] );
				xf++;
			}
			else if ( nsStrcmp ( Tags[xt], "ttmEPS" ) == 0 )
			{
				NewStock.xsttmeps = nsAtof ( tokens[xt] );
				xf++;
			}
			else if ( nsStrcmp ( Tags[xt], "dividendYield" ) == 0 )
			{
				NewStock.xsyield = 100.0 * nsAtof ( tokens[xt] );
				xf++;
			}
#ifdef OTHER_FIELDS
			else if ( nsStrcmp ( Tags[xt], "week52change" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "week52high" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "week52low" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "marketcap" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "employees" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "day200MovingAvg" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "day50MovingAvg" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "avg10Volume" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "avg30Volume" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "companyName" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "maxChangePercent" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "year5ChangePercent" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "year2ChangePercent" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "year1ChangePercent" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "ytdChangePercent" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "month6ChangePercent" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "month3ChangePercent" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "month1ChangePercent" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "day30ChangePercent" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "day5ChangePercent" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "nextDividendDate" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "ttmDividendRate" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "nextEarningsDate" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "exDividendDate" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "peRatio" ) == 0 )
			else if ( nsStrcmp ( Tags[xt], "beta" ) == 0 )
#endif
		}

		if ( NewOutstanding > 1.0 )
		{
			NewStock.xsshares = NewOutstanding;
		}
		else if ( NewFloat > 1.0 )
		{
			NewStock.xsshares = NewFloat;
		}
		else
		{
			printf ( "%s no shares outstanding or float\n", xstock.xsticker );
			break;
		}

		if ( Debug )
		{
			printf ( "%s %.0f %.4f %.4f\n", xstock.xsticker, NewStock.xsshares, NewStock.xsttmeps, NewStock.xsyield );
		}

		if (( fabs ( NewStock.xsshares - xstock.xsshares ) > 10   ) ||
			( fabs ( NewStock.xsttmeps - xstock.xsttmeps ) > 0.01 ) ||
			( fabs ( NewStock.xsyield  - xstock.xsyield  ) > 0.01 ))
		{
			sprintf ( Statement, 
				"update stock set Sshares = '%.0f', Sttmeps = %.4f, Syield = %.4f where Sticker = '%s'", 
							NewStock.xsshares, NewStock.xsttmeps, NewStock.xsyield, xstock.xsticker );

			rv = dbyUpdate ( "getstats", &MySql, Statement, 0, LogFileName );

			if ( rv == 1 )
			{
				UpdateCount++;
			}
			else
			{
				printf ( "Update Failed: rv %d, %s\n", rv, Statement );
			}
		}
		else if ( Debug )
		{
			printf ( "%s NOT UPDATING\n", xstock.xsticker );
			printf ( " STOCK  %.0f %.4f %.4f\n", NewStock.xsshares, NewStock.xsttmeps, NewStock.xsyield );
			printf ( " NEW    %.0f %.4f %.4f\n", xstock.xsshares, xstock.xsttmeps, xstock.xsyield );
		}

		break;
	}

	time ( &etime );
	DestinationTime = DestinationTime + ( etime - stime );

	nsFclose ( fp );

	fflush ( stdout );
#endif
	return ( 0 );
}
