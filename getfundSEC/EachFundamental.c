/*----------------------------------------------------------------------------
	Program : getfundSEC/EachFundamental.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: For each fundmental, print report line.  Called by Report Old.
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

#include	"getfundSEC.h"

extern	int TotalOld;

int EachFundamental ()
{
	static	int		firstpass = 1;
	double	DaysOld;
	char	DeleteStatement[128];
	char	StockWhere[128];

	sprintf ( StockWhere, "Sticker = '%s'", xfundamental.xfticker );
	LoadStock ( &MySql, StockWhere, &xstock, 0, 0 );

#ifdef REPORT_ONLY_HAVE_SEC_CIK
#define REPORT_ONLY_HAVE_SEC_CIK
	long	TestCIK;
	if (( TestCIK = nsAtol(xstock.xscik) ) == 0 )
	{
		return ( 0 );
	}
#endif

	if ( firstpass )
	{
		firstpass = 0;
		printf ( "OLD FUNDAMENTALS WHICH HAVE SEC CIK NUMBERS\n" );
		printf ( "TOTAL %d OLD FUNDAMENTALS REGARDLESS OF CIK\n", TotalOld );
		printf ( "TICKER|CIK|DOMESTIC|TYPE|TYPE2|DJ|SP500|RUSSELL|NASDAQ|AGE|NAME\n" );
	}

	DaysOld = (StartTime - xfundamental.xfupdated ) / 86400.0;

	printf ( "%-10.10s|%10.10s|%c|%c|%c|%c|%c|%c|%c|%5.1f|%s\n", 
		xfundamental.xfticker, 
		xstock.xscik,
		xstock.xsdomfor[0],
		xstock.xstype[0],
		xstock.xstype2[0],
		xstock.xsdj[0],
		xstock.xssp500[0],
		xstock.xsrussell[0],
		xstock.xsnasdaq[0],
		DaysOld,
		xstock.xsname );

	if ( DeleteOld )
	{
		sprintf ( DeleteStatement, "delete from fundamental where Fticker = '%s'", xfundamental.xfticker );
		if ( dbyUpdate ( "getfund", &MySql, DeleteStatement, 0, LogFileName ) != 1 )
		{
			printf ( "Cannot delete fundamental record for %s\n", xfundamental.xfticker );
			printf ( "%s\n", DeleteStatement );
		}
		else
		{
			UpdateCount++;
		}
	}

	return ( 0 );
}
