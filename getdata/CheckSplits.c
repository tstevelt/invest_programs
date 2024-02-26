/*----------------------------------------------------------------------------
	Program : getdata/CheckSplits.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: This function would be better named CheckAdjustments.  As 
				corporate acions (including splits) change the fully adjusted 
				fields on IEX, we have to reload all the historical data for 
				affected stocks.   This function checks the data from one
				year ago.
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

static int DebugCheckSplits = 0;
static char IEX_Date[12];

static int CheckStock ( XSTOCK *ptr )
{
	char	HistoryWhereClause[128];
	FILE	*tfp;
	double	HistClose, IEX_Close, Percent;
	static	int		Retry = 1;

	switch ( ptr->xstype[0] )
	{
		case STYPE_CRYPTO:
		case STYPE_BOND:
		case STYPE_INDEX:
			return ( 0 );
		default:
			break;
	}

	sprintf ( HistoryWhereClause, "Hticker = '%s' and Hdate = '%s'", ptr->xsticker, YearAgoDate );
	if ( LoadHistory ( &MySql, HistoryWhereClause, &xhistory, 0 ) != 1 )
	{
		printf ( "No history record for %s on %s\n", ptr->xsticker, YearAgoDate );
		fflush ( stdout );
		return ( 0 );
	}
	HistClose = xhistory.xhclose;

	sprintf ( cmdline, "%s/stock/%s/chart/date/%s?chartByDay=true&format=csv&token=%s",
							env_ApiURL, ptr->xsticker, IEX_Date,  env_ApiKey );
	if ( DebugCheckSplits )
	{
		printf ( "%s\n", cmdline );
		fflush ( stdout );
	}

	IEX_RateLimit ( 0 );

	if (( tfp = fopen ( TempFileName, "w" )) == (FILE *)0 )
	{
		printf ( "Cannot create temp file %s\n", TempFileName );
		printf ( "%s\n", strerror(errno) );
		fflush ( stdout );

		sprintf ( TempFileName, "%s/getdata_%d.csv", TEMPDIR, Retry+getpid() );
		Retry++;
		
		nsFclose ( tfp );
		if (( tfp = fopen ( TempFileName, "w" )) == (FILE *)0 )
		{
			printf ( "Cannot create retry temp file %s\n", TempFileName );
			printf ( "%s\n", strerror(errno) );
			fflush ( stdout );
			return ( 0 );
		}
	}

	curl_easy_setopt ( curl, CURLOPT_URL, cmdline );
	curl_easy_setopt ( curl, CURLOPT_WRITEDATA, tfp );

	if (( curlRV = curl_easy_perform ( curl )) != CURLE_OK )
	{
		fprintf ( stderr, "perform failed: %s\n", curl_easy_strerror(curlRV) );
		return ( 0 );
	}

	nsFclose ( tfp );

	if (( tfp = fopen ( TempFileName, "r" )) == (FILE *)0 )
	{
		printf ( "fopen failed\n" );
		fflush ( stdout );
		return ( 0 );
	}


	int lineno = 0;
	while ( fgets ( buffer, sizeof(buffer), tfp ) != (char *)0 )
	{
		lineno++;

		if ( nsStrncmp ( buffer, "Unknown symbol", 14 ) == 0 )
		{
			printf ( "IEX says unknown ticker %s\n", xstock.xsticker );
			fflush ( stdout );
			return ( 0 );
		}
		if ( nsStrncmp ( buffer, "close,", 5 ) == 0 )
		{
			continue;
		}

#ifdef NOT_FULLY_ADJUSTED
		IEX_Close = nsAtof(buffer);
#else
		if (( tokcnt = GetTokensA ( buffer, ",\r\n", tokens, MAXTOKS )) < 27 )
		{
			if ( Debug )
			{
				printf ( "tokcnt %d on line %d\n", tokcnt, lineno );
			}
			continue;
		}

// 0     1    2    3   4         5      6      7  8   9      10   11     12              13                   14    15     16    17   18      19    20     21    22   23     24     25     26
// close,high,low,open,priceDate,symbol,volume,id,key,subkey,date,updated,changeOverTime,marketChangeOverTime,uOpen,uClose,uHigh,uLow,uVolume,fOpen,fClose,fHigh,fLow,fVolume,label,change,changePercent

		IEX_Close = nsAtof(tokens[20]);
#endif

		if ( IEX_Close < 0.01 )
		{
			printf ( "Date missing close: %s\n", buffer );
			fflush ( stdout );
			return ( 0 );
		}

		break;
	}

	nsFclose ( tfp );

	if ( lineno != 2 )
	{
		printf ( "%s weirdness, lineno %d\n", ptr->xsticker, lineno );
		fflush ( stdout );
	}
	else
	{
		Percent = fabs ( 100.0 * ( IEX_Close - HistClose ) / IEX_Close );	

		if ( Percent > 0.5 )
		{
			printf ( "%s history %.2f IEX %.2f Percent %.2f\n",
				ptr->xsticker, HistClose, IEX_Close, Percent );
			fflush ( stdout );
		}
	}
	
	return ( 0 );
}

void CheckSplits ()
{
	/*----------------------------------------------------------
		YearAgo was calculated in getargs but not verified that
		it waa a market day. if not, find nearby market day.
	----------------------------------------------------------*/
	sprintf ( WhereClause, "Hticker = 'SPY' and Hdate = '%s'", YearAgoDate );
	if ( LoadHistory ( &MySql, WhereClause, &xhistory, 0 ) != 1 )
	{
		printf ( "YearAgoDate not a market day\n" );
		fflush ( stdout );

		sprintf ( Statement, "select max(Hdate) from history where Hticker = 'SPY' and Hdate < '%s'", YearAgoDate );
		char *cp = dbySelectDate ( &MySql, Statement, LogFileName );
		sprintf ( YearAgoDate, "%s", cp );
		printf ( "Set YearAgoDate to %s\n", YearAgoDate );
		fflush ( stdout );
	}
	else if ( DebugCheckSplits )
	{
		printf ( "YearAgoDate is okay\n" );
		fflush ( stdout );
	}

	/*----------------------------
		0123456789
		2022-09-23
select Stype, count(*) from stock group by Stype
+-------+----------+
| Stype | count(*) |
+-------+----------+
| A     |      210 |
| B     |       41 |
| C     |       12 |
| E     |     2311 |
| S     |     3385 |
| X     |        2 |
+-------+----------+
	----------------------------*/
	sprintf ( IEX_Date, "%4.4s%2.2s%2.2s", &YearAgoDate[0], &YearAgoDate[5], &YearAgoDate[8] );

	sprintf ( WhereClause, "Slast is not NULL and Stype != 'B' and Stype != 'C' and Stype != 'X'" );
	LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) CheckStock, 0 );


}
