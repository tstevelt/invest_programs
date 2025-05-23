/*----------------------------------------------------------------------------
	Program : getfx.c
	Author  : Tom Stevelt
	Date    : 04/18/2025
	Synopsis: Get data from data provider, store in mysql invest database
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		04/19/2025	Added Stype2 U H B for small numbers.
	tms		05/03/2025	Use #defines for Stype2

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

#define		MAIN
#include	"getfx.h"

int main ( int argc, char *argv[] )
{
	time_t		stime, etime;
	struct tm	*tm;
	char		tbuffer[30];

	getargs ( argc, argv );

#ifdef HAVE_LIBCURL
	if (( curl = curl_easy_init () ) == NULL )
	{
		fprintf ( stderr, "curl init failed\n" );
		exit ( 1 );
	}
#endif

	GetInvestCfg ( 1 );

	time ( &stime );
	tm = localtime ( &stime );
	if ( Quiet == 0 )
	{
		printf ( "getfx: Start %s", asctime(tm) );
	}

	StartMySQL ( &MySql, "invest" );

	dbyCommitInit ( &MySql );

	switch ( RunMode )
	{
		case MODE_ALL:
			// sprintf ( WhereClause, "Stype = '%c' and (Slast is NULL or (Slast <= '%s' and Slast > '%s'))",  STYPE_FX, Yesterday, MonthAgoDate );
			sprintf ( WhereClause, "Stype = '%c'",  STYPE_FX );
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 1 );
			break;
		case MODE_ONE:
			sprintf ( WhereClause, "Stype = '%c' and Sticker = '%s'", STYPE_FX, xstock.xsticker );
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 1 );
			break;
	}

	switch ( RunMode )
	{
		case MODE_ALL:
		case MODE_ONE:
			if ( StockCount == 1 )
			{
		//		sprintf ( Statement, "update stock set Slast = (select max(Hdate) from history where history.Hticker = '%s');",
		//				xstock.xsticker );
		//		rv = dbyUpdate ( "getfx", &MySql, Statement, 0, LogFileName );
				
				printf ( "Checked %s, inserted %d history records, updated %d Slast, updated %d Fpettm.\n", 
									xstock.xsticker, HistoryCount, SlastCount, PE_Count );
			}
			else
			{
				sprintf ( Statement, "update system set Ystkcnt = %ld, Ystktime = '%s' where system.id = 1",
					(long) HistoryCount, fmtGetTodayDate(DATEFMT_TIMESTAMP) );

				dbyUpdate ( "getfx", &MySql, Statement, 0, LogFileName );
				
		//		sprintf ( Statement, "update stock set Slast = (select max(Hdate) from history where history.Hticker = stock.Sticker);" );
		//		rv = dbyUpdate ( "getfx", &MySql, Statement, 0, LogFileName );

				printf ( "getfx: Checked %d stocks, inserted %d history records, updated %d Slast, updated %d Fpettm.\n", 
									StockCount, HistoryCount, SlastCount, PE_Count );

			}

			dbyCommitClose ( &MySql );

			if ( InsertFailedErrorCount )
			{
				printf ( "getfx: History insert failed %d times.\n", InsertFailedErrorCount );
			}

			if ( MissingMajorDataErrorCount )
			{
				printf ( "getfx: MISSING DATA ON %d SP500 STOCKS!\n", MissingMajorDataErrorCount );
			}

			if ( MissingBenchmarkDataErrorCount )
			{
				printf ( "getfx: MISSING DATA ON %d BENCHMARK STOCKS!\n", MissingBenchmarkDataErrorCount );
			}

			if ( MissingPortfolioDataErrorCount )
			{
				printf ( "getfx: MISSING DATA ON %d PORTFOLIO STOCKS!\n", MissingPortfolioDataErrorCount );
			}

			if ( StillOpenErrorCount )
			{
				printf ( "getfx: Still open on %d stocks!\n", StillOpenErrorCount );
			}

			if ( MissingOtherDataErrorCount )
			{
				printf ( "getfx: Missing data on %d other stocks!\n", MissingOtherDataErrorCount );
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
						fprintf ( ffp, "getfx: MISSING DATA ON %d SP500 STOCKS!\n", MissingMajorDataErrorCount );
					}

					if ( MissingBenchmarkDataErrorCount )
					{
						fprintf ( ffp, "getfx: MISSING DATA ON %d BENCHMARK STOCKS!\n", MissingBenchmarkDataErrorCount );
					}

					if ( MissingPortfolioDataErrorCount )
					{
						fprintf ( ffp, "getfx: MISSING DATA ON %d PORTFOLIO STOCKS!\n", MissingPortfolioDataErrorCount );
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
		printf ( "getfx: End %s, Total %ld seconds, Get Data %d, Put Data %d\n",
				tbuffer, etime - stime, SourceTime, DestinationTime );
		extern double TotalNapTime;
		printf ( "getfx: Total Rate Limit Nap Time %.2f seconds\n", TotalNapTime / 1000.0 );
	}

	if ( Debug /* || RunMode == MODE_ONE */ )
	{
		printf ( "getfx: not removing %s\n", TempFileName );
	}
	else
	{
		unlink ( TempFileName );
	}

	fflush ( stdout );

#ifdef HAVE_LIBCURL
	curl_easy_cleanup ( curl );
#endif

	if ( RunMode == MODE_ONE && MissingOtherDataErrorCount > 0 )
	{
		return ( 1 );
	}
	
	return ( 0 );
}
