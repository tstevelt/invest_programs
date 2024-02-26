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

#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<ctype.h>
#include	<math.h>

#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <libgen.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <sys/types.h>

#include	"shslib.h"

#include	"dbylib.h"

#define		STOCK
#define		HISTORY
#define		FUNDAMENTAL
#include	"fileinvest.h"
#include	"invlib.h"

/*----------------------------------------------------------
	app defines and variables
----------------------------------------------------------*/
#define		SIX_DAYS	518400
#define		SEVEN_DAYS	604800
#define		STOCK_TYPES		"SAE"

TYPE	char	StockIndex;
TYPE	char	*InputFileName;
TYPE	char	*Ticker;
TYPE	long	StartTime;
TYPE	long	EndTime;
TYPE	int		Ignore;
TYPE	int		UpdateDB;
TYPE	int		Verbose;
TYPE	int		Debug;

#ifdef STUFF
TYPE	char	cmdline[1024];
TYPE	char	ScriptFileName[128];
TYPE	int		ReportOld;
TYPE	int		DeleteOld;
TYPE	long	NapTime;

#define		MAXSTRING	128

typedef struct
{
	char	xsticker[21];
	char	xsname[31];
	double	xsclose;

	
	char	quoteType[MAXSTRING];
	char	country[MAXSTRING];
	long	ebitda;
	long	enterpriseValue;
	double	enterpriseToEbitda;
	double	trailingEps;
	double	forwardEps;
	double	forwardPE;
	double	profitMargins;
	long	floatShares;
	double	beta;
	double	priceToBook;
	double	growth5;
	double	growth1;
	double	targetMeanPrice;
	double	recommendationMean;
	double	quickRatio;
	double	debtToEquity;
	double	returnOnAssets;
	long	freeCashflow;
	double	revenueGrowth;
	double	earningsGrowth;
	long	sharesOutstanding;
	// double	trailingAnnualDividendYield;
	double	forwardYield;

	/* following for ETF */
	long	totalAssets;
	double	etf_yield;
	double	threeYearAverageReturn;
	double	fiveYearAverageReturn;
	long	marketCap;

	long	averageDailyVolume10Day;
	char	fundInceptionDate[12];

} STOCK_RECORD;

TYPE	STOCK_RECORD	StockRecord;
#endif

TYPE	int				StockCount;
TYPE	int				ErrorCount;
TYPE	int				AgeCount;
TYPE	int				UpdateCount;
TYPE	int				InsertCount;
TYPE	int				DoInsert;

/*----------------------------------------------------------
	mysql and dbylib stuff
----------------------------------------------------------*/
TYPE  MYSQL   MySql;
TYPE	DBY_OPTIONS	DbyOptions;
TYPE	char		Statement[2048];
TYPE	char		WhereClause[1024];
TYPE	char		OrderByClause[1024];
#ifdef MAIN
char	*LogFileName = "/var/local/invest.log";
#else
TYPE	char	*LogFileName;
#endif

/*------------------------------
:.,$d
:r ! mkproto -p *.c
------------------------------*/

/* EachStock.c */
int EachStock ( XSTOCK *ptr );

/* getargs.c */
void getargs ( int argc , char *argv []);

/* UpdateFundamental.c */
int main ( int argc , char *argv []);
