
## Database Loading

These programs are generally called from the command line (or crontab)
Also called occasionally from invest.cgi for one individual stock.

-------------------------------------------------------------------------------
```
USAGE: getdata -all        [options]
USAGE: getdata -null       [options]
USAGE: getdata -one TICKER [options]
USAGE: getdata -splits
 -all    = all stocks where stock.Slast in past 30 days.
 -null   = where Slast is NULL
 -one    = one stock, regardless of stock.Slast.
 -splits = check all stocks for splits.
Options:
 -fmt x     = csv or json, default is csv
 period     = default is previous day
              others are -ohlc, cannot use csv format
              -1mo -2yr -3yr -5yr -10yr
              -past yyyymmdd
 -flag file = create file if Missing Benchmark, Portfolio or too many SP500 data.
 -subj file = create email subject file.
 -slast     = ignore stock.Slast.  Use this to load old data or reverse file.
 -tiingo    = use Tiingo instead of IEX. Hard coded to Slast. Not with -all.
 -d     = debug
 -q     = quiet

```
-------------------------------------------------------------------------------
```
USAGE: getdiv -active        [options]
USAGE: getdiv -cuts          [options]
USAGE: getdiv -update        [options]
USAGE: getdiv -ticker TICKER [options]
USAGE: getdiv -member ID     [options]
 -active = stocks in portfolios
 -cuts   = check all portfolios for last dividend cuts.  No DB update.
 -update = any stock where dividend count > 0 and Slast current
 -ticker = one stock
 -member = one member
Options:
 -d1  = debug
 -d2  = debug, more verbose
 -all = get all dividends for each stock
```
-------------------------------------------------------------------------------
```
USAGE: getfundSEC index          [options]
USAGE: getfundSEC -file filename [options]
USAGE: getfundSEC -ticker ticker [options]
USAGE: getfundSEC -old [-delete]
 S = S&P 500
 D = Dow Jones Industrials
 N = Nasdaq 100
 M = Midcap (Russell 1000 ex S&P 500)
 1 = Russell 1000
 2 = Russell 2000
 3 = Russell 3000
 O = Other (not in any index)
 A = All stocks
 E = Only ETF
Options:
 -script file = use file instead of getfundSEC.py
 -ignore      = passes ancient Fframe CY197001
 -v           = verbose
 -nodb        = no db update, statements to stdout
 -d [#]       = debug, no db update
```
---------------------------------------------------------------------------
getsplits Scan stock history for price jumps, verify with IEX /splits/
```
USAGE: getsplits -all           [options]
USAGE: getsplits -active        [options]
USAGE: getsplits -member ID     [options]
USAGE: getsplits -ticker TICKER [options]
 -all    = all stocks
 -active = stocks in all portfolios
 -member = stocks in one member portfolio
 -ticker = one stock
Options:
-range x  = range to retrieve. default 2w
-d1       = debug
-d2       = debug, more verbose
```
-------------------------------------------------------------------------------
```
USAGE: getstats -all         [options]
USAGE: getstats -indexed     [options]
USAGE: getstats -index INDEX [options]
USAGE: getstats -one TICKER  [options]
USAGE: getstats -report      [options]
 -all     = all stocks (about 2000+ at IEX have zero shares).
 -indexed = all stocks in one or more index.
 -index   = stocks in index.
 -one     = one stock.
 -report = print report after data updated.
Options:
 -d     = debug
 -q     = quiet
 -neg   = skip negative earnings in -report.
```

## Update database after loading data
```
USAGE: Fundamentals {-g|-p|-r|-c} TICKER [options]
 -g get from iex, saved as raw
 -p parse from raw to txt
 -r write txt report
 -c write csv format
 -f filter
 -d debug
```
-------------------------------------------------------------------------------
```
USAGE: UpdateAverage -index index   mode [options]
USAGE: UpdateAverage -ticker TICKER mode [options]
index:
 S = S&P 500
 D = Dow Jones Industrials
 N = Nasdaq 100
 M = Midcap (Russell 1000 ex S&P 500)
 1 = Russell 1000
 2 = Russell 2000
 3 = Russell 3000
 A = All stocks
 O = Not indexed
  mode = A for all, Z for zeros
options:
  -col xxx   = only one column {price|volume|rsi|stddev|ctb}
  -p         = pacifier
  -d1 or -d2 = debug level, no db update
```
-------------------------------------------------------------------------------
```
USAGE: UpdateFundamental -all           [options]
USAGE: UpdateFundamental -file filename [options]
USAGE: UpdateFundamental -ticker ticker [options]
Options:
 -nodb   = no db update, statements to stdout
 -v      = verbose
 -d [#]  = debug, no db update
```
---------------------------------------------------------------------------
cleandata Check database for weird data.
```
USAGE: cleandata -stock  date [-d]
   Deletes stocks and history where Slast < date (if not in portfolio)
USAGE: cleandata -history {equal|less} date [-d]
   Deletes history and average records
USAGE: cleandata -report {all | TICKER} date [-d]
   date = history should be loaded up to and including this date. *
format date as yyyy-mm-dd
```
* Report option disabled, too expensive.

## Display data on the website.

USAGE: portdtl -port   ID
       portdtl -member memberID Ticker

-------------------------------------------------------------------------------
fminmax.cgi
Synopsis: The ETF and Equity fundamental screeners can request 

-------------------------------------------------------------------------------
fundamentals.cgi
Synopsis: display fundamentals for one stock

-------------------------------------------------------------------------------
pupld.cgi
Synopsis: Called by website Import Portfolio screen.

-------------------------------------------------------------------------------
wlupld.cgi
Synopsis: Called by website Import Watchlist screen.

