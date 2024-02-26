/*----------------------------------------------------------------------------
	Program : getstats/Report.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Print report of benchmarks averages.
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

#include	"getstats.h"

typedef struct
{
	char	*Label;
	char	*WhereClause;
} RECORD;

static RECORD Array [] =
{
	{ "DOW JONES", "Sdj = 'Y'" },
	{ "SP 500", "Ssp500 = 'Y'" },
	{ "NASDAQ", "Snasdaq = 'Y'" },
	{ "RUSSELL 2000", "Srussell = '2'" },
};

static int Count = sizeof(Array) / sizeof(RECORD);

static double TotalShares;
static double TotalEarnings;
static double TotalPrices;
static double TotalYield;

static int AddNumbers ()
{
	char	MyWhere[48];

	StockCount++;

	/*----------------------------------------------------------
		load fundamentals
	----------------------------------------------------------*/
	sprintf ( MyWhere, "Fticker = '%s'", xstock.xsticker );
	if ( LoadFundamental ( &MySql, MyWhere, &xfundamental, 0 ) != 1 )
	{
		memset ( &xfundamental, '\0', sizeof(xfundamental) );
	}

	if ( SkipNegatives == 1 && xfundamental.xfepsttm < 0.01 )
	{
		return ( 0 );
	}
	TotalShares   +=  xfundamental.xfshares;
	TotalEarnings += (xfundamental.xfshares * xfundamental.xfepsttm);
	TotalPrices   += (xfundamental.xfshares * xstock.xsclose);
	TotalYield    += (xfundamental.xfshares * xfundamental.xfyield);
	return ( 0 );
}

void Report ()
{
	int		ndx;

	printf ( "<table class='table-condensed table-borderless'>\n" );

	printf ( "<tr><td>INDEX</td>\
		<td align='right'>STOCKS</td>\
		<td align='right'>AVG PRICE</td>\
		<td align='right'>AVG EPS</td>\
		<td align='right'>AVG P/E</td>\
		<td align='right'>AVG YLD</td>\
		</tr>\n" );

	for ( ndx = 0; ndx < Count; ndx++ )
	{
		TotalShares = TotalEarnings = TotalPrices = TotalYield = 0.0;
		StockCount = 0;
		LoadStockCB ( &MySql, Array[ndx].WhereClause, NULL, &xstock, (int(*)()) AddNumbers, 0 );

		printf ( "<tr><td>%-20.20s</td>\
		<td align='right'>%4d</td>\
		<td align='right'>%6.2f</td>\
		<td align='right'>%6.2f</td>\
		<td align='right'>%6.2f</td>\
		<td align='right'>%6.2f</td>\
		</tr>\n", 
				Array[ndx].Label, 
				StockCount, 
				TotalPrices / TotalShares,
				TotalEarnings / TotalShares, 
				TotalPrices / TotalEarnings,
				TotalYield / TotalShares );
	}

	if ( SkipNegatives )
	{
		printf ( "<tr><td colspan='5'>AVG EPS and AVG P/E ignore negative earnings.</td></tr>\n" );
	}

	printf ( "</table>\n" );
}
