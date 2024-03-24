/*----------------------------------------------------------------------------
	Program : UpdateFundamental/EachStock.c
	Author  : Tom Stevelt
	Date    : 2019 - 2024
	Synopsis: For each stock, update a few calculated fundamental fields.
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

#include	"UpdateFundamental.h"

#define		TWO_YEARS		(2*252+10)
#define		WEEKS			100

static	int		DebugEachStock = 1;
static	char	Dates   [WEEKS][11];
static	double	SpyROI  [WEEKS];
static	int		SpyCount;
static	double	SpyMean;
static	double	StockROI[WEEKS];
static	int		WeekCount;
static	double	StockMean;

double covariance ( double X[], double Xm, double Y[], double Ym, int N )
{
	double	SumOverN = 0.0;

	for ( int i = 0; i < N; i++ )
	{
		SumOverN += ( X[i] - Xm ) * ( Y[i] - Ym );
	}
	SumOverN = SumOverN / (double) N;

	return ( SumOverN  );
}

static int EachHistory ( XHISTORY *ptr )
{
	if ( HistoryCount >= TWO_YEARS )
	{
		return ( 1 );
	}

	sprintf ( HistoryArray[HistoryCount].Date, "%s", ptr->xhdate );
	HistoryArray[HistoryCount].Close = ptr->xhclose;
	HistoryCount++;

	return ( 0 );
}

int EachStock ( XSTOCK *ptr )
{
	static	int		firstpass = 1;
	int		xi;
	char	Fragment[128];
	int		FragCount = 0;
	double	NewValue;

	
	if ( firstpass )
	{
		firstpass = 0;

		HistoryCount = 0;
		sprintf ( WhereClause, "Hticker = 'SPY'"  );
		LoadHistoryCB ( &MySql, WhereClause, "Hdate desc", &xhistory, (int(*)()) EachHistory, 0 );

		if ( HistoryCount < TWO_YEARS )
		{
			printf ( "EachStock: NOT ENOUGH HISTORY SPY %d\n", HistoryCount );
			fflush ( stdout );
			exit ( 0 );
		}

		if ( Verbose )
		{
			printf ( "EachStock: SPY %d\n", HistoryCount );
			fflush ( stdout );
		}

		SpyMean = 0.0;
		for ( xi = HistoryCount - 6, SpyCount = 0; xi >= 0; xi -= 5, SpyCount++ )
		{
			if ( SpyCount >= WEEKS )
			{
				break;
			}

			SpyROI[SpyCount] = (HistoryArray[xi].Close - HistoryArray[xi+5].Close) / HistoryArray[xi+5].Close;
			sprintf ( Dates[SpyCount], "%s", HistoryArray[xi].Date );

			SpyMean += SpyROI[SpyCount];

			if ( DebugEachStock > 1 )
			{
				printf ( "%s %.2f\n", HistoryArray[xi+5].Date, HistoryArray[xi+5].Close );
				printf ( "%s %.2f\n", HistoryArray[xi  ].Date, HistoryArray[xi  ].Close );
				printf ( "%10.10s %f\n", " ", SpyROI[SpyCount] );
			}
		}
		SpyMean /= (double) SpyCount;
	}

	sprintf ( WhereClause, "Fticker = '%s'", ptr->xsticker );
	if (LoadFundamental ( &MySql, WhereClause, &xfundamental, 0 ) == 0 )
	{
		printf ( "EachStock: %s no fundamental record\n", ptr->xsticker );
		fflush ( stdout );
		return ( 0 );
	}

	/*---------------------------------------------------------------------------
		FIX RIDICULOUS  ...
	---------------------------------------------------------------------------*/
	if (  xstock.xstype[0] != 'E' && xfundamental.xfyield > 0.01 && xfundamental.xfdividend <= 0.0 )
	{
		xfundamental.xfdividend = 0.0;
		xfundamental.xfyield    = 0.0;

		sprintf ( Statement, "update fundamental set Fdividend = %.2f, Fyield = %.2f where Fticker = '%s'",
				xfundamental.xfdividend, xfundamental.xfyield, xfundamental.xfticker );

		if ( UpdateDB )
		{
			AgeCount += dbyUpdate ( "getdata", &MySql, Statement, 0, LogFileName );
		}
		else if ( Debug )
		{
			printf ( "%s\n", Statement );
		}
	}

	if ( xfundamental.xfdividend > xstock.xsclose * 3.0 && xfundamental.xfshares > 1000 )
	{
		xfundamental.xfdividend = xfundamental.xfdividend / (double) xfundamental.xfshares;

		sprintf ( Statement, "update fundamental set Fdividend = %.2f where Fticker = '%s'",
				xfundamental.xfdividend,  xfundamental.xfticker );

		if ( UpdateDB )
		{
			AgeCount += dbyUpdate ( "getdata", &MySql, Statement, 0, LogFileName );
		}
		else if ( Debug )
		{
			printf ( "%s\n", Statement );
		}
	}

	while ( 1 )
	{
		HistoryCount = 0;
		sprintf ( WhereClause, "Hticker = '%s'", ptr->xsticker );
		LoadHistoryCB ( &MySql, WhereClause, "Hdate desc", &xhistory, (int(*)()) EachHistory, 0 );

		if ( HistoryCount < TWO_YEARS )
		{
			if ( Verbose )
			{
				printf ( "EachStock: NOT ENOUGH HISTORY %s %d FOR BETA CALC\n", ptr->xsticker, HistoryCount );
				fflush ( stdout );
			}
			ErrorCount++;
			break; // return ( 0 );
		}

		if ( Verbose )
		{
			printf ( "EachStock: %s %d\n", ptr->xsticker, HistoryCount );
			fflush ( stdout );
		}

		StockMean = 0.0;
		for ( xi = HistoryCount - 6, WeekCount = 0; xi >= 0; xi -= 5, WeekCount++ )
		{
			if ( WeekCount >= WEEKS )
			{
				break;
			}

			StockROI[WeekCount] = (HistoryArray[xi].Close - HistoryArray[xi+5].Close) / HistoryArray[xi+5].Close;
			StockMean += StockROI[WeekCount];
		}
		StockMean /= (double) WeekCount;

		if ( WeekCount != SpyCount )
		{
			printf ( "EachStock: %s Stock count %d not equal SPY count %d\n", ptr->xsticker, WeekCount, SpyCount );
			fflush ( stdout );
			ErrorCount++;
			break; // return ( 0 );
		}

		/*--------------------------------------------------------------------------------------------------
			calculate beta
			https://www.omnicalculator.com/finance/beta-stock#how-to-calculate-beta-of-stock-an-example
			beta = covariance ( stock, spy ) / variance ( spy )
		--------------------------------------------------------------------------------------------------*/
		if ( DebugEachStock > 1 )
		{
			printf ( "EachStock: SPY ROI count %d mean %f\n", SpyCount, SpyMean );
			printf ( "EachStock: %s ROI count %d mean %f\n", ptr->xsticker,  WeekCount, StockMean );

			for ( xi = 0; xi < SpyCount; xi ++ )
			{
				printf ( "%s,%f,%f\n", Dates[xi], SpyROI[xi], StockROI[xi] );
			}
		}

		double SpyStdDev = stddev ( SpyROI, SpyCount );
		double SpyVariance = SpyStdDev * SpyStdDev;
		double CoVar = covariance ( SpyROI, SpyMean, StockROI, StockMean, SpyCount );
		double Beta = CoVar / SpyVariance;

		if ( Debug )
		{
			printf ( "SPY stddev %.8f  variance %.8f\n", SpyStdDev, SpyVariance );
			printf ( "Co-Variance %.8f\n", CoVar );
			printf ( "Beta %f\n", Beta );
		}

		if ( dbldiff(xfundamental.xfbeta,Beta,0.01) )
		{
			sprintf ( Statement, "update fundamental set Fbeta = %.2f where Fticker = '%s'", Beta, ptr->xsticker );

			if ( UpdateDB )
			{
				if ( dbyUpdate ( "invest", &MySql, Statement, 0,  LogFileName ) != 1 )
				{
					fprintf ( stderr, "dbyUpdate falied: %s\n", Statement );
					ErrorCount++;
				}
				else
				{
					UpdateCount++;
				}
			}
			else if ( Debug )
			{
				printf ( "%s\n", Statement );
			}
		}
		else if ( Debug )
		{
			printf ( "No change to beta\n" );
		}



		break;
	}

	sprintf ( Statement, "update fundamental set" );
	FragCount = 0;

	/*----------------------------------------------------------
		price affects dividend yield
	----------------------------------------------------------*/
	if ( xfundamental.xfdividend > 0.0 )
	{
		NewValue = 100.0 * xfundamental.xfdividend / xstock.xsclose;

		if ( NewValue != xfundamental.xfyield )
		{
			sprintf ( Fragment, "%sFyield = '%.2f'", FragCount ? ", " : " ",  NewValue );
			strcat ( Statement, Fragment );
			FragCount++;
		}
	}

	/*----------------------------------------------------------
		price / eps ttm and fwd affect pe ttm and fwd
	----------------------------------------------------------*/
	if ( xfundamental.xfepsttm != 0.0 )
	{
		NewValue = xstock.xsclose / xfundamental.xfepsttm;

		if ( NewValue != xfundamental.xfpettm )
		{
			sprintf ( Fragment, "%sFpettm = '%.2f'", FragCount ? ", " : " ",  NewValue );
			strcat ( Statement, Fragment );
			FragCount++;
		}
	}

	if ( xfundamental.xfepsfwd != 0.0 )
	{
		NewValue = xstock.xsclose / xfundamental.xfepsfwd;

		if ( NewValue != xfundamental.xfpettm )
		{
			sprintf ( Fragment, "%sFpefwd = '%.2f'", FragCount ? ", " : " ",  NewValue );
			strcat ( Statement, Fragment );
			FragCount++;
		}
	}

	/*----------------------------------------------------------
		price affects price to book
	----------------------------------------------------------*/
	if ( xfundamental.xfbvps != 0.0 )
	{
		NewValue = xstock.xsclose / xfundamental.xfbvps;

		if ( NewValue != xfundamental.xfpb )
		{
			sprintf ( Fragment, "%sFpb = '%.2f'", FragCount ? ", " : " ",  NewValue );
			strcat ( Statement, Fragment );
			FragCount++;
		}
	}

	/*----------------------------------------------------------
		Market Cap is easy
	----------------------------------------------------------*/
	if ( xfundamental.xfshares > 0 )
	{
		NewValue = xstock.xsclose * xfundamental.xfshares;

		if ( NewValue != xfundamental.xfmktcap )
		{
			sprintf ( Fragment, "%sFmktcap = '%lld'", FragCount ? ", " : " ",  (long long) NewValue );
			strcat ( Statement, Fragment );
			FragCount++;
		}
	}

	if ( FragCount )
	{
		sprintf ( Fragment, " where Fticker = '%s'", xstock.xsticker );
		strcat ( Statement, Fragment );

		if ( UpdateDB )
		{
			UpdateCount += dbyUpdate ( "getdata", &MySql, Statement, 0, LogFileName );
		}
		else if ( Debug )
		{
			printf ( "%s\n", Statement );
		}
	}

	StockCount++;

	return ( 0 );
}
