/*----------------------------------------------------------
'https://sandbox.iexapis.com/beta/stock/AAPL/previous?filter=date,open,high,low,close,volume&format=csv&token=Tpk_d9b468e630a611e9958142010a80043c'
	IEX Cloud
		Tsk_c08a82fb30a611e9958142010a80043c - secret
		Tpk_d9b468e630a611e9958142010a80043c - publishable
----------------------------------------------------------*/
#include	"getdata.h"

static	char	cmdline[2048];
static	char	buffer[1024];
#define		MAXTOKS		10
static	char	*tokens[MAXTOKS];
static	int		tokcnt;

#define	IEX_CLOUD_APISTR	"Tpk_d9b468e630a611e9958142010a80043c"

int TestIEXCloud ()
{
	int		lineno, Expected;
	FILE	*fp;
	time_t		stime, etime;
	int		HistoryThisStock;

	StockCount++;

	if ( Debug )
	{
		printf ( "%s %s\n", xstock.xsticker, xstock.xslast );
	}

	HistoryThisStock = 0;

	switch ( RunMode )
	{
		case MODE_TEST_EOD:
			Expected = 6;
			sprintf ( cmdline, 
					"curl -s 'https://sandbox.iexapis.com/beta/stock/%s/previous?filter=date,open,high,low,close,volume&format=csv&token=%s' > %s",
							xstock.xsticker, IEX_CLOUD_APISTR, TempFileName );
			break;

		case MODE_TEST_DIV:
			Expected = 8;
			// curl -s "$BASEURL/stock/$2/dividends/1m?format=csv&token=$TOKEN"
			sprintf ( cmdline, 
					"curl -s 'https://sandbox.iexapis.com/beta/stock/%s/dividends/1m?format=csv&token=%s' > %s",
							xstock.xsticker, IEX_CLOUD_APISTR, TempFileName );
			break;
	}

	time ( &stime );
	system ( cmdline );
	time ( &etime );
	SourceTime = SourceTime + ( etime - stime );

	if (( fp = fopen ( TempFileName, "r" )) == (FILE *)0 )
	{
		return ( -1 );
	}

	time ( &stime );

	lineno = 0;
	while ( fgets ( buffer, sizeof(buffer), fp ) != (char *)0 )
	{
		lineno++;

		if ( strncmp ( buffer, "Unknown symbol", 14 ) == 0 )
		{
			// printf ( "Unknown ticker %s\n", xstock.xsticker );
			break;
		}

		if (( strncmp ( buffer, "date,",   5 ) == 0 ) ||
			( strncmp ( buffer, "exDate,", 6 ) == 0 ))
		{
			continue;
		}
		
		if (( tokcnt = GetTokens ( buffer, ",\r\n", tokens, MAXTOKS )) < Expected )
		{
			if ( Debug )
			{
				printf ( "tokcnt %d on line %d\n", tokcnt, lineno );
			}
			continue;
		}

		switch ( RunMode )
		{
			case MODE_TEST_EOD:
				xhistory.xhopen = atof(tokens[1]);
				xhistory.xhhigh = atof(tokens[2]);
				xhistory.xhlow  = atof(tokens[3]);
				xhistory.xhclose = atof(tokens[4]);

				if ( xhistory.xhclose < 0.01 )
				{
					continue;
				}
				if ( xhistory.xhopen < 0.01 )
				{
					xhistory.xhopen = xhistory.xhclose;
				}
				if ( xhistory.xhhigh < 0.01 )
				{
					xhistory.xhhigh = xhistory.xhclose;
				}
				if ( xhistory.xhlow  < 0.01 )
				{
					xhistory.xhlow  = xhistory.xhclose;
				}

				printf ( "insert into history (%s) values ( '%s', '%s', %f, %f, %f, %f, %s )\n",
						INSERT_FIELDS,			
						xstock.xsticker,
						tokens[0],
						xhistory.xhopen,
						xhistory.xhhigh,
						xhistory.xhlow,
						xhistory.xhclose,
						tokens[5] );

				break;

			case MODE_TEST_DIV:
				/*---------------------------------------------------------------------------
					exDate,paymentDate,recordDate,declaredDate,amount,flag,currency,description,frequency
					2019-04-10,2019-04-13,2019-04-07,,0.0332,,DUS,,0
				---------------------------------------------------------------------------*/
				printf ("%10.10s: [%s] [%s] [%s] [%s] [%s] [%s] [%s] [%s]\n", 
					xstock.xsticker,
					tokens[0], tokens[1], tokens[2], tokens[3],
					tokens[4], tokens[5], tokens[6], tokens[7] );

				break;

		}
		HistoryThisStock++;
	}

	time ( &etime );
	DestinationTime = DestinationTime + ( etime - stime );

	fclose ( fp );

	if (  HistoryThisStock == 0 )
	{
		printf ( "Could not find data for %s, Slast %s.\n", xstock.xsticker, xstock.xslast );
	}

	return ( 0 );
}
