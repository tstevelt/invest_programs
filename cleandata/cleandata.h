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
#define		HISTORY
#define		PORTFOLIO
#include	"fileinvest.h"
#include	"invlib.h"

TYPE	int		Debug;
TYPE	char	Today[12];
TYPE	int		TotalUpdated;
TYPE	char	OutFileName[256];
TYPE	FILE	*fpOutput;
TYPE	DATEVAL	dvToday;
TYPE	DATEVAL	dvData;
TYPE	char	*SelectDate;
TYPE	char	*Ticker;
TYPE	int		FlagCount;

#define		MODE_STOCK			'S'
#define		MODE_HISTORY		'H'
#define		MODE_REPORT_ALL		'A'
#define		MODE_REPORT_ONE		'O'
TYPE	int		RunMode;
TYPE	char	OneTicker[22];
TYPE	char	ActionCode[4];

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

/* cleandata.c */
int main ( int argc , char *argv []);

/* do_history.c */
void do_history ( void );

/* do_report.c */
void do_report ( void );

/* do_stock.c */
void do_stock ( void );

/* getargs.c */
void getargs ( int argc , char *argv []);
