/*----------------------------------------------------------------------------
	Program : portdtl.c
	Author  : Tom Stevelt
	Date    : 04/25/2019
	Synopsis: Portfolio Detail Sheet for one stock in a portfolio,  showing
			  EVERYTHING that impacts the return.
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------

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
#include	"portdtl.h"

int main ( int argc, char *argv[] )
{
	long		rv;

	getargs ( argc, argv );

	StartMySQL ( &MySql, "invest" );

	switch ( RunMode )
	{
		case TABLE_portfolio:
			sprintf ( WhereClause, "portfolio.id = %ld", xportfolio.xid );
			if (( rv = LoadPortfolio ( &MySql, WhereClause, &xportfolio, 0 )) < 1 )
			{
				printf ( "Unknown portfolio id %ld\n", xportfolio.xid );
				exit ( 1 );
			}
			break;
		case TABLE_member:
			sprintf ( WhereClause, "portfolio.Pmember = %ld and Pticker = '%s'", xmember.xid, xstock.xsticker );
			rv = LoadPortfolio ( &MySql, WhereClause, &xportfolio, 0 );
			if ( rv > 1 )
			{
				printf ( "Member %ld has %ld of %s in portfolio\n",  xmember.xid, rv, xstock.xsticker );
				exit ( 1 );
			}
			else if ( rv < 1 )
			{
				printf ( "Member %ld does not have %s in portfolio\n",  xmember.xid, xstock.xsticker );
				exit ( 1 );
			}
			break;
	}

	sprintf ( WhereClause, "member.id = %ld",  xportfolio.xpmember );
	if ( LoadMember ( &MySql, WhereClause, &xmember, 0 ) < 1 )
	{
		printf ( "Unknown member %ld\n", xportfolio.xpmember );
		exit ( 1 );
	}
	
	sprintf ( WhereClause, "Sticker = '%s'",  xportfolio.xpticker );
	if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) < 1 )
	{
		printf ( "Unknown stock %s\n", xportfolio.xpticker );
		exit ( 1 );
	}
	
	sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'",  xportfolio.xpticker, xstock.xslast );
	if ( LoadHistory ( &MySql, WhereClause, &xhistory, 0 ) < 1 )
	{
		xhistory.xhclose = 0.0;
	}

	dorpt ();

	return ( 0 );
}
