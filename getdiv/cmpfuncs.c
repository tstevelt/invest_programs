/*----------------------------------------------------------------------------
	Program : getdiv/cmpfuncs.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: Comparison functions used by qsort and bsearch.
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


int cmpstock ( STOCK_RECORD *a, STOCK_RECORD *b )
{
	return ( nsStrcmp ( a->xsticker, b->xsticker ) );
}

int cmpdividend ( XDIVIDEND *a, XDIVIDEND *b )
{
	int		rv;

	rv = nsStrcmp ( a->xdexdate, b->xdexdate );
	if ( rv < 0 )
	{
		return ( -1 );
	}
	if ( rv > 0 )
	{
		return ( 1 );
	}

	return ( 0 );
}
