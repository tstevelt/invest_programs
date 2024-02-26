/*----------------------------------------------------------------------------
	Program : getdiv/EachStock.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: For each stock, load dividends. Get dividend information from IEX.
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

static	long	Affected;

int EachStock ()
{
	char	cmdline[1024];
	char	FileName[256];
	int		Expect, DateTok, DivTok, CurrTok;
	char	*cpPeriod;
	FILE	*fp;
	int		lineno;
	char	xbuffer[1024];
#define		MAXTOKS		20
	char	*tokens[MAXTOKS];
	int		tokcnt;
	double	DivValue;
	int		ndx, Days, xl;
	DATEVAL	dvExDate;
	XDIVIDEND	*Ptr, Key;
	int		CountInPastYear, CutoffDays;
	int		StockUpdated = 0;

	xl = lastchr ( xstock.xsticker, nsStrlen(xstock.xsticker) );

	xstock.xsticker[xl] = '\0';

	DividendCount = 0;
	sprintf ( WhereClause, "Dticker = '%s' and Dexdate >= '%s'", xstock.xsticker, FiveYearsAgo );
	LoadDividendCB ( &MySql, WhereClause, "Dexdate", &xdividend, (int(*)()) EachDividend, 0 );

	CountInPastYear = 0;
	for ( ndx = 0; ndx < DividendCount; ndx++ )
	{
		StrToDatevalFmt ( DividendArray[ndx].xdexdate, DATEFMT_YYYY_MM_DD, &dvExDate );
		Days = DateDiff ( &dvExDate, &dvToday );
		if ( Days < 365 )
		{
			CountInPastYear++;
		}

		if ( Debug >= 2 )
		{
			printf ( "%s %.2f\n", DividendArray[ndx].xdexdate, DividendArray[ndx].xdamount );
		}
	}
	
	if ( CountInPastYear >= 10 )
	{
		/* monthly */
		CutoffDays = 25;
	}
	else if ( CountInPastYear >= 3 )
	{
		/* quarterly */
		CutoffDays = 85;
	}
	else if ( CountInPastYear >= 1 )
	{
		/* semi-annual */
		CutoffDays = 177;
	}
	else
	{
		/* annual */
		CutoffDays = 360;
	}

	if ( IgnoreCutoff == 0 && DividendCount > 0 )
	{
		qsort ( DividendArray, DividendCount, sizeof(XDIVIDEND), (int(*)()) cmpdividend );

		StrToDatevalFmt ( DividendArray[DividendCount-1].xdexdate, DATEFMT_YYYY_MM_DD, &dvExDate );
		Days = DateDiff ( &dvExDate, &dvToday );
		if ( Debug )
		{
			printf ( "FYI: %s Last dividend on file is %d days ago, cutoff %d days\n", xstock.xsticker, Days, CutoffDays );
		}	

		if ( Days < CutoffDays )
		{
			return ( 0 );
		}
	}
	else
	{
		Days = -1;
	}

	sprintf ( FileName, "%s/%s_div.csv", TEMPDIR, xstock.xsticker );

	if ( Debug >= 2 )
	{
		printf ( "%s\n", FileName );
	}

/*---------------------------------------------------------------------------
0      1           2          3            4      5    6
exDate,paymentDate,recordDate,declaredDate,amount,flag,currency,description,frequency
2019-02-15,2019-02-24,2019-02-24,2019-01-30,0.74,Cao hQg NQone,SUD,SeipraP0feleD3drue   rp.sv ley oi r $ecnAdQhae7rlatD,rtlyQeaur
2018-11-09,2018-11-26,2018-11-25,2018-11-12,0.74,hNenQCaQ  goo,UDS,e rlefata37eD0ArP hD e viSnroy   deulep $r.dseclpQari,Quarlrtey

0      1        2            3           4      5    6         7           8          9    10      11 12  13     14   15
amount,currency,declaredDate,description,exDate,flag,frequency,paymentDate,recordDate,refid,symbol,id,key,subkey,date,updated
0.18,USD,2021-01-19,Ordinary Shares,2021-03-04,Cash,quarterly,2021-03-26,2021-03-05,2142061,BAC,DIVIDENDS,BAC,2142061,1614816000000,1611095933000
0.18,USD,2020-10-21,Ordinary Shares,2020-12-03,Cash,quarterly,2020-12-24,2020-12-04,2050526,BAC,DIVIDENDS,BAC,2050526,1606953600000,1608134902000
0.18,USD,2020-07-22,Ordinary Shares,2020-09-03,Cash,quarterly,2020-09-25,2020-09-04,1975551,BAC,DIVIDENDS,BAC,1975551,1599091200000,1608134902000
0.18,USD,2020-04-22,Ordinary Shares,2020-06-04,Cash,quarterly,2020-06-26,2020-06-05,1887889,BAC,DIVIDENDS,BAC,1887889,1591228800000,1608134902000
0.18,USD,2020-01-29,Ordinary Shares,2020-03-05,Cash,quarterly,2020-03-27,2020-03-06,1806414,BAC,DIVIDENDS,BAC,1806414,1583366400000,1608134902000
0.18,USD,2019-10-22,Ordinary Shares,2019-12-05,Cash,quarterly,2019-12-27,2019-12-06,1693769,BAC,DIVIDENDS,BAC,1693769,1575504000000,1608134902000
0.18,USD,2019-07-25,Ordinary Shares,2019-09-05,Cash,quarterly,2019-09-27,2019-09-06,1639748,BAC,DIVIDENDS,BAC,1639748,1567641600000,1608134902000
0.15,USD,2019-04-24,Ordinary Shares,2019-06-06,Cash,quarterly,2019-06-28,2019-06-07,1585116,BAC,DIVIDENDS,BAC,1585116,1559779200000,1608134902000
0.15,USD,2019-01-30,Ordinary Shares,2019-02-28,Cash,quarterly,2019-03-29,2019-03-01,1515199,BAC,DIVIDENDS,BAC,1515199,1551312000000,1608134902000
---------------------------------------------------------------------------*/
	Expect = 5;
	DateTok = 4;
	DivTok = 0;
	CurrTok = 1;

	if ( Days == -1 )
	{
		cpPeriod = "5y";
	}
	else if ( Days < (30+20) )
	{
		cpPeriod = "1m";
	}
	else if ( Days < (30*3+20) )
	{
		cpPeriod = "3m";
	}
	else if ( Days < (30*6+20) )
	{
		cpPeriod = "6m";
	}
	else if ( Days < (365+20) )
	{
		cpPeriod = "1y";
	}
	else if ( Days < (365*2+20) )
	{
		cpPeriod = "2y";
	}
	else
	{
		cpPeriod = "5y";
	}

	sprintf ( cmdline, 
		"curl -s '%s/stock/%s/dividends/%s?format=csv&token=%s' > %s",
				env_ApiURL, xstock.xsticker, cpPeriod, env_ApiKey, FileName );

	if ( Debug )
	{
		printf ( "%s\n", cmdline );
	}

	IEX_RateLimit ( 0 );

	/*----------------------------------------------------------
		fixit - still using curl system() instead of library.
	----------------------------------------------------------*/

	system ( cmdline );

	if (( fp = fopen ( FileName, "r" )) == (FILE *)0 )
	{
		printf ( "Cannot open %s for reading\n", FileName );
		return ( 0 );
	}

	lineno = 0;
	while ( fgets ( xbuffer, sizeof(xbuffer), fp ) != (char *)0 )
	{
		lineno++;

		if ( Debug >= 2 )
		{
			printf ( "%2d: %s\n", lineno, xbuffer );
		}
		
		if ( nsStrcmp ( xbuffer, "Unknown symbol" ) == 0 )
		{
			break;
		}

		if ( xbuffer[0] < '0' || xbuffer[0] > '9' )
		{
			continue;
		}

		if (( tokcnt = GetTokensA ( xbuffer, ",\n\r", tokens, MAXTOKS )) < Expect ) 
		{
			printf ( "%s: syntax error on line %d\n", FileName, lineno );
			continue;
		}

		DivValue = nsAtof ( tokens[DivTok] );

		if ( DivValue < 0.001 )
		{
			continue;
		}

		if ( Debug >= 2 )
		{
			printf ( "[%s]=%d %s %s\n", tokens[DateTok], (int)nsStrlen(tokens[DateTok]), tokens[DivTok], tokens[CurrTok] );
		}

		if ( DividendCount > 0 )
		{
			sprintf ( Key.xdexdate, "%s", tokens[DateTok] );

			/*---------------------------------------------------------------------------
				Can't use amount, due to rounding
			xxx	Key.xdamount =  nsAtof(tokens[DivTok]);
				| MCFT    | 2016-06-02 |     3 |
				| VOD     | 2018-06-07 |     3 |
				| SNY     | 2018-05-11 |     3 |
				| EFAV    | 2018-12-18 |     3 |
				| VTTHX   | 2018-12-28 |     3 |
			---------------------------------------------------------------------------*/

			Ptr = bsearch ( &Key, DividendArray, DividendCount, sizeof(XDIVIDEND), (int(*)()) cmpdividend ); 
			if ( Ptr )
			{
				if ( Debug )
				{
					printf ( "%s %.2f already on file\n", Key.xdexdate, Key.xdamount );
				}
				continue;
			}
		}

		/*--------+------------------+------+-----+---------+----------------+
		| Field   | Type             | Null | Key | Default | Extra          |
		+---------+------------------+------+-----+---------+----------------+
		| id      | int(11) unsigned | NO   | PRI | NULL    | auto_increment |
		| Dticker | varchar(20)      | NO   | MUL |         |                |
		| Dexdate | date             | NO   |     | NULL    |                |
		| Damount | double           | NO   |     | 0       |                |
		| Dcurr   | char(4)          | NO   |     | USD     |                |
		+---------+------------------+------+-----+---------+---------------*/

		sprintf ( Statement, 
				"insert into dividend (Dticker, Dexdate, Damount, Dcurr) \
					values ( '%s', '%s', %.4f, '%s' )",
						xstock.xsticker, tokens[DateTok], nsAtof(tokens[DivTok]), tokens[CurrTok] );
#ifdef SANDBOX
		printf ( "%s\n", Statement );
#else
		Affected = dbyInsert ( "getdiv", &MySql, Statement, 0, LogFileName );
		TotalUpdated += Affected;
		StockUpdated += Affected;
#endif
	}

	nsFclose ( fp );
	if ( Debug == 0 )
	{
		unlink ( FileName );
	}
	else
	{
		printf ( "Not removing %s\n", FileName );
	}

	if ( StockUpdated > 0 )
	{
		sprintf ( WhereClause, "Pticker = '%s'", xstock.xsticker );
		if ( dbySelectCount ( &MySql, "portfolio", WhereClause, LogFileName ) > 0 )
		{
			/*----------------------------------------------------------
				for now, check all.  future maybe just if in portfolio
				tms		02/18/2024	only if in a portfolio.
			----------------------------------------------------------*/
			DividendCuts ();
		}
	}


	return ( 0 );
}
