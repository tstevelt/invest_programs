/*----------------------------------------------------------------------------
	Program : getdiv/DividendCuts.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Print report of dividend cuts.
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

#include	"getdiv.h"

int EachCut ()
{
	memcpy ( &DividendArray[DividendCount++], &xdividend, sizeof(XDIVIDEND) );

	if ( Debug )
	{
		printf ( "%3d %s %.4f\n", DividendCount, xdividend.xdexdate, xdividend.xdamount );
	}

	if ( DividendCount == 2 )
	{
		return ( -1 );
	}
	return ( 0 );
}

void DividendCuts ()
{
	double	Diff, Percent;

	DividendCount = 0;
	sprintf ( WhereClause, "Dticker = '%s'", xstock.xsticker );
	LoadDividendCB ( &MySql, WhereClause, "Dexdate desc", &xdividend, (int(*)()) EachCut, 0 );


	if ( DividendCount < 2 )
	{
		return;
	}

	if ( DividendArray[0].xdamount < DividendArray[1].xdamount )
	{
		Diff = DividendArray[1].xdamount - DividendArray[0].xdamount;
		Percent = 100.0 * Diff / DividendArray[1].xdamount;

		printf ( "%-6.6s %-20.20s %10.10s DIV CUT FROM %.4f TO %.4f, DIFF %.4f, PCT %.2f%%\n",
			xstock.xsticker,
			xstock.xsname,
			DividendArray[0].xdexdate,
			DividendArray[1].xdamount,
			DividendArray[0].xdamount,
			Diff,
			Percent );

		TotalCuts++;

	}
}
