/*----------------------------------------------------------------------------
	Program : getdata.c
	Author  : Tom Stevelt
	Date    : 01/17/2019
	Synopsis: Get data from data provider, store in mysql invest database
	Return  : 

	Provider		Site									License		$$
	------------------------------------------------------------------------
	IEX
	QuoteMedia												Commercial	2000/Month
	Tiingo
	IEX Cloud		https://iexcloud.io/console				Commercial	9/Month
	QuoteStream		https://www.quotestream.com/register/	Personal	29/Month

	Xignite			https://www.xignite.com/product/historical-stock-prices#/productoverview	
					52000/Year

	Interesting new site:
					https://site.financialmodelingprep.com/developer/docs


	Who		Date		Modification
	---------------------------------------------------------------------
	tms		04/19/19	Live on IEX Cloud.
	tms		07/15/2019	Added OHLC option for end-of-same-day!
	tms		08/01/2019	Can no longer get NASDAQ vi OHLC
	tms		05/27/2020	Add MissingMajor to bail where too many [SP500] are missing.
	tms		07/13/2021	Get private api keys with GetInvestCfg().
	tms		06/18/2022	Replaced curl system calls with libcurl functions.
	tms		05/05/2023	IEX rate limit appears to be 5 request per second.
	tms		09/26/2023	Added -splits, to check price one year ago
	tms		11/07/2023	Update per share fields in fundamental table.
	tms		02/04/2024	Default format is csv.
	tms		02/04/2024	Changed historial to get fully adjusted numbers.
	tms		02/04/2024	Changed splits to use fully adjusted close
	tms		05/16/2024	Moved CheckSplits() to getsplits program
	tms		05/16/2024	Changed historial back to historical (not adjusted)

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

#define		MAIN
#include	"getdata.h"

int main ( int argc, char *argv[] )
{
	time_t		stime, etime;
	struct tm	*tm;
	char		tbuffer[30];

	getargs ( argc, argv );

	if (( curl = curl_easy_init () ) == NULL )
	{
		fprintf ( stderr, "curl init failed\n" );
		exit ( 1 );
	}

	GetInvestCfg ( UseTiingo );

	time ( &stime );
	tm = localtime ( &stime );
	if ( Quiet == 0 )
	{
		printf ( "getdata: Start %s", asctime(tm) );
	}

	StartMySQL ( &MySql, "invest" );

	dbyCommitInit ( &MySql );

	switch ( RunMode )
	{
		case MODE_ALL:
			if ( Period == PERIOD_OHLC )
			{
				sprintf ( WhereClause, "Slast < '%s' and Slast > '%s' and Sexchange != 'NASDAQ'", Today, MonthAgoDate );
			}
			else if ( Period == PERIOD_PAST )
			{
				sprintf ( WhereClause, "Slast > '%s'", MonthAgoDate );
			}
			else
			{
				sprintf ( WhereClause, "Slast < '%s' and Slast > '%s'", Yesterday, MonthAgoDate );
			}
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 1 );
			break;
		case MODE_NULL:
			if ( Period == PERIOD_OHLC )
			{
				sprintf ( WhereClause, "Slast is NULL and Sexchange != 'NASDAQ'" );
			}
			else
			{
				sprintf ( WhereClause, "Slast is NULL" );
			}
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 1 );
			break;
		case MODE_ONE:
			if ( Period == PERIOD_OHLC )
			{
				sprintf ( WhereClause, "Sticker = '%s' and Sexchange != 'NASDAQ'", xstock.xsticker );
			}
			else
			{
				sprintf ( WhereClause, "Sticker = '%s'", xstock.xsticker );
			}
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 1 );
			break;
	}

	switch ( RunMode )
	{
		case MODE_ALL:
		case MODE_NULL:
		case MODE_ONE:
			if ( StockCount == 1 )
			{
		//		sprintf ( Statement, "update stock set Slast = (select max(Hdate) from history where history.Hticker = '%s');",
		//				xstock.xsticker );
		//		rv = dbyUpdate ( "getdata", &MySql, Statement, 0, LogFileName );
				
				printf ( "Checked %s, inserted %d history records, updated %d Slast, updated %d Fpettm.\n", 
									xstock.xsticker, HistoryCount, SlastCount, PE_Count );
			}
			else
			{
				sprintf ( Statement, "update system set Ystkcnt = %ld, Ystktime = '%s' where system.id = 1",
					(long) HistoryCount, fmtGetTodayDate(DATEFMT_TIMESTAMP) );

				dbyUpdate ( "getdata", &MySql, Statement, 0, LogFileName );
				
		//		sprintf ( Statement, "update stock set Slast = (select max(Hdate) from history where history.Hticker = stock.Sticker);" );
		//		rv = dbyUpdate ( "getdata", &MySql, Statement, 0, LogFileName );

				printf ( "getdata: Checked %d stocks, inserted %d history records, updated %d Slast, updated %d Fpettm.\n", 
									StockCount, HistoryCount, SlastCount, PE_Count );

			}

			dbyCommitClose ( &MySql );

			if ( InsertFailedErrorCount )
			{
				printf ( "getdata: History insert failed %d times.\n", InsertFailedErrorCount );
			}

			if ( MissingMajorDataErrorCount )
			{
				printf ( "getdata: MISSING DATA ON %d SP500 STOCKS!\n", MissingMajorDataErrorCount );
			}

			if ( MissingBenchmarkDataErrorCount )
			{
				printf ( "getdata: MISSING DATA ON %d BENCHMARK STOCKS!\n", MissingBenchmarkDataErrorCount );
			}

			if ( MissingPortfolioDataErrorCount )
			{
				printf ( "getdata: MISSING DATA ON %d PORTFOLIO STOCKS!\n", MissingPortfolioDataErrorCount );
			}

			if ( StillOpenErrorCount )
			{
				printf ( "getdata: Still open on %d stocks!\n", StillOpenErrorCount );
			}

			if ( MissingOtherDataErrorCount )
			{
				printf ( "getdata: Missing data on %d other stocks!\n", MissingOtherDataErrorCount );
			}

			if (( FlagFile ) && 
				( MissingMajorDataErrorCount > 100 || MissingBenchmarkDataErrorCount || MissingPortfolioDataErrorCount ))
			{
				FILE	*ffp;

				if (( ffp = fopen ( FlagFile, "w" )) == (FILE *)0 )
				{
					printf ( "Cannot create -flag %s\n", FlagFile );
				}
				else
				{
					if ( MissingMajorDataErrorCount )
					{
						fprintf ( ffp, "getdata: MISSING DATA ON %d SP500 STOCKS!\n", MissingMajorDataErrorCount );
					}

					if ( MissingBenchmarkDataErrorCount )
					{
						fprintf ( ffp, "getdata: MISSING DATA ON %d BENCHMARK STOCKS!\n", MissingBenchmarkDataErrorCount );
					}

					if ( MissingPortfolioDataErrorCount )
					{
						fprintf ( ffp, "getdata: MISSING DATA ON %d PORTFOLIO STOCKS!\n", MissingPortfolioDataErrorCount );
					}

					nsFclose ( ffp );
				}
			}

			if ( SubjectFile )
			{
				FILE	*ffp;

				if (( ffp = fopen ( SubjectFile, "w" )) == (FILE *)0 )
				{
					printf ( "Cannot create -subj %s\n", SubjectFile );
				}
				else
				{
/*----------------------------------------------------------
InsertFailedErrorCount
MissingMajorDataErrorCount
MissingBenchmarkDataErrorCount
MissingPortfolioDataErrorCount
MissingOtherDataErrorCount
StillOpenErrorCount
----------------------------------------------------------*/

					if ( InsertFailedErrorCount + MissingMajorDataErrorCount + MissingBenchmarkDataErrorCount + MissingPortfolioDataErrorCount + MissingOtherDataErrorCount + StillOpenErrorCount > 0 )
					{
						fprintf ( ffp, "Get Data and Update: fail %d missing %d %d %d %d open %d",
								InsertFailedErrorCount,
								MissingMajorDataErrorCount,
								MissingBenchmarkDataErrorCount,
								MissingPortfolioDataErrorCount,
								MissingOtherDataErrorCount,
								StillOpenErrorCount );
					}
					else
					{
						fprintf ( ffp, "Get Data and Update" );
					}


					nsFclose ( ffp );
				}
			}

			break;

		default:
			printf ( "Did not update system or stocks\n" );
			break;
	}

	if ( Quiet == 0 )
	{
		time ( &etime );
		tm = localtime ( &etime );
		asctime_r ( tm, tbuffer );
		TrimRightAndLeft ( tbuffer );
		printf ( "getdata: End %s, Total %ld seconds, Get Data %d, Put Data %d\n",
				tbuffer, etime - stime, SourceTime, DestinationTime );
		extern double TotalNapTime;
		printf ( "getdata: Total Rate Limit Nap Time %.2f seconds\n", TotalNapTime / 1000.0 );
	}

	if ( Debug /* || RunMode == MODE_ONE */ )
	{
		printf ( "getdata: not removing %s\n", TempFileName );
	}
	else
	{
		unlink ( TempFileName );
	}

	fflush ( stdout );

	curl_easy_cleanup ( curl );

	if ( RunMode == MODE_ONE && MissingOtherDataErrorCount > 0 )
	{
		return ( 1 );
	}
	
	return ( 0 );
}
