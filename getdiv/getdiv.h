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
#define		MEMBER
#define		HISTORY
#define		PORTFOLIO
#define		DIVIDEND
#include	"fileinvest.h"
#include	"invlib.h"

TYPE	int		Debug;
TYPE	int		IgnoreCutoff;
TYPE	char	Today[12];
TYPE	char	FiveYearsAgo[12];
TYPE	int		TotalUpdated;
TYPE	int		TotalCuts;
TYPE	char	OutFileName[256];
TYPE	FILE	*fpOutput;
TYPE	DATEVAL	dvToday;
TYPE	DATEVAL	dvData;

#define		MODE_ACTIVE		'A'
#define		MODE_CUTS		'C'
#define		MODE_UPDATE		'U'
#define		MODE_ONE		'1'
#define		MODE_MEMBER		'M'
TYPE	int		RunMode;
TYPE	char	OneTicker[22];

typedef struct
{
	char	xsticker[21];
} STOCK_RECORD;

#define		MAXSTOCKS		1000
TYPE	STOCK_RECORD		StockArray[MAXSTOCKS];
TYPE	int					StockCount;

/*----------------------------------------------------------
	some funds pay monthly, looking for five years. 5 x 12 = 60
	allow some wiggle room
----------------------------------------------------------*/
#define		MAXDIVIDENDS	100
TYPE	XDIVIDEND			DividendArray[MAXDIVIDENDS];
TYPE	int					DividendCount;

/*----------------------------------------------------------
	mysql and dbylib stuff
----------------------------------------------------------*/
TYPE  MYSQL   MySql;
TYPE	DBY_OPTIONS	DbyOptions;
TYPE	char		Statement[1024];
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

/* cmpfuncs.c */
int cmpstock ( STOCK_RECORD *a , STOCK_RECORD *b );
int cmpdividend ( XDIVIDEND *a , XDIVIDEND *b );

/* DividendCuts.c */
void DividendCuts ( void );

/* EachDividend.c */
int EachDividend ( void );

/* EachMember.c */
int EachMember ( void );

/* EachPortfolio.c */
int EachPortfolio ( void );

/* EachStock.c */
int EachStock ( void );

/* getargs.c */
void getargs ( int argc , char *argv []);

/* getdiv.c */
int main ( int argc , char *argv []);
