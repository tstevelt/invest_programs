/*----------------------------------------------------------------------------
	Program : getdata/EachStock.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: For each stock, download data from IEX (or Tiingo) and save.
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

#include	"getdata.h"

static int BreakOut ()
{
	return ( -1 );
}

int EachStock ()
{
	int		rv, lineno, Expected;
	int		VolumeIndex = 0;
	FILE	*tfp;
	time_t	stime, etime;
	int		HistoryThisStock;
	char	MaxHistDate[12];
	double	LastClose;
	char	HighDate[12];
	double	NewHigh;
	char	*FormatString;
	char	*Tags[100];
	DATEVAL			dvDate;
	DATETIMEVAL		dtVal;

	switch ( xstock.xstype[0] )
	{
		case STYPE_CRYPTO:
		case STYPE_BOND:
		case STYPE_INDEX:
			return ( 0 );
		default:
			break;
	}

	StockCount++;

	if ( Debug )
	{
		printf ( "%s %s\n", xstock.xsticker, xstock.xslast );
	}

	HistoryThisStock = 0;
	LastClose = 0.0;
	memset ( MaxHistDate, '\0', sizeof(MaxHistDate) );
	NewHigh = xstock.xshigh52;
	memset ( HighDate, '\0', sizeof(HighDate) );
	HistoryThisStock = 0;

	Expected = 6;

	if ( UseTiingo )
	{
		CurrentDateval ( &dvDate );

		if ( nsStrncmp ( xstock.xslast, "(null)", 6 ) == 0 )
		{
			sprintf ( xstock.xslast, "%04d-%02d-%02d", dvDate.year4 - 1, dvDate.month, dvDate.day );
		}

		sprintf ( cmdline, 
			"%s/daily/%s/prices?startDate=%s&format=csv&columns=date,open,high,low,close,volume&token=%s",
				env_ApiURL, xstock.xsticker, 
				xstock.xslast,
				env_ApiKey );
	}
	else
	{

		if ( Format == FORMAT_CSV )
		{
			FormatString = "format=csv";
		}
		else
		{
			FormatString = "";
		}

		switch ( Period )
		{
			case PERIOD_OHLC:
				sprintf ( cmdline, 
					"%s/stock/%s/ohlc?token=%s",
							env_ApiURL, xstock.xsticker, env_ApiKey );
				break;

			case PERIOD_PREVIOUS:
				sprintf ( cmdline, 
					"%s/stock/%s/previous?filter=date,open,high,low,close,volume&%s&token=%s",
							env_ApiURL, xstock.xsticker, FormatString, env_ApiKey );
				break;

			case PERIOD_PAST:
				sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'", xstock.xsticker, PastDate );
				if (  LoadHistoryCB ( &MySql, WhereClause, NULL, &xhistory, BreakOut, 0 ) >= 1 )
				{
					// if ( Debug )
					{
						printf ( "%s %s already loaded in history\n", xstock.xsticker,  xhistory.xhdate );
					}
					return ( 0 );
				}
				/*----------------------------------------------------------------------------------------
					ignores filter=
					date,uClose,uOpen,uHigh,uLow,uVolume,close,open,high,low,volume,change,changePercent,label,changeOverTime,symbol
					2020-02-18,319,315.36,319.75,314.61,38190545,319,315.36,319.75,314.61,38190545,0,0,Feb 18,0,AAPL
				----------------------------------------------------------------------------------------*/
				sprintf ( cmdline, 
					"%s/stock/%s/chart/date/%s?chartByDay=true&%s&token=%s",
							env_ApiURL, xstock.xsticker, PastDate, FormatString, env_ApiKey );
				Expected = 16;
				break;

			case PERIOD_ONE_MONTH:
				sprintf ( cmdline, 
					"%s/stock/%s/chart/1m?%s&token=%s",
							env_ApiURL, xstock.xsticker, FormatString, env_ApiKey );
				Expected = 26;
				break;

			case PERIOD_TWO_YEAR:
				sprintf ( cmdline, 
					"%s/stock/%s/chart/2y?%s&token=%s",
							env_ApiURL, xstock.xsticker, FormatString, env_ApiKey );
				Expected = 26;
				break;

			case PERIOD_THREE_YEAR:
				sprintf ( cmdline, 
					"%s/stock/%s/chart/3y?%s&token=%s",
							env_ApiURL, xstock.xsticker, FormatString, env_ApiKey );
				Expected = 26;
				break;

			case PERIOD_FIVE_YEAR:
				sprintf ( cmdline, 
					"%s/stock/%s/chart/5y?%s&token=%s",
							env_ApiURL, xstock.xsticker, FormatString, env_ApiKey );
				Expected = 26;
				break;

			case PERIOD_TEN_YEAR:
				sprintf ( cmdline, 
					"%s/stock/%s/chart/10y?%s&token=%s",
							env_ApiURL, xstock.xsticker, FormatString, env_ApiKey );
				Expected = 26;
				break;
		}

		IEX_RateLimit ( 0 );
	}

	time ( &stime );

	if ( Debug )
	{
		printf ( "%s\n", cmdline );
		if ( Period == PERIOD_PAST )
		{
			exit ( 1 );
		}
	}

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

		if ( Period == PERIOD_OHLC )
		{
/*---------------------------------------------------------------------------
  0       1      2        3     4              5        6      7      8     9               10    11      12   13   14      15        16       17
{"open":{"price":204.06,"time":1563197400990},"close":{"price":203.3,"time":1562961600729},"high":205.87,"low":204,"volume":12138487,"symbol":"AAPL"}
---------------------------------------------------------------------------*/
			if (( tokcnt = GetTokensA ( buffer, ":,", tokens, MAXTOKS )) < 18 )
			{
				if ( Debug )
				{
					printf ( "tokcnt %d on line %d\n", tokcnt, lineno );
				}
				continue;
			}

			StrToDatetimevalFmt ( tokens[9], DATEFMT_EPOCH_MILLISEC, &dtVal );

#ifdef DEBUG
#define DEBUG
printf ( "open   %s time %s\n", tokens[2], tokens[4] );
printf ( "high   %s\n", tokens[11] );
printf ( "low    %s\n", tokens[13] );
printf ( "close  %s time %s %04d-%02d-%02d\n", tokens[7], tokens[9], dtVal.year4, dtVal.month, dtVal.day );
printf ( "volume %s\n", tokens[15] );
#endif
			xhistory.xhopen = nsAtof(tokens[2]);
			xhistory.xhhigh = nsAtof(tokens[11]);
			xhistory.xhlow  = nsAtof(tokens[13]);
			xhistory.xhclose = nsAtof(tokens[7]);
			xhistory.xhvolume = nsAtol(tokens[15]);
			sprintf ( xhistory.xhdate, "%04d-%02d-%02d", dtVal.year4, dtVal.month, dtVal.day );

if ( nsStrcmp ( tokens[4], tokens[9] ) > 0 )
{
	if ( StillOpenErrorCount < 10 )
	{
		printf ( "Market for %s is currently OPEN!\n", xstock.xsticker );
	}
	else if ( StillOpenErrorCount == 10 )
	{
		printf ( "Not reportiing any more currently OPEN.\n" );
	}
	StillOpenErrorCount++;
	continue;
}

#ifdef DEBUG
continue;
#endif

		}
		else if ( Period == PERIOD_PAST )
		{
			/*----------------------------------------------------------------------------------------
				ignores filter=
				0    1      2     3     4    5       6     7    8    9   10
				date,uClose,uOpen,uHigh,uLow,uVolume,close,open,high,low,volume,change,changePercent,label,changeOverTime,symbol
				2020-02-18,319,315.36,319.75,314.61,38190545,319,315.36,319.75,314.61,38190545,0,0,Feb 18,0,AAPL
			xxx if ( nsStrncmp ( buffer, "date,", 5 ) == 0 )

				format changed, tms december 2020
				0    1     2   3    4      5      6  7   8      9 ...
				close,high,low,open,symbol,volume,id,key,subkey,date,updated,changeOverTime,marketChangeOverTime,uOpen,uClose,uHigh,uLow,uVolume,fOpen,fClose,fHigh,fLow,fVolume,label,change,changePercent

				New format 08/30/2022
				0     1    2   3    4         5      6
				close,high,low,open,priceDate,symbol,volume
			----------------------------------------------------------------------------------------*/

			if ( nsStrncmp ( buffer, "close,", 5 ) == 0 )
			{
				continue;
			}

			if (( tokcnt = GetTokensA ( buffer, ",", tokens, MAXTOKS )) < Expected )
			{
				if ( Debug )
				{
					printf ( "tokcnt %d on line %d\n", tokcnt, lineno );
				}
				continue;
			}

			xhistory.xhclose  = nsAtof(tokens[0]);
			xhistory.xhhigh   = nsAtof(tokens[1]);
			xhistory.xhlow    = nsAtof(tokens[2]);
			xhistory.xhopen   = nsAtof(tokens[3]);
			sprintf ( xhistory.xhdate, "%10.10s", tokens[4] );
			xhistory.xhvolume = nsAtol(tokens[6]);
		}
		else if ( Format == FORMAT_CSV )
		{
			if ( lineno == 1 )
			{
				if ( nsStrstr ( buffer, "fVolume" ) == 0 )
				{
					VolumeIndex = 6;
				}
				else
				{
					VolumeIndex = 23;
				}
				continue;
			}
			if ( nsStrncmp ( buffer, "date,", 5 ) == 0 )
			{
				continue;
			}
			if ( nsStrncmp ( buffer, "close", 5 ) == 0 )
			{
				continue;
			}

			if (( tokcnt = GetTokensA ( buffer, ",", tokens, MAXTOKS )) < Expected )
			{
				if ( Debug )
				{
					printf ( "tokcnt %d on line %d\n", tokcnt, lineno );
				}
				continue;
			}

			switch ( Period )
			{
				case PERIOD_ONE_MONTH:
				case PERIOD_TWO_YEAR:
				case PERIOD_THREE_YEAR:
				case PERIOD_FIVE_YEAR:
				case PERIOD_TEN_YEAR:
// 0     1    2    3   4         5      6      7  8   9      10   11     12              13                   14    15     16    17   18      19    20     21    22   23     24     25     26
// close,high,low,open,priceDate,symbol,volume,id,key,subkey,date,updated,changeOverTime,marketChangeOverTime,uOpen,uClose,uHigh,uLow,uVolume,fOpen,fClose,fHigh,fLow,fVolume,label,change,changePercent
					sprintf ( xhistory.xhdate, "%10.10s", tokens[4] );
					xhistory.xhopen = nsAtof(tokens[19]);
					xhistory.xhclose = nsAtof(tokens[20]);
					xhistory.xhhigh = nsAtof(tokens[21]);
					xhistory.xhlow  = nsAtof(tokens[22]);
					xhistory.xhvolume = nsAtol(tokens[VolumeIndex]);
					break;
				default:
					sprintf ( xhistory.xhdate, "%10.10s", tokens[0] );
					xhistory.xhopen = nsAtof(tokens[1]);
					xhistory.xhhigh = nsAtof(tokens[2]);
					xhistory.xhlow  = nsAtof(tokens[3]);
					xhistory.xhclose = nsAtof(tokens[4]);
					xhistory.xhvolume = nsAtol(tokens[5]);
					break;
			}

		}
		else
		{
			if (( tokcnt = JsonTokens  ( buffer, Tags, tokens, 100 )) < Expected )
			{
				if ( Debug )
				{
					printf ( "tokcnt %d on line %d\n", tokcnt, lineno );
				}
				continue;
			}

			sprintf ( xhistory.xhdate, "%10.10s", tokens[0] );
			xhistory.xhopen = nsAtof(tokens[1]);
			xhistory.xhhigh = nsAtof(tokens[2]);
			xhistory.xhlow  = nsAtof(tokens[3]);
			xhistory.xhclose = nsAtof(tokens[4]);
			xhistory.xhvolume = nsAtol(tokens[5]);
		}

		if ( Period != PERIOD_OHLC && nsStrcmp ( xhistory.xhdate, Today ) == 0 )
		{
			if ( Debug )
			{
				printf ( "Skipping history record for today, stock %s\n", xstock.xsticker );
			}
			continue;
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
			"insert into history (%s) values ( '%s', '%s', %f, %f, %f, %f, %ld )",
				INSERT_FIELDS,
				xstock.xsticker,
				xhistory.xhdate,
				xhistory.xhopen,
				xhistory.xhhigh,
				xhistory.xhlow,
				xhistory.xhclose,
				xhistory.xhvolume );

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

	if ( Period == PERIOD_PAST )
	{
		fflush ( stdout );
		return ( 0 );
	}

	if ( HistoryThisStock > 0 )
	{
		int		rv;

		if (( rv = nsStrcmp ( xstock.xslast, MaxHistDate )) < 0  )
		{
			sprintf ( Statement, 
				"update stock set Slast = '%s', Sclose = %f where Sticker = '%s'", 
						MaxHistDate, LastClose, xstock.xsticker );

			SlastCount += dbyUpdate ( "getdata", &MySql, Statement, 0, LogFileName );
		}

		if ( NewHigh > xstock.xshigh52 )
		{
			xstock.xshigh52 = NewHigh;
			nsStrcpy ( xstock.xsdate52, HighDate );

			sprintf ( Statement, 
				"update stock set Shigh52 = %f, Sdate52 = '%s' where Sticker = '%s'", 
					NewHigh, HighDate, xstock.xsticker );

			if ( dbyUpdate ( "getdata", &MySql, Statement, 0, LogFileName ) != 1 )
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

		if (( rv = dbyUpdate ( "getdata", &MySql, Statement, 0, LogFileName )) != 1 )
		{
			printf ( "Update Shigh52 failed on %s, Sdate52 %s Shigh52 %.2f, rv %d\n", 
					xstock.xsticker, xstock.xsdate52, xstock.xshigh52, rv );
		}

		sprintf ( Statement, 
			"update stock set Sdate52 = (select max(Hdate) from history \
			  where history.Hticker = '%s' and history.Hhigh = stock.Shigh52) where Sticker = '%s'", 
			  		xstock.xsticker, xstock.xsticker ) ;

		if (( rv = dbyUpdate ( "getdata", &MySql, Statement, 0, LogFileName )) != 1 )
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
