/*----------------------------------------------------------------------------
	Program : UpdateAverage/CalcCTB.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Calculate and save Comapred To Benchmark
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

int CalcCTB ()
{
	double	CurrentCompare, YearAgoCompare, CompareToBenchmark;
	int		rv, ndx, nbx, days;
	char	CTB_Statement[1024];

	/*----------------------------------------------------------
		if stock is less than a year old then compare to
		the first date we have
	----------------------------------------------------------*/
	if ( HistoryCount < CTB_PERIOD )
	{
		days = HistoryCount - 1;
	}
	else
	{
		days = CTB_PERIOD;
	}

	if ( Debug )
	{
		printf ( "CalcCTB: History Count = %d, days = %d\n", HistoryCount, days );
	}


	/*----------------------------------------------------------
		calc 5 day moving averages, store in Average[AVG_5]
		2 3 4 5 6
		0 1 2 3 4
	----------------------------------------------------------*/

	for ( ndx = days; ndx < HistoryCount; ndx++ )
	{
		if (( RunMode == MODE_ZERO ) && ( HistoryArray[ndx].CTB > 0 ))
		{
			continue;
		}

		/*----------------------------------------------------------
			find corresponding date in BenchmarkArray
		----------------------------------------------------------*/
		for ( nbx = 0; nbx < BenchmarkCount; nbx++ )
		{
			if ( nsStrcmp ( BenchmarkArray[nbx].Date, HistoryArray[ndx].Date ) == 0 )
			{
				break;
			}
		}

		if ( nbx >= days - 4 && nbx < BenchmarkCount )
		{
			CurrentCompare = HistoryArray[ndx    ].Average[AVG_5] / BenchmarkArray[nbx    ].Average[AVG_5];
			YearAgoCompare = HistoryArray[ndx-252].Average[AVG_5] / BenchmarkArray[nbx-252].Average[AVG_5];
			CompareToBenchmark = 100.0 * CurrentCompare / YearAgoCompare;
		}
		else
		{
			CompareToBenchmark = 0.0;
		}

		if ( Debug > 1 )
		{
			printf ( "CTB %s %.2f\n", HistoryArray[ndx].Date, CompareToBenchmark );
		}

		if ( dbldiff ( HistoryArray[ndx].CTB, CompareToBenchmark, 0.5 ) == 1 )
		{
			HistoryArray[ndx].CTB = CompareToBenchmark;
			HistoryArray[ndx].Flag = 1;
		}
	}

	if ( Debug )
	{
		printf ( "   Updating CTB history\n" );
	}

	ThisCount = ThisError = 0;
	for ( ndx = 0; ndx < HistoryCount; ndx++ )
	{
		if ( HistoryArray[ndx].Flag == 0 )
		{
			continue;
		}

		if ( HistoryArray[ndx].GotRecord == 0 )
		{
			sprintf ( CTB_Statement, 
				"insert into average ( Aticker, Adate, Actb ) values ( '%s', '%s', %.3f )",
					xstock.xsticker, HistoryArray[ndx].Date, HistoryArray[ndx].CTB );

			if ( Debug )
			{
				printf ( "%s;\n", CTB_Statement );
			}
			else
			{
				rv = dbyInsert ( "invest", &MySql, CTB_Statement, 0, LogFileName );
			}

			if ( rv == 1 )
			{
				CTB_Inserted++;
				ThisCount++;
			}
			else
			{
				CTB_Errors++;
				ThisError++;
			}
		}
		else
		{
			sprintf ( CTB_Statement, "update average set Actb = %.3f where Aid = %ld",
					HistoryArray[ndx].CTB, HistoryArray[ndx].AverageID );

			if ( Debug )
			{
				printf ( "%s;\n", CTB_Statement );
			}
			else
			{
				rv = dbyUpdate ( "invest", &MySql, CTB_Statement, 0, LogFileName );
			}

			if ( rv == 1 )
			{
				CTB_Updated++;
				ThisCount++;
			}
			else
			{
				CTB_Errors++;
				ThisError++;
			}
		}
	}

	return ( 0 );
}
