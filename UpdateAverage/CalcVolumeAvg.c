/*----------------------------------------------------------------------------
	Program : UpdateAverage/CalcVolumeAvg.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Calculate 50 day volume average.
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

int CalcVolumeAvg ()
{
	long	Volume;
	int		rv, ndx, xh, days;
	char	FundWhereClause[80];
	DBY_QUERY	*Query = NULL;
	char	VolumeStatement[1024];

	ClearFlags ();

	days = 50;

	/*----------------------------
		5: 4 ...
	----------------------------*/
	for ( ndx = days - 1; ndx < HistoryCount; ndx++ )
	{
		if (( RunMode == MODE_ZERO         ) &&
			( HistoryArray[ndx].AvgVol > 0 ))
		{
			continue;
		}

		/*----------------------------
			5: (4-5+1) = 0 1 2 3 4 
		----------------------------*/
		Volume  = 0;
		for ( xh = ndx - days + 1; xh <= ndx; xh++ )
		{
			Volume = Volume + HistoryArray[xh].Volume;
		}
		Volume = Volume / days;
		if ( HistoryArray[ndx].AvgVol != Volume )
		{
			if ( Debug > 1 )
			{
				printf ( "50 %s old %ld new %ld\n", HistoryArray[ndx].Date, HistoryArray[ndx].AvgVol, Volume );
			}

			HistoryArray[ndx].AvgVol = Volume;
			HistoryArray[ndx].Flag++;
		}
	}

	if ( Debug > 1 )
	{
		printf ( "   Updating Volume history\n" );
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
			sprintf ( VolumeStatement, "insert into average ( Aticker, Adate, Avol50 ) \
				values ( '%s', '%s', %ld )",
					xstock.xsticker,
					HistoryArray[ndx].Date,
					HistoryArray[ndx].AvgVol );

			if ( Debug )
			{
				printf ( "%s;\n", VolumeStatement );
			}
			else
			{
				rv = dbyInsert ( "invest", &MySql, VolumeStatement, 0, LogFileName );
			}

			if ( rv == 1 )
			{
				VolumeInserted++;
				ThisCount++;
			}
			else
			{
				VolumeErrors++;
				ThisError++;
			}
		}
		else
		{
			sprintf ( VolumeStatement, "update average set  Avol50 = %ld where Aid = %ld",
					HistoryArray[ndx].AvgVol, HistoryArray[ndx].AverageID );

			if ( Debug )
			{
				printf ( "%s;\n", VolumeStatement );
			}
			else
			{
				rv = dbyUpdate ( "invest", &MySql, VolumeStatement, 0, LogFileName );
			}

			if ( rv == 1 )
			{
				VolumeUpdated++;
				ThisCount++;
			}
			else
			{
				VolumeErrors++;
				ThisError++;
			}
		}
	}

	/*---------------------------------------------------------------------------
		update fundamental.Fvolume
		select Avol50 from average where Aticker = 'XOM' and Adate = (select max(Adate) from average where Aticker = 'XOM');
	---------------------------------------------------------------------------*/
	sprintf ( FundWhereClause, "Fticker = '%s'", xstock.xsticker );
	if ( LoadFundamental ( &MySql, FundWhereClause, &xfundamental, 0 ) == 1 )
	{
		sprintf ( VolumeStatement, 
				"select Avol50 from average where Aticker = '%s' and Adate = (select max(Adate) from average where Aticker = '%s')",
					xstock.xsticker, xstock.xsticker );

		if (( Query = dbySelect ( "invest", &MySql, VolumeStatement, LogFileName )) == NULL )
		{
			fprintf ( stderr, "dbySelect failed %s\n", VolumeStatement );
		}
		else if ( Query->NumRows == 0 )
		{
			fprintf ( stderr, "dbySelect returned zero rows\n" );
		}
		else if (( Query->EachRow = mysql_fetch_row ( Query->Result )) != NULL )
		{
			Volume = nsAtol( Query->EachRow[0] );
			if ( xfundamental.xfvolume != Volume )
			{
				sprintf ( VolumeStatement, "update fundamental set Fvolume = %ld where Fticker = '%s'", 
					Volume, xstock.xsticker );

				if ( dbyUpdate ( "invest", &MySql, VolumeStatement, 0, LogFileName ) < 1 )
				{
					fprintf ( stderr, "dbyUpdate failed %s\n", VolumeStatement );
				}
			}
		}
		dbyFreeQuery ( Query );
	}

	return ( 0 );
}
