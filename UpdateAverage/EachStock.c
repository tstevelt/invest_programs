/*----------------------------------------------------------------------------
	Program : UpdateAverage/EachStock.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: For each stock, call the various calculation functions.
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

#include	"UpdateAverage.h"

static int MaxLength ()
{
	int		rv = 0;

	if ( rv < PRICE_PERIOD )
	{
		rv = PRICE_PERIOD;
	}

	if ( rv < VOLUME_PERIOD )
	{
		rv = VOLUME_PERIOD;
	}

	if ( rv < RSI_PERIOD )
	{
		rv = RSI_PERIOD;
	}

	if ( rv < STDDEV_PERIOD )
	{
		rv = STDDEV_PERIOD;
	}

	if ( rv < CTB_PERIOD )
	{
		rv = CTB_PERIOD;
	}

	return ( rv );
}

static void MyLoadHistory ()
{
	int		ZeroCount;
	DATEVAL	YearAgo;
	int		Length;
	char	AverageWhereClause[1024];

	/*----------------------------------------------------------
		now load history for this stock
	----------------------------------------------------------*/
	HistoryCount = 0;
	memset ( HistoryArray, '\0', sizeof(HistoryArray) );
	switch ( RunMode )
	{
		case MODE_ZERO:
			/*----------------------------
				01234567890
				2023-09-30
			----------------------------*/
			YearAgo.month = atoi ( &xstock.xslast[5] );
			YearAgo.day   = atoi ( &xstock.xslast[8] );
			YearAgo.year4 = atoi ( &xstock.xslast[0] ) - 1;

			switch ( Columns )
			{
				case COLUMNS_ALL:
					sprintf ( AverageWhereClause, 
"(Ama5 < 0.01 or Avol50 < 1 or Arsi < 0.01 or Astddev < 0.01 or Actb < 0.01) and Adate >= '%04d-%02d-%02d'  and Aticker = '%s'", 
							YearAgo.year4, YearAgo.month, YearAgo.day, xstock.xsticker );
					Length = MaxLength ();
					break;
				case COLUMNS_PRICE:
					sprintf ( AverageWhereClause, "Ama5 < 0.01 and Adate >= '%04d-%02d-%02d'  and Aticker = '%s'", 
							YearAgo.year4, YearAgo.month, YearAgo.day, xstock.xsticker );
					Length = PRICE_PERIOD;
					break;
				case COLUMNS_VOLUME:
					sprintf ( AverageWhereClause, "Avol50 < 1 and Adate >= '%04d-%02d-%02d'  and Aticker = '%s'", 
							YearAgo.year4, YearAgo.month, YearAgo.day, xstock.xsticker );
					Length = VOLUME_PERIOD;
					break;
				case COLUMNS_RSI:
					sprintf ( AverageWhereClause, "Arsi < 0.01  and Adate >= '%04d-%02d-%02d'  and Aticker = '%s'", 
							YearAgo.year4, YearAgo.month, YearAgo.day, xstock.xsticker );
					Length = RSI_PERIOD;
					break;
				case COLUMNS_STDDEV:
					sprintf ( AverageWhereClause, "Astddev < 0.01 and Adate >= '%04d-%02d-%02d'  and Aticker = '%s'", 
							YearAgo.year4, YearAgo.month, YearAgo.day, xstock.xsticker );
					Length = STDDEV_PERIOD;
					break;
				case COLUMNS_CTB:
					sprintf ( AverageWhereClause, "Actb < 0.01 and Adate >= '%04d-%02d-%02d'  and Aticker = '%s'", 
							YearAgo.year4, YearAgo.month, YearAgo.day, xstock.xsticker );
					Length = CTB_PERIOD;
					break;
				default:
					Length = MaxLength ();
					break;
			}

			ZeroCount = dbySelectCount ( &MySql, "average", AverageWhereClause, LogFileName );

			if ( Debug > 1 )
			{
				printf ( "%s\n", AverageWhereClause );
				printf ( "%d\n", ZeroCount );
			}

			ZeroCount += Length;
			LoadHistoryArray ( &MySql, xstock.xsticker, xstock.xslast, SORT_ASCEND, ZeroCount );
			break;

		case MODE_ALL:
			LoadHistoryArray ( &MySql, xstock.xsticker, xstock.xslast, SORT_ASCEND, 0 );
			break;
	}

	CopyHistoryArrays ();
}

int EachStock ()
{
//	if ( strcmp ( xstock.xsticker, "MMM" ) == 0 )
//	{
//		printf ( "kilroy %s\n", xstock.xsticker );
//	}

	if ( Pacifier )
	{
		static	double PrevTime = -1;
				double ThisTime;

		if ( PrevTime < 0 )
		{
			PrevTime = GetCurrentTime ( 0 );
		}
		ThisTime = GetCurrentTime ( 0 );

		if ( Debug > 1 )
		{
			printf ( "Loading history %s\n", xstock.xsticker );
		}

		if ( getpgrp() != tcgetpgrp(STDOUT_FILENO) )
		{
			static	int		firstpass = 1;
			static	FILE	*fp;
			if ( firstpass )
			{
				firstpass = 0;
				fp = fopen ( "./UpdateAverage.txt", "w" );
			}
			fprintf ( fp, "%s %g\n", xstock.xsticker, ThisTime - PrevTime );
		}
		else
		{
			printf ( "%s %g\n", xstock.xsticker, ThisTime - PrevTime );
			fflush ( stdout );
		}
		PrevTime = ThisTime;
	}

	CountVisited++;
	
	if ( Columns == COLUMNS_ALL || Columns == COLUMNS_CTB )
	{
		/*----------------------------------------------------------
			load history for S&P, in order to calculate CTB
			copy into BenchmarkArray.
		----------------------------------------------------------*/
		if ( CountVisited == 1 && BenchmarkCount == 0 )
		{
			XSTOCK	xbenchmark;
			double	Average;
			int		ndx, xh;
			char	BenchmarkWhereClause[126];

			sprintf ( BenchmarkWhereClause, "Sticker = 'SPY'" );
			if ( LoadStock ( &MySql, BenchmarkWhereClause, &xbenchmark, 0, 0 ) != 1 )
			{
				printf ( "Cannot load history for SPY\n" );
				exit ( 0 );
			}

			LoadHistoryArray ( &MySql, xbenchmark.xsticker, xbenchmark.xslast, SORT_ASCEND, 0 );

			memcpy ( BenchmarkArray, HistoryArray, HistoryCount * sizeof(HISTORY_RECORD) );
			BenchmarkCount = HistoryCount;

			if ( Debug )
			{
				printf  ( "Loaded %d benchmark history records\n", BenchmarkCount );
			}

			/*----------------------------------------------------------
				calc 5 day moving averages, store in Average[AVG_5]
				0 1 2 3 4
			----------------------------------------------------------*/
			for ( ndx = 4; ndx < BenchmarkCount; ndx++ )
			{
				Average = 0.0;
				for ( xh = ndx - 4; xh <= ndx; xh++ )
				{
					Average += BenchmarkArray[xh].Close;
				}
				Average /= 5.0;
				BenchmarkArray[ndx].Average[AVG_5] = Average;
			}
		}
	}

	MyLoadHistory ();

	// HistoryCount = LoadHistoryArrays ( &MySql, xstock.xsticker, xstock.xslast, SORT_ASCEND, 0 );

	if ( HistoryCount < 5 )
	{
		return ( 0 );
	}


	if ( Columns == COLUMNS_ALL || Columns == COLUMNS_PRICE )
	{
// fixit - use HistoryCloseArray
		CalcPriceAvg();
		if ( PriceInserted )
		{
			MyLoadHistory ();
		}
	}

	if ( Columns == COLUMNS_ALL || Columns == COLUMNS_VOLUME )
	{
// fixit - use HistoryCloseArray
		CalcVolumeAvg();
		if ( VolumeInserted )
		{
			MyLoadHistory ();
		}
	}

	if ( Columns == COLUMNS_ALL || Columns == COLUMNS_CTB )
	{
// fixit - use HistoryCloseArray
		CalcCTB();
		if ( CTB_Inserted )
		{
			MyLoadHistory ();
		}
	}

	if ( Columns == COLUMNS_ALL || Columns == COLUMNS_RSI )
	{
		CalcRSI();
		if ( RSI_Inserted )
		{
			MyLoadHistory ();
		}
	}

	if ( Columns == COLUMNS_ALL || Columns == COLUMNS_STDDEV )
	{
		CalcSTDDEV ();
	}

	if ( Debug == 1 )
	{
		printf ( "updated %s, count %d, errors %d\n", xstock.xsticker, ThisCount, ThisError );
	}
	else if ( Debug > 1 )
	{
		printf ( "   Updated %d, errors %d\n", ThisCount, ThisError );
	}

	return ( 0 );
}
