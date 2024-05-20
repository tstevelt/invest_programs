/*----------------------------------------------------------------------------
	Program : cleandata/do_stock.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Deletes stocks and history where Slast < date (if not in portfolio)
	Return  : 
----------------------------------------------------------------------------*/
//     Invest extras
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

#include	"cleandata.h"

typedef struct
{
	char	*Table;
	char	*Ticker;
	int		Deleted;
} RECORD;

static	RECORD	Array [] =
{
	{ "average", "Aticker", 0 },
	{ "dividend", "Dticker", 0 },
	{ "history", "Hticker", 0 },
	{ "temp", "Tticker", 0 },
	{ "watchlist", "Wticker", 0 },
	{ "crypto", "Cticker", 0 },
	{ "fundamental", "Fticker", 0 },
	{ "stock", "Sticker", 0 },
};

static	int		Count = sizeof(Array) / sizeof(RECORD);

static int EachStock ()
{
	long	RecordCount;
	long	Affected = 0;
	int		ndx;

	if ( xstock.xsticker[0] == '_' )
	{
		return ( 0 );
	}
	switch ( xstock.xstype[0] )
	{
		case 'B':
		case 'C':
			return ( 0 );
		default:
			break;
	}

	printf ( "%-10.10s %-30.30s %s %s %s %s %s", 
			xstock.xsticker, xstock.xsname, xstock.xslast, xstock.xsdj, xstock.xssp500, xstock.xsnasdaq, xstock.xsrussell );

	sprintf ( Statement, "Pticker = '%s'", xstock.xsticker );
	RecordCount = dbySelectCount ( &MySql, "portfolio", Statement, LogFileName );

	if ( RecordCount > 0 )
	{
		printf ( " in portfolio, skipping delete\n" );
		return ( 0 );
	}
	else
	{
		printf ( "\n" );
	}

	for ( ndx = 0; ndx < Count; ndx++ )
	{
		sprintf ( Statement, "delete from %s where %s = '%s'", Array[ndx].Table, Array[ndx].Ticker, xstock.xsticker );
		if ( Debug )
		{
			printf ( "%s\n", Statement );
		}
		else
		{
			Affected = dbyDelete ( "cleandata", &MySql, Statement, 0, LogFileName );
			printf ( "  Deleted %ld %s records\n", Affected, Array[ndx].Table );
			Array[ndx].Deleted += Affected;
		}
	}

	return ( 0 );
}

void do_stock ()
{
	int		ndx;

	/*----------------------------------------------------------
		Sticker not like '\_%' works in mysql, but not here!
	xxx sprintf ( WhereClause, "Sticker not like '\_%' and (Slast < '%s' or Slast is null)", SelectDate );
	----------------------------------------------------------*/
	sprintf ( WhereClause, "Slast < '%s' or Slast is null", SelectDate );
	LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 1 );

	for ( ndx = 0; ndx < Count; ndx++ )
	{
		printf ( "Deleted %d %s records\n", Array[ndx].Deleted, Array[ndx].Table );
	}
}
