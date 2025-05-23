/*----------------------------------------------------------------------------
	Program : getfx/EachStock.c
	Author  : Tom Stevelt
	Date    : 2025
	Synopsis: For each stock, download data from IEX (or Tiingo) and save.
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

#include	"getfx.h"

static int BreakOut ()
{
	return ( -1 );
}

int GetIndex ( char *Field )
{
	for ( int ndx = 0; ndx < tokcnt; ndx++ )
	{
		if ( strcmp ( Field, tokens[ndx] ) == 0 )
		{
			return ( ndx );
		}
	}
	return ( -1 );
}

int EachStock ()
{
	int		rv, lineno, Expected;
	int		DateNdx, OpenNdx, HighNdx, LowNdx, CloseNdx;
	FILE	*tfp;
	time_t	stime, etime;
	int		HistoryThisStock;
	char	MaxHistDate[12];
	double	LastClose;
	char	HighDate[12];
	double	NewHigh;
	DATEVAL			dvDate;

	if ( xstock.xstype[0] != STYPE_FX )
	{
		return ( 0 );
	}

	StockCount++;

	if ( Debug )
	{
		printf ( "EachStock: %s %s\n", xstock.xsticker, xstock.xslast );
	}

	HistoryThisStock = 0;
	LastClose = 0.0;
	memset ( MaxHistDate, '\0', sizeof(MaxHistDate) );
	NewHigh = xstock.xshigh52;
	memset ( HighDate, '\0', sizeof(HighDate) );
	HistoryThisStock = 0;

	Expected = 6;

	CurrentDateval ( &dvDate );

	switch ( Period )
	{
		case PERIOD_ONE_MONTH:
			if ( dvDate.month > 1 )
			{
				sprintf ( xstock.xslast, "%04d-%02d-%02d", dvDate.year4, dvDate.month - 1, 1 /*dvDate.day*/ );
			}
			else
			{
				sprintf ( xstock.xslast, "%04d-%02d-%02d", dvDate.year4 - 1, 12, 1 );
			}
			break;

		case PERIOD_TWO_YEAR:
			sprintf ( xstock.xslast, "%04d-%02d-%02d", dvDate.year4 - 2, dvDate.month, dvDate.day );
			break;

		case PERIOD_THREE_YEAR:
			sprintf ( xstock.xslast, "%04d-%02d-%02d", dvDate.year4 - 3, dvDate.month, dvDate.day );
			break;

		case PERIOD_FIVE_YEAR:
			sprintf ( xstock.xslast, "%04d-%02d-%02d", dvDate.year4 - 5, dvDate.month, dvDate.day );
			break;

		case PERIOD_TEN_YEAR:
			sprintf ( xstock.xslast, "%04d-%02d-%02d", dvDate.year4 - 10, dvDate.month, dvDate.day );
			break;

		default:
			sprintf ( xstock.xslast, "%s", Yesterday );
			break;
	}

/*---------------------------------------------------------------------------
# Alternative
TIINGO_APISTR       2b11ee15eefa81de13f020d2cbfc654e0f19092b
TIINGO_URL          https://api.tiingo.com/tiingo
	'-forex' )
		if [ "$DATE" ]
		then
			# curl -s "https://api.tiingo.com/tiingo/fx/$TICKER/prices?startDate=$DATE&resampleFreq=24hour&token=$APISTR" | sed 's/,/\n/g'
			curl -s "https://api.tiingo.com/tiingo/fx/$TICKER/prices?startDate=$DATE&resampleFreq=24hour&token=$APISTR&format=csv"
		else
			curl -s "https://api.tiingo.com/tiingo/fx/$TICKER/top?token=$APISTR"
		fi
---------------------------------------------------------------------------*/
#ifdef HAVE_LIBCURL
	sprintf ( cmdline, 
		"%s/fx/%s/prices?startDate=%s&resampleFreq=24hour&format=csv&columns=date,open,high,low,close&token=%s",
			env_ApiURL, xstock.xsticker, 
			xstock.xslast,
			env_ApiKey );
#else
	sprintf ( cmdline, 
		"curl -s '%s/fx/%s/prices?startDate=%s&resampleFreq=24hour&format=csv&columns=date,open,high,low,close&token=%s' > %s",
			env_ApiURL, xstock.xsticker, 
			xstock.xslast,
			env_ApiKey, TempFileName );
#endif

	time ( &stime );

	if ( Debug )
	{
		printf ( "%s\n", cmdline );
	}

#ifdef HAVE_LIBCURL
	if (( tfp = fopen ( TempFileName, "w" )) == (FILE *)0 )
	{
		return ( -1 );
	}

	curl_easy_setopt ( curl, CURLOPT_URL, cmdline );
	curl_easy_setopt ( curl, CURLOPT_WRITEDATA, tfp );

	if (( curlRV = curl_easy_perform ( curl )) != CURLE_OK )
	{
		fprintf ( stderr, "perform failed: %s\n", curl_easy_strerror(curlRV) );
		exit ( 1 );
	}

	nsFclose ( tfp );
#else
	system ( cmdline );
#endif

	time ( &etime );
	SourceTime = SourceTime + ( etime - stime );

	if (( tfp = fopen ( TempFileName, "r" )) == (FILE *)0 )
	{
		if ( Debug )
		{
			printf ( "fopen failed\n" );
		}
		return ( -1 );
	}

	time ( &stime );

	lineno = 0;
	while ( fgets ( buffer, sizeof(buffer), tfp ) != (char *)0 )
	{
		lineno++;

		if ( nsStrncmp ( buffer, "Unknown symbol", 14 ) == 0 )
		{
			if ( Debug )
			{
				printf ( "Unknown ticker %s\n", xstock.xsticker );
			}
			break;
		}

		TrimRight ( buffer );

			tokcnt = GetTokensA ( buffer, ",", tokens, MAXTOKS );

			if ( lineno == 1 )
			{
				if (( DateNdx = GetIndex ( "date" )) == -1 )
				{
					printf ( "missing date\n" );
					return ( 0 );
				}
				if (( OpenNdx = GetIndex ( "open" )) == -1 )
				{
					printf ( "missing open\n" );
					return ( 0 );
				}
				if (( HighNdx = GetIndex ( "high" )) == -1 )
				{
					printf ( "missing high\n" );
					return ( 0 );
				}
				if (( LowNdx = GetIndex ( "low" )) == -1 )
				{
					printf ( "missing low\n" );
					return ( 0 );
				}
				if (( CloseNdx = GetIndex ( "close" )) == -1 )
				{
					printf ( "missing close\n" );
					return ( 0 );
				}

				continue;
			}

			sprintf ( xhistory.xhdate, "%10.10s", tokens[DateNdx] );
			xhistory.xhopen  = nsAtof(tokens[OpenNdx]);
			xhistory.xhclose = nsAtof(tokens[CloseNdx]);
			xhistory.xhhigh  = nsAtof(tokens[HighNdx]);
			xhistory.xhlow   = nsAtof(tokens[LowNdx]);

			switch ( xstock.xstype2[0] )
			{
				case STYPE2_HUNDREDTH:
					xhistory.xhopen  *= 100.0;
					xhistory.xhclose *= 100.0;
					xhistory.xhhigh  *= 100.0;
					xhistory.xhlow   *= 100.0;
					break;
				case STYPE2_BASIS:
					xhistory.xhopen  *= 10000.0;
					xhistory.xhclose *= 10000.0;
					xhistory.xhhigh  *= 10000.0;
					xhistory.xhlow   *= 10000.0;
					break;
			}

		if ( CheckSlast == 1 && nsStrcmp ( xhistory.xhdate, xstock.xslast ) <= 0 )
		{
			if ( Debug )
			{
				printf ( "Skipping date %s\n", xhistory.xhdate );
			}
			continue;
		}

		sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'", xstock.xsticker, xhistory.xhdate );
		if (  LoadHistoryCB ( &MySql, WhereClause, NULL, &xhistory, BreakOut, 0 ) >= 1 )
		{
			if ( Debug )
			{
				printf ( "%s %s already loaded in history\n", xstock.xsticker,  xhistory.xhdate );
			}
			continue;
		}

		if ( xhistory.xhclose < 0.01 )
		{
			if ( Debug )
			{
				printf ( "Date %s missing close\n", xhistory.xhdate );
			}
			continue;
		}
		if ( xhistory.xhopen < 0.01 )
		{
			xhistory.xhopen = xhistory.xhclose;
		}
		if ( xhistory.xhhigh < 0.01 )
		{
			xhistory.xhhigh = xhistory.xhclose;
		}
		if ( xhistory.xhlow  < 0.01 )
		{
			xhistory.xhlow  = xhistory.xhclose;
		}

		sprintf ( Statement,
			"insert into history (%s) values ( '%s', '%s', %f, %f, %f, %f )",
				INSERT_FIELDS,
				xstock.xsticker,
				xhistory.xhdate,
				xhistory.xhopen,
				xhistory.xhhigh,
				xhistory.xhlow,
				xhistory.xhclose );

		rv = dbyInsert ( "EachStock", &MySql, Statement, 0, LogFileName );

		if ( Debug )
		{
			printf ( "rv %d: %s\n", rv, Statement );
		}

		if ( rv == 0 )
		{
			// if ( RunMode == MODE_ONE )
			{
				printf ( "insert failed on stock %s, date %s, line %d\n",
					xstock.xsticker, xhistory.xhdate, lineno );
				printf ( "    %s\n", Statement );
			}
			InsertFailedErrorCount++;
		}
		else
		{
			if ( MaxHistDate[0] == '\0' || nsStrcmp ( MaxHistDate, xhistory.xhdate ) < 0 )
			{
				LastClose = xhistory.xhclose;
				nsStrcpy ( MaxHistDate, xhistory.xhdate );
			}

			if ( xhistory.xhhigh > NewHigh )
			{
				NewHigh = xhistory.xhhigh;
				nsStrcpy ( HighDate, xhistory.xhdate );
			}

			HistoryCount++;
			HistoryThisStock++;
			if ( Debug || RunMode == MODE_ONE )
			{
				if ( Quiet == 0 )
				{
					if ( HistoryThisStock == 1 )
					{
						printf ( "Got history for %s from %s.\n", xstock.xsticker, UseTiingo ? "Tiingo" : "IEX Cloud" );
					}
				}
			}
		}
	}

	time ( &etime );
	DestinationTime = DestinationTime + ( etime - stime );

	nsFclose ( tfp );

	if ( Debug )
	{
		printf ( "lineno %d\n", lineno );
	}


	if ( HistoryThisStock > 0 )
	{
		int		rv;

		if (( rv = nsStrcmp ( xstock.xslast, MaxHistDate )) < 0  )
		{
			sprintf ( Statement, 
				"update stock set Slast = '%s', Sclose = %f where Sticker = '%s'", 
						MaxHistDate, LastClose, xstock.xsticker );

			SlastCount += dbyUpdate ( "getfx", &MySql, Statement, 0, LogFileName );
		}

		if ( NewHigh > xstock.xshigh52 )
		{
			xstock.xshigh52 = NewHigh;
			nsStrcpy ( xstock.xsdate52, HighDate );

			sprintf ( Statement, 
				"update stock set Shigh52 = %f, Sdate52 = '%s' where Sticker = '%s'", 
					NewHigh, HighDate, xstock.xsticker );

			if ( dbyUpdate ( "getfx", &MySql, Statement, 0, LogFileName ) != 1 )
			{
				printf ( "Update Shigh52 and Sdate52 failed on %s\n", xstock.xsticker );
			}
			else if ( Debug )
			{
				printf ( "%s new 52 week high %.4f\n", xstock.xsticker, NewHigh );
			}
		}

	}

	if ( nsStrcmp ( xstock.xsdate52, YearAgoDate ) < 0 )
	{
		sprintf ( Statement, 
			"update stock set Shigh52 = (select max(Hhigh) from history \
			  where history.Hticker = '%s' and history.Hdate > date_sub(stock.Slast, interval 52 week)) where Sticker = '%s'", 
					xstock.xsticker, xstock.xsticker );

		if (( rv = dbyUpdate ( "getfx", &MySql, Statement, 0, LogFileName )) != 1 )
		{
			printf ( "Update Shigh52 failed on %s, Sdate52 %s Shigh52 %.2f, rv %d\n", 
					xstock.xsticker, xstock.xsdate52, xstock.xshigh52, rv );
		}

		sprintf ( Statement, 
			"update stock set Sdate52 = (select max(Hdate) from history \
			  where history.Hticker = '%s' and history.Hhigh = stock.Shigh52) where Sticker = '%s'", 
			  		xstock.xsticker, xstock.xsticker ) ;

		if (( rv = dbyUpdate ( "getfx", &MySql, Statement, 0, LogFileName )) != 1 )
		{
			printf ( "Update Sdate52 failed on %s, Sdate52 %s Shigh52 %.2f, rv %d\n", 
					xstock.xsticker, xstock.xsdate52, xstock.xshigh52, rv );
		}
	}

	if ( HistoryThisStock == 0 )
	{
		int		Printed = 0;

		if ( xstock.xssp500[0] == 'Y' )
		{
			MissingMajorDataErrorCount++;
			if ( MissingMajorDataErrorCount < 10 )
			{
				printf ( "Could not find data for SP500 stock %s.\n", xstock.xsticker );
				Printed = 1;
			}
			else if ( MissingMajorDataErrorCount == 10 )
			{
				printf ( "Not reportiing any more missing SP500 stocks.\n" );
			}
		}

		if ( IsBenchmark ( xstock.xsticker ) == 1 )
		{
			if ( Printed == 0 ) printf ( "Could not find data for %s BENCHMARK.\n", xstock.xsticker );
			Printed = 1;
			MissingBenchmarkDataErrorCount++;
		}

		if ( InPortfolio ( &MySql, xstock.xsticker ) == 1 )
		{
			if ( Printed == 0 ) printf ( "Could not find data for %s PORTFOLIO.\n", xstock.xsticker );
			Printed = 1;
			MissingPortfolioDataErrorCount++;
		}

		if ( MissingOtherDataErrorCount < 10 )
		{
			if ( Printed == 0 ) printf ( "Could not find data for %s.\n", xstock.xsticker );
			Printed = 1;
		}
		else if ( MissingOtherDataErrorCount == 10 )
		{
			printf ( "Not reportiing any more missing data.\n" );
		}
		MissingOtherDataErrorCount++;
	}

	fflush ( stdout );

	return ( 0 );
}
