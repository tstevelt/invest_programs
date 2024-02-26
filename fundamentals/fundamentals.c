/*----------------------------------------------------------------------------
	Program : fundamentals.c
	Author  : Silver Hammer Software LLC
	Author  : Tom Stevelt
	Date    : Aug 2022
	Synopsis: display fundamentals for one stock
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------
	tms	 	09/06/2022	Added ETF, switch on stock.Stype
	tms		11/06/2023	Added Fdividend to table, fixed Fyield
	tms		11/06/2023	Calc Fpettm
	tms		11/06/2023	Added Fbvps to table, fixed Fpb
	tms		11/07/2023	Added Fmktcap until get Fentval and Febitda
	tms		11/16/2023	Highlight UPDATED if more than three months old.

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
#include	"fundamentals.h"

typedef struct
{
	int		isETF;
	char	*Field;
	int		Type;
	void	*Data;
	short	Compare;
} RECORD;

#define		DATA_TYPE_LONG		1
#define		DATA_TYPE_INT		2
#define		DATA_TYPE_SHORT		3
#define		DATA_TYPE_CONDENSE	4
#define		DATA_TYPE_DOUBLE	5
#define		DATA_TYPE_DOUBLE_4	6
#define		DATA_TYPE_SYSTIME	7
#define		DATA_TYPE_STRING	8

#define		LOWER_IS_BETTER		-1
#define		HIGHER_IS_BETTER	1
#define		HIGHLIGHT_OLD		9

#undef  GRADE_FIVE
#define GRADE_THREE

static char *GetClass ( const short Compare, const double Percent)
{
	if ( Compare == LOWER_IS_BETTER )
	{
#ifdef GRADE_FIVE
		if ( Percent < -30.0 )
		{
			return ( "grade_A" );
		}
		else if ( Percent < -10.0 )
		{
			return ( "grade_B" );
		}
		else if ( Percent > 10.0 && Percent < 30.0 )
		{
			return ( "grade_D" );
		}
		else if ( Percent > 30.0 )
		{
			return ( "grade_F" );
		}
#endif
#ifdef GRADE_THREE
		if ( Percent < -40.0 )
		{
			return ( "grade_buy" );
		}
		else if ( Percent > 40.0 )
		{
			return ( "grade_sell" );
		}
		else
		{
			return ( "grade_hold" );
		}
#endif
	}
	else if ( Compare == HIGHER_IS_BETTER )
	{
#ifdef GRADE_FIVE
		if ( Percent < -30.0 )
		{
			return ( "grade_F" );
		}
		else if ( Percent < -10.0 )
		{
			return ( "grade_D" );
		}
		else if ( Percent > 10.0 && Percent < 30.0 )
		{
			return ( "grade_B" );
		}
		else if ( Percent > 30.0 )
		{
			return ( "grade_A" );
		}
#endif
#ifdef GRADE_THREE
		if ( Percent < -40.0 )
		{
			return ( "grade_sell" );
		}
		else if ( Percent > 40.0 )
		{
			return ( "grade_buy" );
		}
		else
		{
			return ( "grade_hold" );
		}
#endif
	}
	else if ( Compare == HIGHLIGHT_OLD )
	{
		if ( Percent > 4.1 )
		{
			return ( "grade_sell" );
		}
		if ( Percent > 3.0 )
		{
			return ( "grade_hold" );
		}
	}
	return ( "" );
}

#define		HAVE_ENTVAL_AND_EBITDA


static	RECORD	Array [] =
{
//  { 0, "Fticker", DATA_TYPE_STRING, &xfundamental.xfticker },

  { 0, "Fepsttm", DATA_TYPE_DOUBLE,  &xfundamental.xfepsttm, HIGHER_IS_BETTER },
  { 0, "Fpettm", DATA_TYPE_DOUBLE,  &xfundamental.xfpettm, LOWER_IS_BETTER },
#ifdef HAVE_ANALYST
  { 0, "Fepsfwd", DATA_TYPE_DOUBLE,  &xfundamental.xfepsfwd, HIGHER_IS_BETTER },
  { 0, "Fpefwd", DATA_TYPE_DOUBLE,  &xfundamental.xfpefwd, LOWER_IS_BETTER },
#endif
  { 0, "Fpb", DATA_TYPE_DOUBLE,  &xfundamental.xfpb, LOWER_IS_BETTER },
  { 0, "Fyield", DATA_TYPE_DOUBLE,  &xfundamental.xfyield, HIGHER_IS_BETTER },
  { 0, "FreturnA", DATA_TYPE_DOUBLE,  &xfundamental.xfreturna, HIGHER_IS_BETTER },
  { 0, "Fmargin", DATA_TYPE_DOUBLE,  &xfundamental.xfmargin, HIGHER_IS_BETTER },
  { 0, "Fbeta", DATA_TYPE_DOUBLE,  &xfundamental.xfbeta, LOWER_IS_BETTER },
#ifdef HAVE_ANALYST
  { 0, "Fgrow1", DATA_TYPE_DOUBLE,  &xfundamental.xfgrow1, HIGHER_IS_BETTER },
  { 0, "Fgrow5", DATA_TYPE_DOUBLE,  &xfundamental.xfgrow5, HIGHER_IS_BETTER },
  { 0, "Ftarget", DATA_TYPE_DOUBLE,  &xfundamental.xftarget, 0 },
  { 0, "Freco", DATA_TYPE_DOUBLE,  &xfundamental.xfreco, HIGHER_IS_BETTER },
#endif
  { 0, "Fquick", DATA_TYPE_DOUBLE,  &xfundamental.xfquick, LOWER_IS_BETTER },
  { 0, "Fdebteq", DATA_TYPE_DOUBLE,  &xfundamental.xfdebteq, LOWER_IS_BETTER },
  { 0, "Ftotasst", DATA_TYPE_CONDENSE, &xfundamental.xftotasst, 0 },
  { 0, "Frevenuegrow", DATA_TYPE_DOUBLE,  &xfundamental.xfrevenuegrow, 0 },
  { 0, "Fearngrow", DATA_TYPE_DOUBLE,  &xfundamental.xfearngrow, 0 },
  { 0, "Ffreecash", DATA_TYPE_CONDENSE, &xfundamental.xffreecash, 0 },
  { 0, "Ffcfgrow", DATA_TYPE_DOUBLE, &xfundamental.xffcfgrow, 0 },
  { 0, "Ffcfcv", DATA_TYPE_DOUBLE, &xfundamental.xffcfcv, 0 },
  { 0, "Fshares", DATA_TYPE_CONDENSE, &xfundamental.xfshares, 0 },

  { 0, "Fmktcap", DATA_TYPE_CONDENSE, &xfundamental.xfmktcap, 0 },
#ifdef HAVE_ENTVAL_AND_EBITDA
  { 0, "Fentval", DATA_TYPE_CONDENSE, &xfundamental.xfentval, 0 },
  { 0, "Febitda", DATA_TYPE_CONDENSE, &xfundamental.xfebitda, 0 },
  { 0, "Fvalue", DATA_TYPE_DOUBLE,  &xfundamental.xfvalue, LOWER_IS_BETTER },
#endif

  { 0, "Fvolume", DATA_TYPE_CONDENSE, &xfundamental.xfvolume, HIGHER_IS_BETTER },
  { 0, "Fcountry", DATA_TYPE_STRING, xfundamental.xfcountry, 0 },
  { 0, "Fupdated", DATA_TYPE_SYSTIME, &xfundamental.xfupdated, HIGHLIGHT_OLD },


#define ETF_SOURCE
#ifdef ETF_SOURCE
  { 1, "Fyield", DATA_TYPE_DOUBLE,  &xfundamental.xfyield, HIGHER_IS_BETTER },
  { 1, "Fexpratio", DATA_TYPE_DOUBLE_4,  &xfundamental.xfexpratio, LOWER_IS_BETTER },
  { 1, "Favgret3", DATA_TYPE_DOUBLE,  &xfundamental.xfavgret3, HIGHER_IS_BETTER },
  { 1, "Favgret5", DATA_TYPE_DOUBLE,  &xfundamental.xfavgret5, HIGHER_IS_BETTER },
  { 1, "Ftotasst", DATA_TYPE_CONDENSE, &xfundamental.xftotasst, 0 },
  { 1, "Fmktcap", DATA_TYPE_CONDENSE, &xfundamental.xfmktcap, 0 },
  { 1, "Finception", DATA_TYPE_STRING, xfundamental.xfinception, 0 },
  { 1, "Fmstar", DATA_TYPE_STRING,   xfundamental.xfmstar, 0 },
  { 1, "Ffactset", DATA_TYPE_STRING, xfundamental.xffactset, 0 },
  { 1, "Fvolume", DATA_TYPE_CONDENSE, &xfundamental.xfvolume, 0 },
  { 1, "Fupdated", DATA_TYPE_SYSTIME, &xfundamental.xfupdated, HIGHLIGHT_OLD }
#endif


};

static	int		Count = sizeof(Array) / sizeof(RECORD);

int main ( int argc, char *argv[] )
{
	WEBPARMS	*ptrWebParms = webInitParms();
	int			rv, ndx;
	short		shortValue = 0;
	int			intValue = 0;
	long		longValue = 0;
	double		doubleValue = 0.0;
	double		Average = 0.0;
	double		Difference = 0.0;
	double		Percent = 0.0;
	long		CurrentTime;

	webContentHtml ();

	ptrWebParms->WP_Title = "Fundamentals";
	webStartHead ( ptrWebParms );

	StartMySQL ( &MySql, "invest" );

	printf ( "<meta name='viewport' content='width=device-width, initial-scale=1'>\n" );
	printf ( "<link rel='stylesheet' href='my_navbar_colors.css'>\n" );
	printf ( "<link rel='stylesheet' href='invest.css'>\n" );
	printf ( "<link rel='stylesheet' href='fundamentals.css'>\n" );

	// webHeadToBody ( ptrWebParms );
	printf ( "</head>\n<body>\n" );
	// ptrWebParms->WP_OnLoad = "javascript:window.parent.focus();";
	// webHeadToBody ( ptrWebParms );

	webFreeParms ( ptrWebParms );

#ifdef DEBUG
#define DEBUG
	printf ( "Hello world<br>\n" );
#endif

	GetInput ();

	sprintf ( WhereClause, "Sticker = '%s'", Ticker );
	if (( rv = LoadStock ( &MySql,  WhereClause, &xstock, 0, 0 )) != 1 )
	{
		printf ( "Cannot load stock for %s<br>\n", Ticker );
		return ( 0 );
	}
	else
	{
		printf ( "<h3>%s (%s)</h3>\n", xstock.xsname, xstock.xsticker );
		printf ( "<hr>\n" );
	}

	sprintf ( WhereClause, "Fticker = '%s'", Ticker );
	if (( rv = LoadFundamental ( &MySql,  WhereClause, &xfundamental, 0 )) != 1 )
	{
		printf ( "%s does not have fundamentals on file.<br>\n", Ticker );
	}
	else
	{
		printf ( "<table  class='table-condensed'>\n" );

		printf ( "<tr>" );
		printf ( "<td>FIELD</td>" );
		printf ( "<td align='right'>STOCK</td>" );

		if ( xstock.xstype[0] == 'E' )
		{
			sprintf ( WhereClause, "Fticker=Sticker and Stype='E'" );
			printf ( "<td align='right'>ALL ETF</td>" );
		}
		else if ( xstock.xsdj[0] == 'N' && xstock.xssp500[0] == 'N' && xstock.xsrussell[0] == 'N' && xstock.xsnasdaq[0] == 'N' )
		{
			sprintf ( WhereClause, "Fticker=Sticker and Ssp500='Y'" );
			printf ( "<td align='right'>S&P 500</td>" );
		}
		else if ( xstock.xsdj[0] == 'N' && xstock.xssp500[0] == 'N' && xstock.xsrussell[0] == 'N' && xstock.xsnasdaq[0] == 'Y' )
		{
			sprintf ( WhereClause, "Fticker=Sticker and Snasdaq='Y'" );
			printf ( "<td align='right'>NASDAQ</td>" );
		}
		else if ( xstock.xsdj[0] == 'N' && xstock.xssp500[0] == 'N' && xstock.xsrussell[0] == '1' )
		{
			sprintf ( WhereClause, "Fticker=Sticker and Srussell='1'" );
			printf ( "<td align='right'>RUSSELL 1000</td>" );
		}
		else if ( xstock.xsdj[0] == 'N' && xstock.xssp500[0] == 'N' && xstock.xsrussell[0] == '2' )
		{
			sprintf ( WhereClause, "Fticker=Sticker and Srussell='2'" );
			printf ( "<td align='right'>RUSSELL 2000</td>" );
		}
		else
		{
			sprintf ( WhereClause, "Fticker=Sticker and Ssp500='Y'" );
			printf ( "<td align='right'>S&P 500</td>" );
		}

		printf ( "<td align='right'>PERCENT</td>" );

		printf ( "</tr>" );

		// printf ( "<tr><td>%s</td></tr>\n", WhereClause );

		for ( ndx = 0; ndx < Count; ndx++ )
		{
			if ( xstock.xstype[0] != 'E' && Array[ndx].isETF )
			{
				continue;
			}
			else if ( xstock.xstype[0] == 'E' && Array[ndx].isETF == 0 )
			{
				continue;
			}

			if ( Array[ndx].Compare )
			{
				if (( rv = dbySelectAvg ( &MySql, "fundamental,stock", Array[ndx].Field, WhereClause, "", &Average )) != 0 )
				{
					// printf ( "Could not find values for %s<br>\n", FundamentalHuman(Array[ndx].Field) );
					Average = 0.0;
					Difference = 0.0;
					Percent = 0.0;
				}
			}
			else
			{
				Average = 0.0;
				Difference = 0.0;
				Percent = 0.0;
			}

			printf ( "<tr class='%s'>", ndx % 2 ? "report-even" : "report-odd" );
			printf ( "<td>%s</td>", FundamentalHuman(Array[ndx].Field) );

			switch ( Array[ndx].Type  )
			{
				case DATA_TYPE_STRING:
					printf ( "<td align='right'>%s", (char *) Array[ndx].Data );
					break;

				case DATA_TYPE_SHORT:
					memcpy ( &shortValue, Array[ndx].Data, sizeof(short) );
					printf ( "<td align='right'>%d", shortValue );
					if ( Array[ndx].Compare ) printf ( "<td align='right'>%.2f</td>", Average );
					break;

				case DATA_TYPE_INT:
					memcpy ( &intValue, Array[ndx].Data, sizeof(int) );
					printf ( "<td align='right'>%d", intValue );
					if ( Array[ndx].Compare ) printf ( "<td align='right'>%.2f</td>", Average );
					break;

				case DATA_TYPE_LONG:
					memcpy ( &longValue, Array[ndx].Data, sizeof(long) );
					printf ( "<td align='right'>%ld", longValue );
					if ( Array[ndx].Compare ) printf ( "<td align='right'>%.2f</td>", Average );
					break;

				case DATA_TYPE_CONDENSE:
					memcpy ( &longValue, Array[ndx].Data, sizeof(long) );
					if ( longValue <= -1000000000 )
					{
						printf ( "<td align='right'>%.2f B", (double) longValue / 1000000000.0 );
					}
					else if ( longValue > -1000000000 && longValue < -1000000 )
					{
						printf ( "<td align='right'>%.2f M", (double) longValue / 1000000.0 );
					}
					else if ( longValue < 1000000 )
					{
						printf ( "<td align='right'>%ld", longValue );
					}
					else if ( longValue < 1000000000 )
					{
						printf ( "<td align='right'>%.2f M", (double) longValue / 1000000.0 );
					}
					else if ( longValue >= 1000000000 )
					{
						printf ( "<td align='right'>%.2f B", (double) longValue / 1000000000.0 );
					}

					// if ( Array[ndx].Compare ) printf ( "<td align='right'>%.2f</td>", Average );
					if ( Average <= -1000000000 )
					{
						printf ( "<td align='right'>%.2f B", (double) Average / 1000000000.0 );
					}
					else if ( Average > -1000000000 && Average < -1000000 )
					{
						printf ( "<td align='right'>%.2f M", (double) Average / 1000000.0 );
					}
					else if ( Average > 0.0 && Average < 1000000 )
					{
						printf ( "<td align='right'>%.2f K", (double) Average / 1000.0 );
					}
					else if ( Average > 0.0 && Average < 1000000000 )
					{
						printf ( "<td align='right'>%.2f M", (double) Average / 1000000.0 );
					}
					else if ( Average > 0.0 && Average >= 1000000000 )
					{
						printf ( "<td align='right'>%.2f B", (double) Average / 1000000000.0 );
					}

					break;

				case DATA_TYPE_DOUBLE:
				case DATA_TYPE_DOUBLE_4:
					memcpy ( &doubleValue, Array[ndx].Data, sizeof(double) );
					if ( Array[ndx].Type == DATA_TYPE_DOUBLE_4 )
					{
						printf ( "<td align='right'>%.4f", doubleValue );
					}
					else
					{
						printf ( "<td align='right'>%.2f", doubleValue );
					}
					if ( Array[ndx].Compare ) 
					{
						if ( Array[ndx].Type == DATA_TYPE_DOUBLE_4 )
						{
							printf ( "<td align='right'>%.4f</td>", Average );
						}
						else
						{
							printf ( "<td align='right'>%.2f</td>", Average );
						}
						Difference = doubleValue - Average;
						if ( Average < 0.0 ) Average = 0.0 - Average;
						Percent = 100.0 * Difference / Average;
						printf ( "<td class='%s' align='right'>%.1f", GetClass(Array[ndx].Compare,Percent), Percent );
					}
					break;

				case DATA_TYPE_SYSTIME:
					time ( &CurrentTime );
					memcpy ( &longValue, Array[ndx].Data, sizeof(long) );
					Difference = CurrentTime - longValue;
					if ( Difference > 86400 * 30 * 3 )
					{
						Percent = 5.0;
					}
					else if ( Difference > 86400 * 7 )
					{
						Percent = 4.0;
					}
					else
					{
						Percent = 0.0;
					}
					printf ( "<td class='%s' align='left'>%s</td>", GetClass(Array[ndx].Compare,Percent),  DateYYYY_MM_DD(longValue) );
					break;

			}

			printf ( "</tr>\n" );
		}


		printf ( "</table>\n" );
	}

	webEndPage ();

	return ( 0 );
}
