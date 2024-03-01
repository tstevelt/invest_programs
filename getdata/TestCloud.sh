#!/bin/sh
#     Programs called by invest.cgi
# 
#     Copyright (C)  2019 - 2024 Tom Stevelt
# 
#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU Affero General Public License as
#     published by the Free Software Foundation, either version 3 of the
#     License, or (at your option) any later version.
# 
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU Affero General Public License for more details.
# 
#     You should have received a copy of the GNU Affero General Public License
#     along with this program.  If not, see <https://www.gnu.org/licenses/>.

#
#	I signed up for Beta Testing on IEX Cloud
#	https://iexcloud.io/?referrerCode=236283
#

if [ "$1" = '' ]
then
	echo "USAGE: TestCloud.sh {T|R} TICKER {options company companyCSV funval quote TOPS previous news+date splits div1mo div1yr div5yr divnext 10yr 5yr 2yr 1mo {{date|minute'} yyyymmdd} intraday ohlc book financials qtr stats}"
	echo "USAGE: TestCloud.sh X daily"
	echo "USAGE: TestCloud.sh X sectors"
	echo "USAGE: TestCloud.sh X symbols"
	echo "USAGE: TestCloud.sh X exchanges"
	echo "USAGE: TestCloud.sh X volume"
	echo "USAGE: TestCloud.sh X story link"
	exit 1 
fi

case "$1" in
	'R' )
		# live
		BASEURL='https://cloud.iexapis.com/stable'
		TOKEN='pk_d5fe7ef227584b168b60c0d54d0ba18f'
		;;
	
	'T' )
		# test
		BASEURL='https://sandbox.iexapis.com/beta'
		TOKEN='Tpk_d9b468e630a611e9958142010a80043c'
		;;

	'X' )
		BASEURL='https://cloud.iexapis.com/stable'
		TOKEN='pk_d5fe7ef227584b168b60c0d54d0ba18f'
		;;

	* )
		echo "Unknown first arg, use T for TEST, R for REAL"
		exit 1
		;;
esac


case "$2" in 
	'daily' )
		DATE=20190124
		echo "==== DAILY LIST $DATE ===="
		curl -s "$BASEURL/ref-data/daily-list/symbol-directory/$DATE?format=csv"
		echo "==== DIVIDENDS LIST $DATE ===="
		curl -s "$BASEURL/ref-data/daily-list/dividends/$DATE?format=csv"
		;;

	'symbols' )
		MONTH=`date +%m`
		FILE=symbols_$MONTH.txt
#		echo $FILE
#		curl -s "$BASEURL/ref-data/symbols/?format=csv&token=$TOKEN" -o $FILE
		curl -s "$BASEURL/ref-data/iex/symbols/?format=csv&token=$TOKEN" -o $FILE
		wc -l $FILE
		;;

	'sectors' )
		curl -s "$BASEURL/ref-data/sectors?token=$TOKEN" 
		;;

	'exchanges' )
		curl -s "$BASEURL/ref-data/market/us/exchanges?token=$TOKEN"  | JsonTree -
		;;
	
	'volume' )
		#curl -s "$BASEURL/stock/market/volume?token=$TOKEN"  | JsonTree -
		curl -s "$BASEURL/stock/market/volume?format=csv&token=$TOKEN"
		;;
	
	'story' )
		curl -s "$3?token=$TOKEN"
		;;

	* ) case "$3" in
			'company' )
				# curl -s "$BASEURL/stock/$2/company?format=json&token=$TOKEN" | jsondump
				# !!! NOTE !!! UpdateCompany process counting on follow sed command 
				curl -s "$BASEURL/stock/$2/company?format=json&token=$TOKEN"  | sed 's/,/\n/g'
				# curl -s "$BASEURL/stock/$2/company?format=json&token=$TOKEN"  | JsonTree -
				;;

			'companyCSV' )
				curl -s "$BASEURL/stock/$2/company?format=csv&token=$TOKEN"
				;;

			'funval' )
				#curl -s "$BASEURL/time-series/FUNDAMENTAL_VALUATIONS/$2?token=$TOKEN" |  JsonTree -
				curl -s "$BASEURL/time-series/FUNDAMENTAL_VALUATIONS/$2?token=$TOKEN" |  JsonTree -
				echo ""
				echo "---------"
				;;

			'quote' )
				echo "quote"
				#curl -s "$BASEURL/stock/$2/quote?format=json&token=$TOKEN" | jsondump 
				#curl -s "$BASEURL/stock/$2/quote?format=json&token=$TOKEN" | jsondump
				#curl -s "$BASEURL/stock/$2/quote?format=json&token=$TOKEN" |  JsonTree -
				curl -s "$BASEURL/stock/$2/quote?format=json&token=$TOKEN" |  sed 's/,"/,"\n/g'
				echo ""
				echo "---------"

				echo "latest"
				#curl -s "$BASEURL/stock/$2/quote?format=csv&token=$TOKEN" 
				#echo ""
				#curl -s "$BASEURL/stock/$2/quote?format=csv&token=$TOKEN"
				curl -s "$BASEURL/stock/$2/quote/latestPrice?token=$TOKEN"
				echo ""
				;;

			'TOPS' )
				# curl -s "$BASEURL/data/CORE/IEX_TOPS/$2?format=csv&token=$TOKEN"
				# curl -s "$BASEURL/data/CORE/IEX_TOPS/$2?token=$TOKEN" | JsonTree - 
				curl -s "$BASEURL/data/CORE/IEX_TOPS/$2?token=$TOKEN" | sed 's/,"/,"\n/g'
				;;

			'options' )
				curl -s "$BASEURL/stock/$2/options?token=$TOKEN" 
				exit 0
				curl -s "$BASEURL/stock/$2/options?token=$TOKEN" |  JsonTree - | head -1 | sed 's/ //g'
				NEXT_EXPIRE=`curl -s "$BASEURL/stock/$2/options?token=$TOKEN" |  JsonTree - | head -1 | sed 's/ //g'`
				echo "NEXT_EXPIRE $NEXT_EXPIRE"
				curl -s "$BASEURL/stock/$2/options/$NEXT_EXPIRE?token=$TOKEN"  | sed 's/}/\n/g' | tee x1
				;;

			'previous' )
				curl -s "$BASEURL/stock/$2/previous?filter=date,open,high,low,close,volume&format=csv&token=$TOKEN"
				;;
			'ohlc' )
				curl -s "$BASEURL/stock/$2/ohlc?token=$TOKEN" | JsonTree -
				;;
			'splits' )
				curl -s "$BASEURL/stock/$2/splits/2y?token=$TOKEN" | JsonTree -
				;;
			'news' )
				#curl -s "$BASEURL/stock/$2/news/last/1?token=$TOKEN" | JsonTree -

				#echo "xxx"
				curl -s "$BASEURL/time-series/news/$2?from=$4&sort=DESC&token=$TOKEN" | JsonTree -

				#echo "xxx"
				#curl -s "$BASEURL/time-series/news/$2?from=$4&last=1&sort=DESC&token=$TOKEN" | JsonTree - | grep datetime

				#curl -s "$BASEURL/time-series/news/$2?from=$4&limit=3&token=$TOKEN" | JsonTree -

				#curl -s "$BASEURL/time-series/news/$2?from=$4&subattribute=datetime~1627048371000&limit=1&token=$TOKEN" | JsonTree -
				;;
			'div1mo' )
				curl -s "$BASEURL/stock/$2/dividends/1m?format=csv&token=$TOKEN"
				;;
			'div1yr' )
				curl -s "$BASEURL/stock/$2/dividends/1y?format=csv&token=$TOKEN"
				;;
			'div5yr' )
				curl -s "$BASEURL/stock/$2/dividends/5y?format=csv&token=$TOKEN"
				# curl -s "$BASEURL/stock/$2/dividends/from=2000-01-01&limit=500?format=csv&token=$TOKEN"
				# curl -s "$BASEURL/stock/$2/dividends/range=20y&limit=500?format=csv&token=$TOKEN"
				;;
			'divnext' )
				curl -s "$BASEURL/stock/$2/dividends/next?format=csv&token=$TOKEN"
				;;
			'10yr' )
				curl -s "$BASEURL/stock/$2/chart/10y?format=csv&token=$TOKEN"
				;;
			'5yr' )
				curl -s "$BASEURL/stock/$2/chart/5y?format=csv&token=$TOKEN"
				;;
			'2yr' )
				curl -s "$BASEURL/stock/$2/chart/2y?format=csv&token=$TOKEN"
				;;
			'1mo' )
				curl -s "$BASEURL/stock/$2/chart/1m?filter=date,open,high,low,close,volume&format=csv&token=$TOKEN"
				;;
			'date' )
				# curl -s "$BASEURL/stock/$2/chart/date/$4?chartByDay=true&token=$TOKEN" | JsonTree -
				echo    "$BASEURL/stock/$2/chart/date/$4?chartByDay=true&token=$TOKEN&format=csv" 
				curl -s "$BASEURL/stock/$2/chart/date/$4?chartByDay=true&token=$TOKEN&format=csv" 
				;;
			'minute' )
				# curl -s "$BASEURL/stock/$2/chart/date/$4?token=$TOKEN" | JsonTree -
				# curl -s "$BASEURL/stock/$2/chart/date/$4?token=$TOKEN&format=csv"
				curl -s "$BASEURL/stock/$2/chart/date/$4?token=$TOKEN&format=csv&filter=symbol,date,minute,open,high,low,close,volume"
#AWS </var/local/invest>$ TestCloud.sh R SPY minute 20210114 
#date,minute,high,low,symbol
#2021-01-14,09:30,380.79,380.56,SPY
#2021-01-14,09:31,380.64,380.48,SPY
#2021-01-14,09:32,380.58,380.485,SPY
#2021-01-14,09:33,380.51,380.44,SPY
#2021-01-14,09:34,380.45,380.29,SPY
				;;
			'intraday' )
				echo    "$BASEURL/data/CORE/INTRADAY_PRICES/$2?filter=marketHigh,marketLow,marketVolume,minute&sort=DESC&format=csv&token=$TOKEN" 
				curl -s "$BASEURL/data/CORE/INTRADAY_PRICES/$2?filter=marketHigh,marketLow,marketVolume,minute&sort=DESC&format=csv&token=$TOKEN" | head -20
				;;
			'book' )
				curl -s "$BASEURL/stock/$2/book?token=$TOKEN" | JsonTree -
				;;
			'financials' )
				curl -s "$BASEURL/stock/$2/financials?period=annual&token=$TOKEN" | JsonTree -
				;;
			'qtr' )
				echo "1. TOO EXPENSIVE!!!"
				echo "2. NO ETF!!!"
				exit 1
curl -s "https://api.iex.cloud/v1/data/core/fundamentals/$2/quarterly?limit=1&subattribute=fiscalQuarter|3,fiscalYear|2023&token=$TOKEN" > $2_qtr_3_2023.json
# https://api.iex.cloud/v1/data/core/fundamentals/tsla/quarterly?limit=1&subattribute=fiscalQuarter%7C3,fiscalYear%7C2020&token=pk_d5fe7ef227584b168b60c0d54d0ba18f

				;;
			'stats' )
				# curl -s "$BASEURL/stock/$2/stats?token=$TOKEN" | JsonTree -
				# curl -s "$BASEURL/stock/$2/stats?token=$TOKEN"  | JsonTree - -print
				# curl -s "$BASEURL/stock/$2/stats?token=$TOKEN" 
				curl -s "$BASEURL/stock/$2/stats?token=$TOKEN"  | sed 's/,"/\n"/g' | sed 's/:/\t/g'
				;;
			* )
				echo "Unknown option $3"
				exit 1
				;;
		esac
		;;
esac

echo ""
