/*----------------------------------------------------------------------------
	Program : UpdateAverage.c
	Author  : Tom Stevelt
	Date    : 03/18/2019
	Synopsis: Update various averages after history is updated.

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		04/08/2019	Moved averages from history to average table.
	tms		05/27/2019	Added average.Arsi
						Broke EachStock into three calls.
	tms		08/21/2023	Yahoo quit sending averageDailyVolume10Day, so
						have to update fundamental.Fvolume
	tms		09/22/2023	Added average.Astddev
	tms		09/24/2023	Renamed RSI to CTB (Compare to Benchmark). (Not used).
						Added standard RSI
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
#include	"UpdateAverage.h"

extern	char StockWhereClause[];

int main ( int argc, char *argv[] )
{
	getargs ( argc, argv );

	StartMySQL ( &MySql, "invest" );

	dbyCommitInit ( &MySql );

	/*----------------------------------------------------------
		StockWhereClause is set in getargs().
	----------------------------------------------------------*/
	LoadStockCB ( &MySql, StockWhereClause, "Sticker", &xstock, (int(*)()) EachStock, 0 );

	dbyCommitClose ( &MySql );

	printf ( "UpdateAverage: Checked %4d stocks\n", CountVisited );
	printf ( "  PriceAvg  : inserted %4d, updated %4d, errors %4d\n", PriceInserted, PriceUpdated, PriceErrors );
	printf ( "  Volume Avg: inserted %4d, updated %4d, errors %4d\n", VolumeInserted, VolumeUpdated, VolumeErrors );
	printf ( "  RSI       : inserted %4d, updated %4d, errors %4d\n", RSI_Inserted, RSI_Updated, RSI_Errors );
	printf ( "  STDDEV    : inserted %4d, updated %4d, errors %4d\n", STDDEV_Inserted, STDDEV_Updated, STDDEV_Errors );
	printf ( "  CTB       : inserted %4d, updated %4d, errors %4d\n", CTB_Inserted, CTB_Updated, CTB_Errors );

	return ( 0 );
}
