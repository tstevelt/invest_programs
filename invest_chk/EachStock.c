/*----------------------------------------------------------------------------
	Program : invest_chk/EachStock.c
	Author  : Tom Stevelt
	Date    : 2025
	Synopsis: For each stock, download data from IEX (or Tiingo) and save.
----------------------------------------------------------------------------*/
//     Programs called by invest.cgi
// 
//     Copyright (C)  2025 Tom Stevelt
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

#include	"invest_chk.h"

int EachStock ()
{
	StockCount++;

	switch ( xstock.xstype[0] )
	{
		case STYPE_BOND:
		case STYPE_ETF:
		case STYPE_PREFER:
		case STYPE_STOCK:
		case STYPE_ADR:
		case STYPE_REIT:
		case STYPE_CRYPTO:
		case STYPE_INDEX:
		case STYPE_FX:
		case STYPE_OTHER:
			break;
		default:
			printf ( "%10.10s %-40.40s: BAD STYPE %c\n", xstock.xsticker, xstock.xsname, xstock.xstype[0] );
			ErrorCount++;
			break;
			
	}

	switch ( xstock.xstype2[0] )
	{
		case STYPE2_LARGE_CAP:
		case STYPE2_MID_CAP:
		case STYPE2_SMALL_CAP:
		case STYPE2_DEVELOPED:
		case STYPE2_EMERGING:
		case STYPE2_OTHER:
		case STYPE2_HUNDREDTH:
		case STYPE2_BASIS:
			break;
		default:
			printf ( "%10.10s %-40.40s: BAD STYPE2 %c\n", xstock.xsticker, xstock.xsname, xstock.xstype2[0] );
			ErrorCount++;
			break;
	}

	switch ( xstock.xssp500[0] )
	{
		case 'Y':
		case 'N':
			break;
		default:
			printf ( "%10.10s %-40.40s: BAD STYPE2 %c\n", xstock.xsticker, xstock.xsname, xstock.xstype2[0] );
			break;
	}

	switch ( xstock.xsrussell[0] )
	{
		case '1':
		case '2':
		case 'N':
			break;
		default:
			printf ( "%10.10s %-40.40s: BAD RUSSELL %c\n", xstock.xsticker, xstock.xsname, xstock.xsrussell[0] );
			break;
	}

	switch ( xstock.xsdj[0] )
	{
		case 'Y':
		case 'N':
			break;
		default:
			printf ( "%10.10s %-40.40s: BAD DOW JONES %c\n", xstock.xsticker, xstock.xsname, xstock.xsdj[0] );
			break;
	}

	switch ( xstock.xsnasdaq[0] )
	{
		case 'Y':
		case 'N':
			break;
		default:
			printf ( "%10.10s %-40.40s: BAD NASDAQ %c\n", xstock.xsticker, xstock.xsname, xstock.xsnasdaq[0] );
			break;
	}

	switch ( xstock.xsdomfor[0] )
	{
		case 'D':
		case 'F':
			break;
		default:
			printf ( "%10.10s %-40.40s: BAD DOMESTIC/FOREIGN %c\n", xstock.xsticker, xstock.xsname, xstock.xsdomfor[0] );
			break;
	}

	return ( 0 );
}
