/*----------------------------------------------------------------------------
	Program : UpdateAverage/CalcSTDDEV.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Calculate Standard Deviation for past 50 days.
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

static double StddevArray[MAXHISTRECS];

int CalcSTDDEV ()
{
	int		rv, ndx;
	TA_RetCode	TA_RV;
	int         outBegIdx;
	int         outNBElement;
	char		STDDEV_Statement[1024];

/*---------------------------------------------------------------------------
TA_LIB_API TA_RetCode TA_STDDEV( int   		  startIdx,
                                 int   		  endIdx,
                                 const double inReal[],
                                 int           optInTimePeriod, From 2 to 100000
                                 double        optInNbDev, 	    From TA_REAL_MIN to TA_REAL_MAX
                                 int          *outBegIdx,
                                 int          *outNBElement,
                                 double        outReal[] );
---------------------------------------------------------------------------*/

	TA_RV = TA_STDDEV( 0, HistoryCount-1, HistoryCloseArray, 
							STDDEV_PERIOD, 1, &outBegIdx, &outNBElement, &StddevArray[STDDEV_PERIOD-1] );

	if ( TA_RV != TA_SUCCESS )
	{
		printf ( "TA_STDDEV() failed on %s, rv %d\n", xstock.xsticker, TA_RV );
		exit ( 1 );
	}

#ifdef DEBUG
	for ( ndx = 0; ndx < HistoryCount; ndx++ )
	{
		printf ( "%d %s %10.4f %10.4f %10.4f\n", 
			ndx, HistoryArray[ndx].Date, HistoryArray[ndx].Close, HistoryCloseArray[ndx], StddevArray[ndx] );
	}
#endif

	if ( Debug )
	{
		printf ( "   Updating STDDEV history\n" );
	}

	ThisCount = ThisError = 0;

	for ( ndx = STDDEV_PERIOD - 1; ndx < HistoryCount; ndx++ )
	{
		if ( RunMode == MODE_ZERO )
		{
			if ( HistoryArray[ndx].STDDEV > 0.0 )
			{
				continue;
			}
			if ( StddevArray[ndx] == 0 )
			{
				continue;
			}
		}
		else
		{
			if ( HistoryArray[ndx].GotRecord && dbldiff ( HistoryArray[ndx].STDDEV,  StddevArray[ndx], 0.01 ) == 0  )
			{
				continue;
			}
		}

		if ( HistoryArray[ndx].GotRecord == 0 )
		{
			sprintf ( STDDEV_Statement, 
				"insert into average ( Aticker, Adate, Astddev ) values ( '%s', '%s', %.2f )",
					xstock.xsticker, HistoryArray[ndx].Date, StddevArray[ndx] );

			if ( Debug )
			{
				printf ( "%s;\n", STDDEV_Statement );
			}
			else
			{
				rv = dbyInsert ( "invest", &MySql, STDDEV_Statement, 0, LogFileName );
			}

			if ( rv == 1 )
			{
				STDDEV_Inserted++;
				ThisCount++;
			}
			else
			{
				STDDEV_Errors++;
				ThisError++;
			}
		}
		else
		{
			sprintf ( STDDEV_Statement, "update average set Astddev = %.2f where Aid = %ld",
					StddevArray[ndx], HistoryArray[ndx].AverageID );

			if ( Debug )
			{
				printf ( "%s;\n", STDDEV_Statement );
			}
			else
			{
				rv = dbyUpdate ( "invest", &MySql, STDDEV_Statement, 0, LogFileName );
			}

			if ( rv == 1 )
			{
				STDDEV_Updated++;
				ThisCount++;
			}
			else
			{
				STDDEV_Errors++;
				ThisError++;
			}
		}
	}

	return ( 0 );
}
