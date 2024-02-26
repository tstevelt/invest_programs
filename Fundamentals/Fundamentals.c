/*----------------------------------------------------------------------------
	Program : Fundamentals.c
	Author  : Silver Hammer Software LLC
	Author  : Tom Stevelt
	Date    : March 2021
	Synopsis: Get Fundamentals from IEX. After "Fun with Fundamentals" webinar.
	Return  : 

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		07/13/2021	Get private api keys with GetInvestCfg().

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

#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>

#define		MAIN
#include	"shslib.h"
#include	"dbylib.h"
#include	"invlib.h"

#define		GET_MODE	1
#define		PARSE_MODE	2
#define		REPORT_MODE	3
static	int		RunMode = 0;

static	char	*Ticker;
static	int		Debug = 0;
static	int		Format = 'T';
static	int		FilterResults = 0;

static void MyUsage ()
{
	printf ( "USAGE: Fundamentals {-g|-p|-r|-c} TICKER [options]\n" );
	printf ( " -g get from iex, saved as raw\n" );
	printf ( " -p parse from raw to txt\n" );
	printf ( " -r write txt report\n" );
	printf ( " -c write csv format\n" );
	printf ( " -f filter\n" );
	printf ( " -d debug\n" );
	exit ( 1 );
}

static void getargs ( int argc, char *argv[] )
{
	int		xa;

	if ( argc < 2 )
	{
		MyUsage ();
	}

	for ( xa = 1; xa < argc; xa++ )
	{
		if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-g" ) == 0 )
		{
			RunMode = GET_MODE;
			xa++;
			Ticker = argv[xa];
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-p" ) == 0 )
		{
			RunMode = PARSE_MODE;
			xa++;
			Ticker = argv[xa];
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-r" ) == 0 )
		{
			RunMode = REPORT_MODE;
			Format = 'T';
			xa++;
			Ticker = argv[xa];
		}
		else if ( xa + 1 < argc && nsStrcmp ( argv[xa], "-c" ) == 0 )
		{
			RunMode = REPORT_MODE;
			Format = 'C';
			xa++;
			Ticker = argv[xa];
		}
		else if ( nsStrcmp ( argv[xa], "-f" ) == 0 )
		{
			FilterResults = 1;
		}
		else if ( nsStrcmp ( argv[xa], "-d" ) == 0 )
		{
			Debug = 1;
		}
		else
		{
			MyUsage ();
		}
	}
}

int main ( int argc, char *argv [] )
{
	FILE	*fp;
	char	Char;
	char	CommandLine[1024];
	char	FileName[128];
	int		InQuote = 0;
	int		Counter = 0;
	char	Buffer[1024];
	char	*tokens[4];
	int		tokcnt;
	int		KeepIt;
	char	symbol[10];
	char	companyName[30];
	char	country[6];
	double	shareholderEquity = 0.0;
	double	cashFlow = 0.0;
	double	ebit = 0.0;
	double	grossProfit = 0.0;
	double	netIncome = 0.0;
	double	totalAssets = 0.0;
	double	totalDebt = 0.0;
	double	totalRevenue = 0.0;
	double	thisDebt = 0.0;
	double	researchAndDevelopment = 0.0;
	int		employees = 0;
	double	Percent = 0.0;

	getargs ( argc, argv );

	GetInvestCfg ( 0 );
	
	/*----------------------------------------------------------
		fixit - still using curl system() instead of library.
	----------------------------------------------------------*/

	switch ( RunMode )
	{
		case GET_MODE:
			sprintf ( FileName, "%s.json", Ticker );

			sprintf ( CommandLine, "curl -s '%s/stock/%s/company?format=json&token=%s' > %s", env_ApiURL, Ticker, env_ApiKey, FileName );
			if ( Debug )
			{
				printf ( "%s\n", CommandLine );
			}
			IEX_RateLimit ( 0 );
			system ( CommandLine );
			sleep (  1 );

#ifdef BLOCK

			sprintf ( CommandLine, "curl -s '%s/data-points/%s?format=json&token=%s' >> %s", env_ApiURL, Ticker, env_ApiKey, FileName );
			if ( Debug )
			{
				printf ( "%s\n", CommandLine );
			}
			IEX_RateLimit ( 0 );
			system ( CommandLine );
			sleep (  1 );

#endif

			sprintf ( CommandLine, "curl -s '%s/stock/%s/financials?format=json&token=%s' >> %s", env_ApiURL, Ticker, env_ApiKey, FileName );
			if ( Debug )
			{
				printf ( "%s\n", CommandLine );
			}
			IEX_RateLimit ( 0 );
			system ( CommandLine );
			sleep (  1 );

			sprintf ( CommandLine, "curl -s '%s/stock/%s/balance-sheet?format=json&token=%s' >> %s", env_ApiURL, Ticker, env_ApiKey, FileName );
			if ( Debug )
			{
				printf ( "%s\n", CommandLine );
			}
			IEX_RateLimit ( 0 );
			system ( CommandLine );
			sleep (  1 );

			sprintf ( CommandLine, "curl -s '%s/stock/%s/income?format=json&token=%s' >> %s", env_ApiURL, Ticker, env_ApiKey, FileName );
			if ( Debug )
			{
				printf ( "%s\n", CommandLine );
			}
			IEX_RateLimit ( 0 );
			system ( CommandLine );

			break;

		case PARSE_MODE:
			sprintf ( FileName, "%s.json", Ticker );

			if (( fp = fopen ( FileName, "r" )) == NULL )
			{
				printf ( "Cannot open %s for reading\n", FileName );
				exit ( 1 );
			}

			while (( Char =  fgetc ( fp )) != EOF )
			{
				Counter++;

//				if ( Counter == 1000 )
//				{
//					fprintf ( stderr, "Counter %d\n", Counter );
//				}

				switch ( Char )
				{
					case '"':
						if ( InQuote )
						{
							InQuote = 0;
						}
						else
						{
							InQuote = 1;
						}
						break;
					case '[':
					case ']':
					case '{':
						break;
					case '}':
						putc ( '\n', stdout );
						putc ( '\n', stdout );
						break;
					case ',':
						if ( InQuote )
						{
							putc ( Char, stdout );
						}
						else
						{
							putc ( '\n', stdout );
						}
						break;
					default:
						putc ( Char, stdout );
						break;

				}
			}
			nsFclose ( fp );
			break;

		case REPORT_MODE:
			sprintf ( FileName, "%s.txt", Ticker );

			if (( fp = fopen ( FileName, "r" )) == NULL )
			{
				printf ( "Cannot open %s for reading\n", FileName );
				exit ( 1 );
			}

			while ( fgets ( Buffer, sizeof(Buffer), fp ) != (char *)0 )
			{
				if (( tokcnt = GetTokensStd ( Buffer, ":\n\r", tokens, 4, 0 )) < 2 )
				{
					continue;
				}


				if ( nsStrcmp ( tokens[0], "shareholderEquity" ) == 0 )
				{
					shareholderEquity = nsAtof ( tokens[1] );
				}
				else if ( nsStrcmp ( tokens[0], "symbol" ) == 0 )
				{
					snprintf ( symbol, sizeof(symbol), "%s", tokens[1] );
				}
				else if ( nsStrcmp ( tokens[0], "companyName" ) == 0 )
				{
					snprintf ( companyName, sizeof(companyName), "%s", tokens[1] );
				}
				else if ( nsStrcmp ( tokens[0], "country" ) == 0 )
				{
					snprintf ( country, sizeof(country), "%s", tokens[1] );
				}
				else if ( nsStrcmp ( tokens[0], "cashFlow" ) == 0 )
				{
					cashFlow = nsAtof ( tokens[1] );
				}
				else if ( nsStrcmp ( tokens[0], "ebit" ) == 0 )
				{
					ebit = nsAtof ( tokens[1] );
				}
				else if ( nsStrcmp ( tokens[0], "grossProfit" ) == 0 )
				{
					grossProfit = nsAtof ( tokens[1] );
				}
				else if ( nsStrcmp ( tokens[0], "netIncome" ) == 0 )
				{
					netIncome = nsAtof ( tokens[1] );
				}
				else if ( nsStrcmp ( tokens[0], "totalAssets" ) == 0 )
				{
					totalAssets = nsAtof ( tokens[1] );
				}
				else if (( nsStrcmp ( tokens[0], "totalDebt"           ) == 0 ) ||
						 ( nsStrcmp ( tokens[0], "currentLongTermDebt" ) == 0 ) ||
						 ( nsStrcmp ( tokens[0], "longTermDebt"        ) == 0 ))
				{
					thisDebt = nsAtof ( tokens[1] );
					if ( totalDebt < thisDebt )
					{
						totalDebt = thisDebt;
					}
				}
				else if ( nsStrcmp ( tokens[0], "totalRevenue" ) == 0 )
				{
					totalRevenue = nsAtof ( tokens[1] );
				}
				else if ( nsStrcmp ( tokens[0], "researchAndDevelopment" ) == 0 )
				{
					researchAndDevelopment = nsAtof ( tokens[1] );
				}
				else if ( nsStrcmp ( tokens[0], "employees" ) == 0 )
				{
					employees = nsAtoi ( tokens[1] );
				}
				else if ( Debug )
				{
					printf ( "skipping %s = %s\n", tokens[0], tokens[1] );
				}

			}

			nsFclose ( fp );

if ( Format == 'C' )
{
printf ( "TICKER,COMPANY,CN,REVENU,GRSPCT,GPCT,EBIT,EPCT,CSHFLO,CPCT,R&D,RDPC,LT/EQUITY,LT/ASSETS,NET-MRG,EMPLO\n");
}
else
{
//        INTC   Intel Corp.                    US  19978  11348 56.8%   5936 29.7%  35384 177.1%   3655 18.3%     0.45       0.24    29.3% 110600
printf ( "TICKER COMPANY                        CN REVENU GRSPCT  GPCT   EBIT EPCT  CSHFLO   CPCT    R&D  RDPC LT/EQUITY  LT/ASSETS NET-MRG  EMPLO\n" );
printf ( "----------------------------------------------------------------------------------------------------------------------------------------\n" );
}
			
			
			KeepIt = 1;
			if ( FilterResults )
			{
				if ( nsStrncmp ( country, "China", 5 ) == 0 )
				{
					KeepIt = 0;
				}
				else if ( totalRevenue < 1.0 )
				{
					KeepIt = 0;
				}
				else if ( totalRevenue > 0.0 && 100.0 * researchAndDevelopment / totalRevenue < 5.0 )
				{
					KeepIt = 0;
				}
				else if ( totalRevenue > 0.0 && 100.0 * netIncome / totalRevenue < 5.0 )
				{
					KeepIt = 0;
				}
				else if ( shareholderEquity != 0.0 && totalDebt / shareholderEquity > 0.5 )
				{
					KeepIt = 0;
				}
			}

			if ( KeepIt )
			{
				if ( Format == 'C' )
				{
					Stripper ( companyName, "," );
				}

				Percent = 0.0;

				printf ( "%-6.6s",    symbol );
				printf ( "%s%-30.30s", Format == 'C' ? "," : " ", companyName );
				printf ( "%s%-5.5s", Format == 'C' ? "," : " ", nsStrlen(country) ? country : " " );
				printf ( "%s%6.0f", Format == 'C' ? "," : " ",    totalRevenue / 1000000.0 );

				printf ( "%s%6.0f", Format == 'C' ? "," : " ",    grossProfit / 1000000.0 );
				if ( totalRevenue > 0.0 ) Percent = 100.0 * grossProfit / totalRevenue;
				printf ( "%s%4.1f%%", Format == 'C' ? "," : " ", Percent );

				printf ( "%s%6.0f", Format == 'C' ? "," : " ",    ebit / 1000000.0 );
				if ( totalRevenue > 0.0 ) Percent = 100.0 * ebit / totalRevenue;
				printf ( "%s%4.1f%%", Format == 'C' ? "," : " ", Percent );

				printf ( "%s%6.0f", Format == 'C' ? "," : " ",    cashFlow / 1000000.0 );
				if ( totalRevenue > 0.0 ) Percent = 100.0 * cashFlow / totalRevenue;
				printf ( "%s%4.1f%%", Format == 'C' ? "," : " ", Percent );

				printf ( "%s%6.0f", Format == 'C' ? "," : " ",    researchAndDevelopment / 1000000.0 );
				if ( totalRevenue > 0.0 ) Percent = 100.0 * researchAndDevelopment / totalRevenue;
				printf ( "%s%4.1f%%", Format == 'C' ? "," : " ", Percent );

				printf ( "%s%5.2f", Format == 'C' ? "," : "    ", shareholderEquity != 0.0 ? totalDebt / shareholderEquity : 0.0 );	// 4
				printf ( "%s%5.2f", Format == 'C' ? "," : "      ", totalAssets != 0.0 ? totalDebt / totalAssets : 0.0 );		// 6
				printf ( "%s%4.1f%%", Format == 'C' ? "," : "    ", totalRevenue != 0.0 ? 100.0 *  netIncome / totalRevenue : 0.0 );	// 4

				printf ( "%s%6d", Format == 'C' ? "," : " ",      employees );

	//			printf ( "%s%6.0f", Format == 'C' ? "," : " ",    shareholderEquity / 1000000.0 );
	//			printf ( "%s%6.0f", Format == 'C' ? "," : " ",    shareholderEquity / 1000000.0 );
	//			printf ( "%s%6.0f", Format == 'C' ? "," : " ",    totalDebt / 1000000.0 );
	//			printf ( "%s%6.0f", Format == 'C' ? "," : " ",    netIncome / 1000000.0 );
	//			printf ( "%s%6.0f", Format == 'C' ? "," : " ",    totalDebt / 1000000.0 );
	//			printf ( "%s%6.0f", Format == 'C' ? "," : " ",    shareholderEquity / 1000000.0 );

				printf ( "\n" );
			}
			break;

	}

	return ( 0 );
}
