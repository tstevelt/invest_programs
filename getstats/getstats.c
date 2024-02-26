/*----------------------------------------------------------------------------
	Program : getstats.c
	Author  : Tom Stevelt
	Date    : 08/08/2019
	Synopsis: Get company key stats from IEX Cloud.  Print report for display.

	Provider		Site								License		$$
	-----------------------------------------------------------------------
	IEX Cloud		https://iexcloud.io/console			Commercial	9/Month

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		07/13/2021	Get private api keys with GetInvestCfg().
	tms		2023		Disabled get data from IEX.  Only used to 
						print report shown on the website 'Market Moving 
						Averages' story.
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
#include	"getstats.h"

int main ( int argc, char *argv[] )
{
	time_t		stime, etime;
	struct tm	*tm;
	char		tbuffer[30];
	char		Fragment[128];
	int			DoWrapUp = 1;

	getargs ( argc, argv );

	GetInvestCfg ( 0 );

	time ( &stime );
	tm = localtime ( &stime );
	if ( Quiet == 0 )
	{
		printf ( "getstats: Start %s", asctime(tm) );
	}

	StartMySQL ( &MySql, "invest" );

	switch ( RunMode )
	{
		case MODE_INDEXED:
			sprintf ( WhereClause, "Slast >= '%s' and (Sdj = 'Y' or Ssp500 = 'Y' or Snasdaq = 'Y' or Srussell = '1' or Srussell = '2')", Yesterday );
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 1 );
			break;
		case MODE_ALL:
			sprintf ( WhereClause, "Stype != '%c'", STYPE_CRYPTO );
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 1 );
			break;
		case MODE_INDEX:
			SetWhereClause ( Fragment, StockIndex );
			sprintf ( WhereClause, "%s and Slast >= '%s'", Fragment, Yesterday );
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 1 );
			break;
		case MODE_ONE:
			sprintf ( WhereClause, "Sticker = '%s'", xstock.xsticker );
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 1 );
			break;
		case MODE_REPORT:
			Report ();
			DoWrapUp = 0;
			break;
	}

	if ( DoWrapUp )
	{
		if ( Quiet == 0 )
		{
			time ( &etime );
			tm = localtime ( &etime );
			asctime_r ( tm, tbuffer );
			TrimRightAndLeft ( tbuffer );
			printf ( "getstats: End %s, Total %ld seconds, Get Data %d, Put Data %d, Updated %d\n",
					tbuffer, etime - stime, SourceTime, DestinationTime, UpdateCount );
		}

		if ( Debug || RunMode == MODE_ONE )
		{
			printf ( "getstats: not removing %s\n", TempFileName );
		}
		else
		{
			unlink ( TempFileName );
		}
	}

	fflush ( stdout );

	return ( 0 );
}
