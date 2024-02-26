/*----------------------------------------------------------------------------
	Program : UpdateAverage/CalcRSI.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Calculate Relative Strength Index. Based on 14 days.
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

#undef	DEBUG

static double RsiArray[MAXHISTRECS];

int CalcRSI ()
{
	int		rv, ndx;
	TA_RetCode	TA_RV;
	int         outBegIdx;
	int         outNBElement;
	char		RSI_Statement[1024];

/*---------------------------------------------------------------------------
TA_LIB_API TA_RetCode TA_RSI( int    startIdx,
                              int    endIdx,
                                     const double inReal[],
                                     int           optInTimePeriod,  From 2 to 100000 
                                     int          *outBegIdx,
                                     int          *outNBElement,
                                     double        outReal[] );
---------------------------------------------------------------------------*/

	TA_RV = TA_RSI( 0, HistoryCount-1, HistoryCloseArray, 
							RSI_PERIOD, &outBegIdx, &outNBElement, &RsiArray[RSI_PERIOD] );

	if ( TA_RV != TA_SUCCESS )
	{
		printf ( "TA_RSI() failed on %s, rv %d\n", xstock.xsticker, TA_RV );
		exit ( 1 );
	}

#ifdef DEBUG
	for ( ndx = 0; ndx < HistoryCount; ndx++ )
	{
		printf ( "%d %s %10.4f %10.4f %10.4f\n", 
			ndx, HistoryArray[ndx].Date, HistoryArray[ndx].Close, HistoryCloseArray[ndx], RsiArray[ndx] );
	}
	exit ( 0 );
#endif

	if ( Debug )
	{
		printf ( "   Updating RSI history\n" );
	}

	ThisCount = ThisError = 0;

	for ( ndx = RSI_PERIOD - 1; ndx < HistoryCount; ndx++ )
	{
		if ( RunMode == MODE_ZERO )
		{
			if ( HistoryArray[ndx].RSI > 0.0 )
			{
				continue;
			}
			if ( RsiArray[ndx] == 0 )
			{
				continue;
			}
		}
		else
		{
			if ( HistoryArray[ndx].GotRecord && dbldiff ( HistoryArray[ndx].RSI, RsiArray[ndx], 0.5 ) == 0 )
			{
				continue;
			}
		}

		if ( HistoryArray[ndx].GotRecord == 0 )
		{
			sprintf ( RSI_Statement, 
				"insert into average ( Aticker, Adate, Arsi ) values ( '%s', '%s', %.2f )",
					xstock.xsticker, HistoryArray[ndx].Date, RsiArray[ndx] );

			if ( Debug )
			{
				printf ( "%s;\n", RSI_Statement );
			}
			else
			{
				rv = dbyInsert ( "invest", &MySql, RSI_Statement, 0, LogFileName );
			}

			if ( rv == 1 )
			{
				RSI_Inserted++;
				ThisCount++;
			}
			else
			{
				RSI_Errors++;
				ThisError++;
			}
		}
		else
		{
			sprintf ( RSI_Statement, "update average set Arsi = %.2f where Aid = %ld",
					RsiArray[ndx], HistoryArray[ndx].AverageID );

			if ( Debug )
			{
				printf ( "%s;\n", RSI_Statement );
			}
			else
			{
				rv = dbyUpdate ( "invest", &MySql, RSI_Statement, 0, LogFileName );
			}

			if ( rv == 1 )
			{
				RSI_Updated++;
				ThisCount++;
			}
			else
			{
				RSI_Errors++;
				ThisError++;
			}
		}
	}

	return ( 0 );
}
