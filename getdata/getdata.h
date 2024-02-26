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
#include	<errno.h>

#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <libgen.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <sys/types.h>
#include <curl/curl.h>

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

#define		FORMAT_JSON			91
#define		FORMAT_CSV			92

TYPE	int		Debug;
TYPE	int		Format;
TYPE	int		UseTiingo;
TYPE	int		Quiet;
TYPE	char	Today[12];
TYPE	char	Yesterday[12];
TYPE	char	MonthAgoDate[12];
TYPE	char	YearAgoDate[12];
TYPE	char	PastDate[12];
TYPE	int		CheckSlast;
TYPE	char	*FlagFile;
TYPE	char	*SubjectFile;

#define	MODE_ALL		11
#define	MODE_NULL		21
#define	MODE_ONE		31
#define	MODE_SPLITS		41
TYPE	int		RunMode;

#define	PERIOD_OHLC			41
#define	PERIOD_PREVIOUS		42
#define	PERIOD_ONE_MONTH	43
#define	PERIOD_TWO_YEAR		44
#define	PERIOD_THREE_YEAR	45
#define	PERIOD_FIVE_YEAR	46
#define	PERIOD_TEN_YEAR		47
#define	PERIOD_PAST			48
TYPE	int		Period;

TYPE	char	TempFileName[128];
TYPE	int		StockCount;
TYPE	int		HistoryCount;
TYPE	int		SlastCount;
TYPE	int		PE_Count;
TYPE	int		SourceTime;
TYPE	int		DestinationTime;
TYPE	int		InsertFailedErrorCount;
TYPE	int		MissingMajorDataErrorCount;
TYPE	int		MissingBenchmarkDataErrorCount;
TYPE	int		MissingPortfolioDataErrorCount;
TYPE	int		MissingOtherDataErrorCount;
TYPE	int		StillOpenErrorCount;
TYPE	char	cmdline[2048];
TYPE	char	buffer[1024];
#define		MAXTOKS		30
TYPE	char	*tokens[MAXTOKS];
TYPE	int		tokcnt;


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

/*----------------------------------------------------------
	libcurl stuff
----------------------------------------------------------*/
TYPE	CURL		*curl;
TYPE	CURLcode	curlRV;

/*------------------------------
:.,$d
:r ! mkproto -p *.c
------------------------------*/

/* CheckSplits.c */
void CheckSplits ( void );

/* EachStock.c */
int EachStock ( void );

/* getargs.c */
void getargs ( int argc , char *argv []);

/* getdata.c */
int main ( int argc , char *argv []);
