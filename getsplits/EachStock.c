/*----------------------------------------------------------------------------
	Program : getsplits/EachStock.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: For each stock, get splits information from IEX
	Return  : 
----------------------------------------------------------------------------*/
//     Invest extras
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

#include	"getsplits.h"

static int EachHistory ( XHISTORY *xhistory )
{
	return ( -1 );
}

int EachStock ()
{
	char	cmdline[1024];
	char	FileName[256];
	char	*buffer;
	JSON_NAME_VALUE		*ptrNameValue;
static	int		firstpass = 1;
static	DATEVAL	dvCurrent;
static	DATEVAL	dvPastDate;
	int		nr;
	double	PercentDiff, CurrentClose, PastClose;
	int		CheckAnyway;

	switch ( xstock.xstype[0] )
	{
		case STYPE_BOND:
		case STYPE_CRYPTO:
		case STYPE_INDEX:
		case STYPE_OTHER:
			return ( 0 );
		default:
			VisitCount++;
			break;
	}

	if ( firstpass ) 
	{
		firstpass = 0;
		CurrentDateval ( &dvCurrent );
		time_t Time = time ( NULL );
		struct tm * LocalTime = localtime ( &Time );

		/* int tm_wday;   Day of the week (0-6, Sunday = 0) */
		switch ( LocalTime->tm_wday )
		{
			case 0:
				Days += 2;
				break;
			case 6:
				Days += 1;
				break;
			default:
				break;
		}

		Days = 0 - Days;
		DateAdd ( &dvCurrent, Days, &dvPastDate );
	}

	// xl = lastchr ( xstock.xsticker, nsStrlen(xstock.xsticker) );
	// xstock.xsticker[xl] = '\0';
	if ( RunMode != MODE_ONE )
	{
		CheckAnyway = 0;

		sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'", xstock.xsticker, xstock.xslast );
		nr = LoadHistory ( &MySql, WhereClause, &xhistory, 0 );
		if ( nr == 0 )
		{
			if ( Debug > 1 )
			{
				printf ( "No history for %s\n", WhereClause );
			}
			CurrentClose = xstock.xsclose;
			CheckAnyway = 1;
		}
		else
		{
			CurrentClose = xhistory.xhclose;
		}

	
		/*----------------------------------------------------------
			find beginning of range
		----------------------------------------------------------*/
		sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%04d-%02d-%02d'", 
			xstock.xsticker, dvPastDate.year4, dvPastDate.month, dvPastDate.day );
		nr = LoadHistory ( &MySql, WhereClause, &xhistory, 0 );
		if ( nr == 0 )
		{
			if ( Debug > 1 )
			{
				printf ( "No history for %s\n", WhereClause );
				printf ( "Trying earliest on history\n" );
			}
			/*----------------------------------------------------------
				if not found find earliest history
			----------------------------------------------------------*/
			sprintf ( WhereClause, "Hticker = '%s'", xstock.xsticker );
			nr = LoadHistoryCB ( &MySql, WhereClause, "Hdate", &xhistory, EachHistory, 0 );
			if ( nr == 0 )
			{
				/*----------------------------------------------------------
					if that not found, the bag it
				----------------------------------------------------------*/
				if ( Debug > 1 )
				{
					printf ( "No history for %s, bag it\n", WhereClause );
				}
				return ( 0 );
			}
			else
			{
				if ( Debug > 1 )
				{
					printf ( "Earliest %s %.2f\n", xhistory.xhdate, xhistory.xhclose );
				}
				PastClose = xhistory.xhclose;
			}
		}
		else
		{
			if ( Debug > 1 )
			{
				printf ( "Range start %s %.2f\n", xhistory.xhdate, xhistory.xhclose );
			}
			PastClose = xhistory.xhclose;
		}

		PercentDiff = 100.0 * CurrentClose / PastClose;
		if ( PercentDiff > 200.0  )
		{
			if ( Debug > 1 )
			{
				printf ( "%s reverse split? past %.2f current %.2f\n", xstock.xsticker, PastClose, CurrentClose );
			}
		}
		else if ( PercentDiff < 50.0  )
		{
			if ( Debug > 1 )
			{
				printf ( "%s split? past %.2f current %.2f\n", xstock.xsticker, PastClose, CurrentClose );
			}
		}
		else if ( CheckAnyway == 0 )
		{
			return ( 0 );
		}
		else
		{
			if ( Debug > 1 )
			{
				printf ( "%s check anyway\n", xstock.xsticker );
			}
		}
	}

	sprintf ( FileName, "%s/%s_split.json", TEMPDIR, xstock.xsticker );
	
	switch ( Debug )
	{
		case 0:
			break;
		case 1:
			printf ( "Check %s\n", xstock.xsticker );
			break;
		case 2:
			printf ( "%s\n", FileName );
			break;
	}

/*---------------------------------------------------------------------------
 curl -s "$BASEURL/stock/$2/splits/range=2w?token=$TOKEN" | JsonTree -
		"curl -s '%s/stock/%s/splits/%s?&token=%s' > %s",
---------------------------------------------------------------------------*/

	sprintf ( cmdline, 
		"curl -s '%s/stock/%s/splits/%s?token=%s' > %s",
				env_ApiURL, xstock.xsticker, Range, env_ApiKey, FileName );

	if ( Debug > 1 )
	{
		printf ( "%s\n", cmdline );
	}

	IEX_RateLimit ( 0 );

	/*----------------------------------------------------------
		fixit - still using curl system() instead of library.
	----------------------------------------------------------*/

	system ( cmdline );

	CheckCount++;

	if (( buffer = JsonOpenFileAndRead ( FileName )) == NULL )
	{
		if ( Debug > 1 )
		{
			printf ( "JsonOpenFileAndRead NULL on %s\n", FileName );
		}
		return ( 0 );
	}

	if ( nsStrncmp ( buffer, "[]", 2 ) == 0 )
	{
		if ( Debug )
		{
			printf ( "No split on file for %s\n", xstock.xsticker );
		}
	}
	else
	{
		FoundCount++;

		printf ( "%s", xstock.xsticker );
		if (( ptrNameValue = JsonScan ( buffer, "exDate", JSON_FIRST )) != NULL )
		{
			printf ( " exDate %-10.10s", ptrNameValue->Value );
		}
		if (( ptrNameValue = JsonScan ( buffer, "fromFactor", JSON_FIRST )) != NULL )
		{
			printf ( " fromFactor %d", nsAtoi(ptrNameValue->Value) );
		}
		if (( ptrNameValue = JsonScan ( buffer, "toFactor", JSON_FIRST )) != NULL )
		{
			printf ( " toFactor %d", nsAtoi(ptrNameValue->Value) );
		}
		printf ( "\n" );
	}

	free ( buffer );

	return ( 0 );
}
