/*----------------------------------------------------------------------------
	Program : portdtl/dorpt.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: print information for one portfolio holding in HTML format
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

#include	"portdtl.h"

static	int		DividendCount;
static	double	TotalDividend;
static	int		CheckFirstDate = 0;

int EachDividend ()
{
	DATEVAL	dvPurch, dvFirst;
	long	FirstDays;

	if ( DividendCount == 0 )
	{
		printf ( "<tr>\n" );
		printf ( "<td>Dividend Date</td>\n" );
		printf ( "<td>Amount</td>\n" );
		printf ( "<td>Total</td>\n" );
		printf ( "</tr>\n" );

		StrToDatevalFmt ( xportfolio.xpdate,  DATEFMT_YYYY_MM_DD, &dvPurch );
		StrToDatevalFmt ( xdividend.xdexdate, DATEFMT_YYYY_MM_DD, &dvFirst );
		FirstDays = DateDiff ( &dvPurch, &dvFirst );

		if ( FirstDays > 90 )
		{
			CheckFirstDate = -1;
		}
		else
		{
			CheckFirstDate = 1;
		}
	}

	printf ( "<tr>\n" );
	if ( CheckFirstDate == -1 )
	{
		printf ( "<td><span style='background: yellow;'>%s</span></td>\n", xdividend.xdexdate );
		CheckFirstDate = 1;
	}
	else
	{
		printf ( "<td>%s</td>\n", xdividend.xdexdate );
	}
	printf ( "<td>%.2f</td>\n", xdividend.xdamount );
	printf ( "<td>%.2f</td>\n", xportfolio.xpshares * xdividend.xdamount );
	printf ( "</tr>\n" );

	DividendCount++;
	TotalDividend += (xportfolio.xpshares * xdividend.xdamount);

	return ( 0 );
}

void dorpt ()
{
	double	Invested, Market, Profit, Percent;
	char	Today[11];
	DATEVAL	dvPurch, dvToday;
	long	HeldDays;

	sprintf ( Today, "%s", fmtGetTodayDate ( DATEFMT_YYYY_MM_DD ) );
	StrToDatevalFmt ( Today, DATEFMT_YYYY_MM_DD, &dvToday );
	StrToDatevalFmt ( xportfolio.xpdate, DATEFMT_YYYY_MM_DD, &dvPurch );
	
	HeldDays = DateDiff ( &dvPurch, &dvToday );

	printf  ( "<table width='95%%'>\n" );

	/*----------------------------------------------------------
		row - ticker and company
	----------------------------------------------------------*/
	printf ( "<tr>\n" );
	printf ( "<td>%s</td>\n", xstock.xsticker );
	printf ( "<td colspan='2'>%s</td>\n", xstock.xsname );
	printf ( "</tr>\n" );

	/*----------------------------------------------------------
		row - purch date
	----------------------------------------------------------*/
	printf ( "<tr>\n" );
	printf ( "<td>Purchase Date</td>\n" );
	printf ( "<td>%s</td>\n", xportfolio.xpdate );
	printf ( "</tr>\n" );

	/*----------------------------------------------------------
		row - shares cost
	----------------------------------------------------------*/
	printf ( "<tr>\n" );
	printf ( "<td>Shares</td>\n" );
	printf ( "<td>%.2f</td>\n", xportfolio.xpshares );
	printf ( "</tr>\n" );

	/*----------------------------------------------------------
		row - cost
	----------------------------------------------------------*/
	printf ( "<tr>\n" );
	printf ( "<td>Cost</td>\n" );
	printf ( "<td>%.2f</td>\n", xportfolio.xpprice );
	printf ( "</tr>\n" );

	/*----------------------------------------------------------
		row - close
	----------------------------------------------------------*/
	printf ( "<tr>\n" );
	printf ( "<td>Market</td>\n" );
	printf ( "<td>%.2f</td>\n", xhistory.xhclose );
	printf ( "</tr>\n" );

	/*----------------------------------------------------------
		4 rows - calc and display basic ROI
	----------------------------------------------------------*/
	Invested = xportfolio.xpshares * xportfolio.xpprice;
	Market = xportfolio.xpshares * xhistory.xhclose;
	Profit = Market - Invested;
	Percent = 100.0 * Profit / Invested;

	printf ( "<tr>\n" );
	printf ( "<td>Invested</td>\n" );
	printf ( "<td>%.2f</td>\n", Invested );
	printf ( "</tr>\n" );

	printf ( "<tr>\n" );
	printf ( "<td>Current Value</td>\n" );
	printf ( "<td>%.2f</td>\n", Market );
	printf ( "</tr>\n" );

	printf ( "<tr>\n" );
	printf ( "<td>Profit</td>\n" );
	printf ( "<td>%.2f</td>\n", Profit );
	printf ( "</tr>\n" );

	printf ( "<tr>\n" );
	printf ( "<td>Base ROI</td>\n" );
	printf ( "<td>%.2f</td>\n", Percent );
	printf ( "</tr>\n" );

	/*----------------------------------------------------------
		many rows - dividends since purch date
	----------------------------------------------------------*/
	DividendCount = 0;
	TotalDividend = 0.0;
	sprintf ( WhereClause, "Dticker = '%s' and Dexdate >= '%s' and Dexdate <= now()", xstock.xsticker, xportfolio.xpdate );
	if ( LoadDividendCB ( &MySql, WhereClause, "Dexdate", &xdividend, (int(*)()) EachDividend, 0 ) < 1 )
	{
		printf ( "<tr>\n" );
		printf ( "<td>Dividends</td>\n" );
		printf ( "<td>None since Purchase Date</td>\n" );
		printf ( "</tr>\n" );
	}
	else
	{
		Profit += TotalDividend;
		Percent = 100.0 * Profit / Invested;

		printf ( "<tr>\n" );
		printf ( "<td>Total Dividends</td>\n" );
		printf ( "<td>%.2f</td>\n", TotalDividend );
		printf ( "</tr>\n" );

		printf ( "<tr>\n" );
		printf ( "<td>Profit+Dividends</td>\n" );
		printf ( "<td>%.2f</td>\n", Profit );
		printf ( "</tr>\n" );

		printf ( "<tr>\n" );
		printf ( "<td>Total ROI</td>\n" );
		printf ( "<td>%.2f</td>\n", Percent );
		printf ( "</tr>\n" );

	}

	/*----------------------------------------------------------
		row - annual yield
	----------------------------------------------------------*/
	if ( HeldDays > 365 )
	{
		Percent = Percent * 365.0 / (double) HeldDays;

		printf ( "<tr>\n" );
		printf ( "<td>Annual ROI</td>\n" );
		printf ( "<td>%.2f</td>\n", Percent );
		printf ( "<td>Held %ld days</td>\n", HeldDays );
		printf ( "</tr>\n" );
	}

#ifdef MORE
	/*----------------------------------------------------------
		row - xxx
	----------------------------------------------------------*/
	printf ( "<tr>\n" );
	printf ( "<td>label</td>\n" );
	printf ( "<td>%s</td>\n", xxx );
	printf ( "</tr>\n" );

#endif
	printf  ( "</table>\n" );
}
