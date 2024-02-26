/*----------------------------------------------------------------------------
	Program : getfundSEC/EachStock.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Call python script to get fundamental information from SEC Edgar.
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

int EachStock ( XSTOCK *ptr )
{
	if ( xstock.xscik[0] != '0' )
	{
		if ( StockIndex == 'x' )
		{
			fprintf ( stderr, "%s does not have a CIK number\n", ptr->xsticker );
		}
		ErrorCount++;
		return ( 0 );
	}

	if ( Verbose )
	{
		printf ( "EachStock: %s %s %.2f\n", ptr->xsticker, ptr->xscik, ptr->xsclose );
		fflush ( stdout );
	}

	if ( Ignore == 0 )
	{
		sprintf ( WhereClause, "Fticker = '%s'", ptr->xsticker );
		if (LoadFundamental ( &MySql, WhereClause, &xfundamental,  0 ) == 0 )
		{
			fprintf ( stderr, "%s fundamental record\n", ptr->xsticker );
			ErrorCount++;
			return ( 0 );
		}

		if ( nsStrncmp ( xfundamental.xfframe, "CY", 2 ) != 0 )
		{
			strcpy ( xfundamental.xfframe, "CY197202" );
		}
	}
	else
	{
		strcpy ( xfundamental.xfframe, "CY197001" );
	}

	nap ( 100 );
	NapTime += 100;

	sprintf ( cmdline, "%s %s %.2f %s %s %ld >> %s", 
		Python_Script, ptr->xsticker, ptr->xsclose, ptr->xscik, xfundamental.xfframe, StartTime,  SQL_Script );

	if ( Debug )
	{
		printf ( "%s\n", cmdline );
	}

	system ( cmdline );

	StockCount++;

#ifdef TESTING
if ( StockCount == 10 )
{
	printf ( "Quitting after %d\n", StockCount );
	exit ( 0 );
}
#endif

	return ( 0 );
}
