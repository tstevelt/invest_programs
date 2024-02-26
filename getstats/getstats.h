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
#define		FUNDAMENTAL
#define		HISTORY
#define		PORTFOLIO
#include	"fileinvest.h"
#include	"invlib.h"

#define		INSERT_FIELDS	"Hticker, Hdate, Hopen, Hhigh, Hlow, Hclose, Hvolume"

#define		MAX_SHORT_STRING	256
#define		MAX_LONG_STRING		2048
#define		MAXPAGES			20


TYPE	int		Debug;
TYPE	int		Quiet;
TYPE	char	Today[12];
TYPE	char	Yesterday[12];
TYPE	char	StockIndex;
TYPE	int		SkipNegatives;
TYPE	int		UpdateCount;

#define	MODE_INDEX		11
#define	MODE_ALL		21
#define	MODE_ONE		31
#define	MODE_INDEXED	41
#define	MODE_REPORT		51
TYPE	int		RunMode;

TYPE	char	TempFileName[128];
TYPE	int		StockCount;
TYPE	int		HistoryCount;
TYPE	int		SlastCount;
TYPE	int		SourceTime;
TYPE	int		DestinationTime;
TYPE	int		InsertFailedErrorCount;
TYPE	int		MissingBenchmarkDataErrorCount;
TYPE	int		MissingPortfolioDataErrorCount;
TYPE	int		MissingOtherDataErrorCount;

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

/* EachStock.c */
int EachStock ( void );

/* getargs.c */
void getargs ( int argc , char *argv []);

/* getstats.c */
int main ( int argc , char *argv []);

/* Report.c */
void Report ( void );
