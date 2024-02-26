/*----------------------------------------------------------------------------
	Program : getdiv.c
	Author  : Tom Stevelt
	Date    : 01/24/2019
	Synopsis: Get past twelve month of dividends paid [to user], store in
			  portfolio.Panndiv
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		04/23/2019	Renamed from upddiv to getdiv
						Get dividend data from IEX Cloud
						Only for stocks in portfolio where member.Mincdiv = 'Y'
						Only for dates greater than or equal to earliest portfolio.Pdate
						or 5 years, whichever is later.
						-one gets for a stock regardless of above.
	tms		04/26/2019	added -update option
	tms		07/13/2021	Get private api keys with GetInvestCfg().
	tms		01/23/2023	Ignore member.Mincdiv
	tms		06/29/2023	Check for dividend cuts.

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
#include	"getdiv.h"

int main ( int argc, char *argv[] )
{
	getargs ( argc, argv );

	GetInvestCfg ( 0 );

	StartMySQL ( &MySql, "invest" );

	switch ( RunMode )
	{
		case MODE_ACTIVE:
		case MODE_CUTS:
		case MODE_MEMBER:
			switch ( RunMode )
			{
				case MODE_ACTIVE:
				case MODE_CUTS:
					sprintf ( WhereClause, "id > 0" );
					LoadMemberCB ( &MySql, WhereClause, NULL, &xmember, (int(*)()) EachMember, 1 );
					break;

				case MODE_MEMBER:
					sprintf ( WhereClause, "Pmember = %ld", xmember.xid );
					if ( LoadPortfolioCB ( &MySql, WhereClause, NULL, &xportfolio, (int(*)()) EachPortfolio, 0 ) < 1 )
					{
						printf ( "Member does not have any stocks in portfolio\n" );
					}
					break;
			}

			int		ndx;
			for ( ndx = 0; ndx < StockCount; ndx++ )
			{
				if ( Debug )
				{
					printf ( "%s\n", StockArray[ndx].xsticker );
				}

				sprintf ( WhereClause, "Sticker = '%s'", StockArray[ndx].xsticker );
				if ( LoadStock ( &MySql, WhereClause, &xstock, 1, 0 ) != 1 )
				{
					printf ( "Cannot load stock for %s\n", StockArray[ndx].xsticker );
				}
				else if ( RunMode == MODE_CUTS )
				{
					DividendCuts ();
				}
				else
				{
					EachStock ();
				}
			}

			if ( TotalUpdated > 0 )
			{
				sprintf ( Statement, "update system set Ydivcnt = %ld, Ydivtime = '%s' where system.id = 1",
					(long) TotalUpdated, fmtGetTodayDate(DATEFMT_TIMESTAMP) );

				dbyUpdate ( "getdiv", &MySql, Statement, 0, LogFileName );
			}

			break;

		case MODE_UPDATE:
			sprintf ( WhereClause, "(select count(*) from dividend where Dticker = Sticker) > 0" );
			StockCount = LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 0 );

			if ( TotalUpdated > 0 )
			{
				sprintf ( Statement, "update system set Ydivcnt = %ld, Ydivtime = '%s' where system.id = 1",
					(long) TotalUpdated, fmtGetTodayDate(DATEFMT_TIMESTAMP) );

				dbyUpdate ( "getdiv", &MySql, Statement, 0, LogFileName );
			}

			break;

		case MODE_ONE:
			sprintf ( WhereClause, "Sticker = '%s'", OneTicker );
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 1 );
			break;
	
	}

	if ( RunMode == MODE_CUTS )
	{
		printf ( "Checked %d stocks, found %d cuts\n", StockCount, TotalCuts );
	}
	else
	{
		printf ( "Checked %d stocks, Inserted %d dividend records\n", StockCount, TotalUpdated );
	}
	
	return ( 0 );
}
