/*----------------------------------------------------------------------------
	Program : UpdateAverage/CalcPriceAvg.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Calculate Average Price for 5, 10, 20, 50, 100 and 200 days.
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

static int AvgArray [] = { 5, 10, 20, 50, 100, 200 };

static int AvgCount = sizeof(AvgArray) / sizeof(int);

int CalcPriceAvg ()
{
	double	Average;
	int		rv, ndx, xh, xa, days;
	char	PriceStatement[1024];

	ClearFlags ();

	for ( xa = 0; xa < AvgCount; xa++ )
	{
		days = AvgArray[xa];

		if ( Debug > 1 )
		{
			printf ( "   Calculating %d average\n", days );
		}

		/*----------------------------
			5: 4 ...
		----------------------------*/
		for ( ndx = days - 1; ndx < HistoryCount; ndx++ )
		{
			if (( RunMode == MODE_ZERO              ) &&
				( HistoryArray[ndx].Average[xa] > 0 ))
			{
				continue;
			}

			/*----------------------------
				5: (4-5+1) = 0 1 2 3 4 
			----------------------------*/
			Average = 0.0;
			for ( xh = ndx - days + 1; xh <= ndx; xh++ )
			{
				Average = Average + HistoryArray[xh].Close;
			}
			Average = Average / (double) days;
			if ( HistoryArray[ndx].Average[xa] != Average )
			{
				if ( Debug > 1 )
				{
					printf ( "%d  %s old %.4f new %.4f\n", AvgArray[xa], HistoryArray[ndx].Date,  HistoryArray[ndx].Average[xa], Average );
				}

				if ( dbldiff ( HistoryArray[ndx].Average[xa], Average, 0.01 ) == 1 )
				{
					HistoryArray[ndx].Average[xa] = Average;
					HistoryArray[ndx].Flag++;
				}
			}
		}
	}

	if ( Debug )
	{
		printf ( "   Updating Price history\n" );
	}

	ThisCount = ThisError = 0;
	for ( ndx = 4; ndx < HistoryCount; ndx++ )
	{
		if ( HistoryArray[ndx].Flag == 0 )
		{
			continue;
		}

		if ( HistoryArray[ndx].GotRecord == 0 )
		{
			sprintf ( PriceStatement, "insert into average ( Aticker, Adate, Ama5, Ama10, Ama20, Ama50, Ama100, Ama200 ) \
				values ( '%s', '%s', %.3f, %.3f, %.3f, %.3f, %.3f, %.3f )",
					xstock.xsticker,
					HistoryArray[ndx].Date,
					HistoryArray[ndx].Average[0],
					HistoryArray[ndx].Average[1],
					HistoryArray[ndx].Average[2],
					HistoryArray[ndx].Average[3],
					HistoryArray[ndx].Average[4],
					HistoryArray[ndx].Average[5] );
			
			if ( Debug )
			{
				printf ( "%s;\n", PriceStatement );
			}
			else
			{
				rv = dbyInsert ( "invest", &MySql, PriceStatement, 0, LogFileName );
			}

			if ( rv == 1 )
			{
				PriceInserted++;
				ThisCount++;
			}
			else
			{
				PriceErrors++;
				ThisError++;
			}
		}
		else
		{
			sprintf ( PriceStatement, 
"update average set Ama5 = %.3f, Ama10 = %.3f, Ama20 = %.3f, Ama50 = %.3f, Ama100 = %.3f, Ama200 = %.3f where Aid = %ld",
					HistoryArray[ndx].Average[0],
					HistoryArray[ndx].Average[1],
					HistoryArray[ndx].Average[2],
					HistoryArray[ndx].Average[3],
					HistoryArray[ndx].Average[4],
					HistoryArray[ndx].Average[5],
					HistoryArray[ndx].AverageID );

			if ( Debug )
			{
				printf ( "%s;\n", PriceStatement );
			}
			else
			{
				rv = dbyUpdate ( "invest", &MySql, PriceStatement, 0, LogFileName );
			}

			if ( rv == 1 )
			{
				PriceUpdated++;
				ThisCount++;
			}
			else
			{
				PriceErrors++;
				ThisError++;
			}
		}
	}

	return ( 0 );
}
