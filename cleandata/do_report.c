/*----------------------------------------------------------------------------
	Program : cleandata/do_report.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Compare history to current data from IEX.
	Return  : 
----------------------------------------------------------------------------*/
//     Invest extras
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

#include	"cleandata.h"

#define		CHECK_VOLUME_BREAK
#undef		CHECK_VOLUME_BREAK

static	char	TempFileName[128];
static int StockCount;

static char *IndexMember ()
{
	if (( xstock.xsdj[0]      == 'Y' ) ||
		( xstock.xssp500[0]   == 'Y' ) ||
		( xstock.xsnasdaq[0]  == 'Y' ) ||
		( xstock.xsrussell[0] == '1' ) ||
		( xstock.xsrussell[0] == '2' ))
	{
		return ( " index member" );
	}
	
	return ( "" );
}

typedef struct
{
	char	Date[13];
	DATEVAL	dvDate;
	double	Open;
	double	Low;
	double	High;
	double	Close;
	long	Volume;
} RECORD;

#define			MAXRECS		(10*255)
/*----------------------------------------------------------
static	RECORD		Array[MAXRECS];
----------------------------------------------------------*/
static	RECORD		*Array;
static	int			ArraySize = MAXRECS;
static	int			Count;

static int EachHistory ()
{
	if (( xhistory.xhclose > 0.0 ) && ( xhistory.xhopen == 0.0  ))
	{
		printf ( "update history set Hopen = Hclose where Hticker = '%s' and Hdate = '%s' ;\n", xhistory.xhticker, xhistory.xhdate );
	}
	if (( xhistory.xhclose > 0.0 ) && ( xhistory.xhlow == 0.0   ))
	{
		printf ( "update history set Hlow = Hclose where Hticker = '%s' and Hdate = '%s' ;\n", xhistory.xhticker, xhistory.xhdate );
	}
	if (( xhistory.xhclose > 0.0 ) && ( xhistory.xhhigh == 0.0  ))
	{
		printf ( "update history set Hhigh = Hclose where Hticker = '%s' and Hdate = '%s' ;\n", xhistory.xhticker, xhistory.xhdate );
	}

	if ( xhistory.xhclose == 0.0 )
	{
		printf ( "%s %s zero close %.2f%s\n", xhistory.xhticker, xhistory.xhdate, xhistory.xhclose, IndexMember() );
	}

#ifdef YAP_ZERO_VOLUME
#define YAP_ZERO_VOLUME
	if ( xhistory.xhvolume == 0  )
	{
		printf ( "%s %s zero volume %ld%s\n", xhistory.xhticker, xhistory.xhdate, xhistory.xhvolume, IndexMember() );
	}
#endif

	if ( Count >= ArraySize )
	{
		printf ( "Exceeds ArraySize %s %s%s\n", xstock.xsticker, xhistory.xhdate, IndexMember() );
		Count = -1;
		return ( -1 );
	}

	if ( Count == 0 && RunMode == MODE_REPORT_ONE )
	{
		printf ( "%s first date is %s\n", xhistory.xhticker, xhistory.xhdate );
	}

	snprintf ( Array[Count].Date, sizeof(Array[Count].Date), "%s", xhistory.xhdate );
	StrToDatevalFmt ( xhistory.xhdate, DATEFMT_YYYY_MM_DD, &Array[Count].dvDate );
	Array[Count].Open = xhistory.xhopen;
	Array[Count].Low = xhistory.xhlow;
	Array[Count].High = xhistory.xhhigh;
	Array[Count].Close = xhistory.xhclose;
	Array[Count].Volume = xhistory.xhvolume;
	Count++;
	return ( 0 );
}

static void FlagStock ()
{
	sprintf ( Statement, "update stock set Scheck = 'Y' where Sticker = '%s' and Scheck != 'Y'", xstock.xsticker );
	dbyUpdate ( "invest", &MySql, Statement, 0, LogFileName );
	FlagCount++;
}

static int EachStock ()
{
	int		rv, ndx, xh, DateGapCount, Days;
//	int		ndxOne, ndxTwo,
//	double	AvgOne, AvgTwo;
//	DATEVAL		dvTestDate;
	char		cmdline[1024];
	FILE		*fp;
	char		xbuffer[1024];
	char		*tokens[100];
	char		*Tags[100];
	int			tokcnt;
	XHISTORY	xdiff;
	char		TestDate[12];

	StockCount++;
	Count = 0;

	//printf( "%s %s\n", xstock.xsticker, xstock.xsname );
	if ( nsStrcmp ( xstock.xslast, SelectDate ) < 0 )
	{
		printf( "Check last date %s %s%s\n", xstock.xsticker, xstock.xslast, IndexMember() );
		FlagStock ();
	}

	sprintf ( WhereClause, "Hticker = '%s'", xstock.xsticker );
	if (( rv =  LoadHistoryCB ( &MySql, WhereClause, "Hdate", &xhistory, (int(*)()) EachHistory, 0 )) < 1 )
	{
		printf( "Load History for %s returned %d%s\n", xstock.xsticker, rv, IndexMember() );
		FlagStock ();
		return ( 0 );
	}

	if ( Count < 1 )
	{
		FlagStock ();
		return ( 0 );
	}

	DateGapCount = 0;
	for ( ndx = 1; ndx < Count; ndx++ )
	{
		Days = DateDiff ( &Array[ndx-1].dvDate, &Array[ndx].dvDate );
		if ( Days > 5 )
		{
			DateGapCount++;

			if ( DateGapCount == 1 || RunMode == MODE_REPORT_ONE )
			{
				printf ( "%s %sgap at %s to %s%s\n", 
					xstock.xsticker, 
					DateGapCount == 1 ? " first" : "",
					Array[ndx-1].Date, 
					Array[ndx].Date, 
					IndexMember() );
			}
		}
	}
	if ( DateGapCount > 0 )
	{
		printf( "%s has %d date gaps%s\n", xstock.xsticker, DateGapCount, IndexMember() );
		FlagStock ();
		return ( 0 );
	}
	if ( nsStrcmp ( Array[Count-1].Date, xstock.xslast ) < 0 )
	{
		printf( "update stock set Slast = '%s' where Sticker = '%s' ;\n", Array[Count-1].Date, xstock.xsticker );
		return ( 0 );
	}

	/*------------------------------------------------------------------
		new way - check an old record against current data source.
		curl -s "$BASEURL/stock/$2/chart/date/$4?chartByDay=true&token=$TOKEN"
	------------------------------------------------------------------*/

	if ( Count > 50 )
	{
		ndx = 10;

		sprintf ( cmdline, "curl -s '%s/stock/%s/chart/date/%04d%02d%02d?chartByDay=true&token=%s' > %s",
			env_ApiURL, xstock.xsticker, Array[ndx].dvDate.year4, Array[ndx].dvDate.month, Array[ndx].dvDate.day, env_ApiKey, TempFileName );

		sprintf ( TestDate, "%04d-%02d-%02d", Array[ndx].dvDate.year4, Array[ndx].dvDate.month, Array[ndx].dvDate.day );

		if ( Debug )
		{
			printf ( "%s\n", cmdline );
		}

		IEX_RateLimit ( 0 );

		/*----------------------------------------------------------
			fixit - still using curl system() instead of library.
		----------------------------------------------------------*/

		system ( cmdline );

		if (( fp = fopen ( TempFileName, "r" )) == (FILE *)0 )
		{
			printf ( "Cannot open %s\n", TempFileName );
		}
		else
		{
			if ( fgets ( xbuffer, sizeof(xbuffer), fp ) == (char *)0 )
			{
				printf ( "Empty file %s\n", TempFileName );
			}
			else if (( tokcnt = JsonTokens ( xbuffer, Tags, tokens, 100 )) < 15 )
			{
				printf ( "Token count %d\n", tokcnt );
				
			}
			else
			{
				xhistory.xhclose = 0.0;
				xhistory.xhopen  = 0.0;
				xhistory.xhhigh  = 0.0;
				xhistory.xhlow   = 0.0;
				xhistory.xhvolume = 0L;
				xhistory.xhdate[0] = '\0';
				for ( xh = 0; xh < tokcnt; xh++ )
				{
					if ( Debug )
					{
						printf ( "%s = %s\n", Tags[xh], tokens[xh] );
					}
					if ( nsStrcmp ( Tags[xh], "close" ) == 0 )
					{
						xhistory.xhclose = nsAtof(tokens[xh]);
					}
					else if ( nsStrcmp ( Tags[xh], "open" ) == 0 )
					{
						xhistory.xhopen = nsAtof(tokens[xh]);
					}
					else if ( nsStrcmp ( Tags[xh], "high" ) == 0 )
					{
						xhistory.xhhigh = nsAtof(tokens[xh]);
					}
					else if ( nsStrcmp ( Tags[xh], "low" ) == 0 )
					{
						xhistory.xhlow = nsAtof(tokens[xh]);
					}
					else if ( nsStrcmp ( Tags[xh], "volume" ) == 0 )
					{
						xhistory.xhvolume = nsAtol(tokens[xh]);
						break;
					}
					else if ( nsStrcmp ( Tags[xh], "priceDate" ) == 0 )
					{
						sprintf ( xhistory.xhdate, "%s", tokens[xh] );
					}
				}

				if ( strcmp ( xhistory.xhdate, TestDate ) != 0 )
				{
					printf ( "Date mismatch %s != %s\n", xhistory.xhdate, TestDate );
				}

/*---------------------------------------------------------------------------
SPY first date is 2021-05-18
SPY data mismatched 2021-06-02 open 417.85 420.37 high 419.99 421.23 low 416.28 419.29 close 420.33 420.33 volume 58138763 49097061
Examined 1 stocks
Flagged 1 stocks


[{"close":420.33,"high":421.23,"low":419.29,"open":420.37,"priceDate":"2021-06-02","symbol":"SPY","volume":49097061,"id":"HISTORICAL_PRICES","key":"SPY","subkey":"","date":"2021-06-02","updated":1715904190453.4507,"changeOverTime":0,"marketChangeOverTime":0,"uOpen":420.37,"uClose":420.33,"uHigh":421.23,"uLow":419.29,"uVolume":49097061,"fOpen":405.026495,"fClose":404.987955,"fHigh":405.855105,"fLow":403.985915,"fVolume":49097061,"label":"Jun 2, 21","change":0,"changePercent":0},
{"close":418.77,"high":419.99,"low":416.28,"open":417.85,"priceDate":"2021-06-03","symbol":"SPY","volume":58138763,"id":"HISTORICAL_PRICES","key":"SPY","subkey":"","date":"2021-06-03","updated":1715904190453.4507,"changeOverTime":-0.003711369638141466,"marketChangeOverTime":-0.003711369638141466,"uOpen":417.85,"uClose":418.77,"uHigh":419.99,"uLow":416.28,"uVolume":58138763,"fOpen":402.598475,"fClose":403.484895,"fHigh":404.660365,"fLow":401.08578,"fVolume":58138763,"label":"Jun 3, 21","change":-1.5600000000000023,"changePercent":-0.0037},


---------------------------------------------------------------------------*/

				if (( xhistory.xhclose > 0.01 ) &&
					( xhistory.xhopen  > 0.01 ) &&
					( xhistory.xhhigh  > 0.01 ) &&
					( xhistory.xhlow   > 0.01 ))
				{
					xdiff.xhclose  = xhistory.xhclose  - Array[ndx].Close;
					xdiff.xhopen   = xhistory.xhopen   - Array[ndx].Open;
					xdiff.xhhigh   = xhistory.xhhigh   - Array[ndx].High;
					xdiff.xhlow    = xhistory.xhlow    - Array[ndx].Low;
					xdiff.xhvolume = xhistory.xhvolume - Array[ndx].Volume;

					if (( xdiff.xhclose  > 0.01 ) || ( xdiff.xhclose  < -0.01 ) ||
						( xdiff.xhopen   > 0.01 ) || ( xdiff.xhopen   < -0.01 ) ||
						( xdiff.xhhigh   > 0.01 ) || ( xdiff.xhhigh   < -0.01 ) ||
						( xdiff.xhlow    > 0.01 ) || ( xdiff.xhlow    < -0.01 ) ||
						( xdiff.xhvolume > 1    ) || ( xdiff.xhvolume < -1    ))
					{
						printf( "%s data mismatched %s open %.2fI %.2fH high %.2fI %.2fH low %.2fI %.2fH close %.2fI %.2fH volume %ldI %ldH%s\n", 
							xstock.xsticker, 
							Array[ndx].Date, 
							xhistory.xhopen,   Array[ndx].Open,
							xhistory.xhhigh,   Array[ndx].High,
							xhistory.xhlow,    Array[ndx].Low,
							xhistory.xhclose,  Array[ndx].Close,
							xhistory.xhvolume, Array[ndx].Volume,
							IndexMember() );

						FlagStock ();
					}
				}
			}
			
			nsFclose ( fp );
		}

	}

	return ( 0 );
}

void do_report ()
{
	printf ( "!!! THIS IS TOO EXPENSIVE, ABORTED !!!\n" );
	exit ( 1 );

	StockCount = 0;

	sprintf ( TempFileName, "%s/cleandata_%d.csv", "/var/local/tmp", getpid() );

	switch ( RunMode )
	{
		case MODE_REPORT_ALL:
			if (( Array  = calloc ( MAXRECS, sizeof(RECORD) )) == NULL )
			{
				fprintf ( stderr, "calloc failed\n" );
				exit ( 1 );
			}
			sprintf ( WhereClause, "Stype != 'C' and Stype != 'B' and (select count(*) from history where Sticker = Hticker) > 0" );
			break;
		case MODE_REPORT_ONE:
			sprintf ( WhereClause, "Hticker = '%s'", Ticker );
			ArraySize = dbySelectCount ( &MySql, "history", WhereClause, LogFileName );
			if ( ArraySize < MAXRECS )
			{
				ArraySize = MAXRECS;
			}
			if (( Array  = calloc ( ArraySize, sizeof(RECORD) )) == NULL )
			{
				fprintf ( stderr, "calloc failed\n" );
				exit ( 1 );
			}
			sprintf ( WhereClause, "Stype != 'C' and Stype != 'B' and Sticker = '%s'", Ticker );
			break;
		default:
			printf ( "do_report: unknown run mode\n" );
			return;
	}
	LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 0 );

	printf ( "Examined %d stocks\n", StockCount );

	if ( Debug )
	{
		printf ( "not removiing %s\n", TempFileName );
	}
	else
	{
		unlink ( TempFileName );
	}
}
