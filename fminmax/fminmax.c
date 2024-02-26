/*----------------------------------------------------------------------------
	Program : fminmax/fminmax.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: The ETF and Equity fundamental screeners can request 
				various minimum and maximum values for selelcted fields.

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
#include	"fminmax.h"

typedef struct
{
	char	*Name;
	int		DataType;
} RECORD;

static	RECORD	Array[] = 
{
  { "Fticker"     , DATATYPE_STRING },
  { "Fcountry"    , DATATYPE_STRING },
  { "Ftype"       , DATATYPE_STRING },
  { "Fframe"      , DATATYPE_STRING },
  { "Finception"  , DATATYPE_MYSQL_DATE },
  { "Ffactset"    , DATATYPE_CHAR },
  { "Febitda"     , DATATYPE_LONG },
  { "Fentval"     , DATATYPE_LONG },
  { "Fshares"     , DATATYPE_LONG },
  { "Ffreecash"   , DATATYPE_LONG },
  { "Fupdated"    , DATATYPE_LONG },
  { "Ftotasst"    , DATATYPE_LONG },
  { "Fmktcap"     , DATATYPE_LONG },
  { "Fvolume"     , DATATYPE_LONG },
  { "Fmstar"      , DATATYPE_CHAR },
};

static	int	Count = sizeof(Array) / sizeof(RECORD);

int main ( int argc, char *argv[] )
{
	WEBPARMS	*ptrWebParms = webInitParms();
	int			DataType;
	double		Minimum, Maximum, Average;
	int			rv;
	char		*Title;
	char		Fragment[128];

	webContentHtml ();

	ptrWebParms->WP_Title = "Fundamental Minimum and Maximum";
	webStartHead ( ptrWebParms );

	StartMySQL ( &MySql, "invest" );

	webHeadToBody ( ptrWebParms );

	webFreeParms ( ptrWebParms );

#ifdef DEBUG
	printf ( "Hello world<br>\n" );
#endif

	GetInput ();

	switch ( Source )
	{
		case '3':
			sprintf ( WhereClause, "Fticker=Sticker and (Srussell='1' or Srussell='2'" );
			Title = "Russell 3000";
			break;
		case 'E':
			sprintf ( WhereClause, "Fticker=Sticker and Stype='E'" );
			Title = "ETF";
			break;
		case 'S':
		default:
			sprintf ( WhereClause, "Fticker=Sticker and Ssp500='Y'" );
			Title = "S&P 500";
			break;
	}

	DataType = DATATYPE_DOUBLE;
	for ( int ndx = 0; ndx < Count; ndx++ )
	{
		if ( strcmp ( FieldName, Array[ndx].Name ) == 0 )
		{
			DataType = Array[ndx].DataType;
			break;
		}
	}

	switch ( DataType )
	{
		case DATATYPE_MYSQL_DATE:
		case DATATYPE_STRING:
			break;
		case DATATYPE_CHAR:
			sprintf ( Statement, "select %s, count(*) from stock,fundamental where %s and %s > ' ' group by %s", 
								FieldName, WhereClause, FieldName, FieldName );
			break;
		case DATATYPE_SHORT:
		case DATATYPE_LONG:
		case DATATYPE_DOUBLE:
			sprintf ( Fragment, " and %s != 0", FieldName );
			strcat ( WhereClause, Fragment );
			if (( rv = dbySelectMinMaxAvg ( &MySql, "fundamental,stock", FieldName, WhereClause, "", &Minimum, &Maximum, &Average )) != 0 )
			{
				printf ( "Could not find values for %s<br>\n", FundamentalHuman(FieldName) );
				webEndPage ();
				return ( 0 );
			}
			break;
	}

	int EachRecord ( DBY_QUERY *Query )
	{
		printf ( "<tr><td align='center'>%s</td><td align='center'>%s</td></tr>\n", Query->EachRow[0], Query->EachRow[1] );
		return ( 0 );
	}

	switch ( DataType )
	{
		case DATATYPE_CHAR:
			printf ( "<h3>%s %s</h3>\n", Title, FundamentalHuman(FieldName) );
			printf ( "<table border='0' cellpadding='2' cellspacing='2' width='100%%'>\n" );
			// long dbySelectCB ( char *Module, MYSQL *MySql, char *SelectStatement, int (*CallBack)(), char *LogFileName )
			dbySelectCB ( "invest", &MySql, Statement, (int(*)()) EachRecord, LogFileName );
			printf ( "</table>\n" );
			break;
		case DATATYPE_MYSQL_DATE:
		case DATATYPE_STRING:
			break;
		case DATATYPE_SHORT:
		case DATATYPE_LONG:
			printf ( "<h3>%s %s</h3><hr>Minimum&emsp;%.0f<br>Maximum&emsp;%.0f<br>Average&emsp;%.0f\n", 
						Title, FundamentalHuman(FieldName), Minimum, Maximum, Average );
			break;
		case DATATYPE_DOUBLE:
			printf ( "<h3>%s %s</h3><hr>Minimum&emsp;%.2f<br>Maximum&emsp;%.2f<br>Average&emsp;%.2f\n", 
						Title, FundamentalHuman(FieldName), Minimum, Maximum, Average );
			break;
	}

	webEndPage ();

	return ( 0 );
}
