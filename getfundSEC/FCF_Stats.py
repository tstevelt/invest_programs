#!/usr/bin/python3
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

import sys
import os
from datetime import datetime
import requests
import pandas as pd
import numpy as np
#import scipy as stats
from scipy.stats import linregress

Debug = 0

# 1 = update
# 2 = chart
# 3 = csv
# 4 = 10 year projected
Format = 1

if len(sys.argv) < 6:
	print ( "USAGE: FCF.py TICKER PRICE CIK FRAME UNIXTIME" )
	if Debug == 0:
		quit()
	
	ticker = 'BA'
	price = 195.00
	cik = '0000012927'
	
	ticker = 'IBM'
	price = 158.00
	cik = '0000051143'

	ticker = 'KVUE'
	price = 21.5
	cik = '0001944048'

	frame = 'CY1970Q'
	systime = 1699636971
else:
	ticker = sys.argv[1:]
	ticker = ticker[0]
	price = sys.argv[2:]
	price = float(price[0])
	cik = sys.argv[3:]
	cik = cik[0]
	frame = sys.argv[4:]
	frame = frame[0]
	systime = sys.argv[5:]
	systime = systime[0]

if Debug:
	print ( '-- ', ticker, price, cik, frame, systime)

# create request header
# get company facts data
headers = {'User-Agent': "tstevelt@silverhammersoftware.com"}
try:
	companyFacts = requests.get( f'https://data.sec.gov/api/xbrl/companyfacts/CIK{cik}.json', headers=headers )
except:
	print ( f'-- {ticker} {cik} no companyFacts' )
	quit()

if companyFacts.status_code != 200:
	print ( f'-- Stock {ticker} {cik} bad URL, status_code', companyFacts.status_code )
	print ( f'-- https://data.sec.gov/api/xbrl/companyfacts/CIK{cik}.json' )
	quit()
	
##
## Start up functions
##
def FormatOne ( FieldList ):
	for Field in FieldList:
		try:
			ndx = len(companyFacts.json()['facts']['dei'][f'{Field}']['units']['shares']) - 1
			Shares = companyFacts.json()['facts']['dei'][f'{Field}']['units']['shares'][ndx]['val']
			FiledDate = companyFacts.json()['facts']['dei'][f'{Field}']['units']['shares'][ndx]['filed']
			try:
				Frame = companyFacts.json()['facts']['dei'][f'{Field}']['units']['shares'][ndx]['frame']
				if Debug:
					print ( f'-- Format One: Found shares at {Field}' )
			except:
				Frame = "none"
			return Shares, Frame, FiledDate
		except:
			continue
	return 0, "none", "none"

def FormatTwo ( FieldList ):
	for Field in FieldList:
		try:
			ndx = len(companyFacts.json()['facts']['dei'][f'{Field}']['units']['USD']) - 1
			Shares = companyFacts.json()['facts']['dei'][f'{Field}']['units']['USD'][ndx]['val']
			FiledDate = companyFacts.json()['facts']['dei'][f'{Field}']['units']['USD'][ndx]['filed']
			try:
				Frame = companyFacts.json()['facts']['dei'][f'{Field}']['units']['USD'][ndx]['frame']
				if Debug:
					print ( f'-- Format Two: Found shares at {Field}' )
			except:
				Frame = "none"
			return Shares, Frame, FiledDate
		except:
			continue
	return 0, "none", "none"

def FormatThree ( FieldList, SubField ):
	for Field in FieldList:
		try:
			ndx = len(companyFacts.json()['facts']['us-gaap'][f'{Field}']['units'][f'{SubField}']) - 1
			Shares = companyFacts.json()['facts']['us-gaap'][f'{Field}']['units'][f'{SubField}'][ndx]['val']
			FiledDate = companyFacts.json()['facts']['us-gaap'][f'{Field}']['units'][f'{SubField}'][ndx]['filed']
			try:
				Frame = companyFacts.json()['facts']['us-gaap'][f'{Field}']['units'][f'{SubField}'][ndx]['frame']
				if Debug:
					print ( f'-- Format Three: Found shares at {Field}' )
			except:
				Frame = "none"
			return Shares, Frame, FiledDate
		except:
			continue
	return 0, "none", "none"

def days_between(d1, d2):
	d1 = datetime.strptime(d1, "%Y-%m-%d")
	d2 = datetime.strptime(d2, "%Y-%m-%d")
	return abs((d2 - d1).days)

##
## Start up -- get filed date and shares
##
FieldList = { 'EntityCommonStockSharesOutstanding', 'EntityPublicFloat', 'CommonStockSharesOutstanding', 'CommonStockSharesIssued', 'NumberOfSharesOutstanding', 'WeightedAverageNumberOfSharesOutstandingBasic' }
Shares, Frame, FiledDate = FormatOne ( FieldList )

if Frame == 'none':
	Shares, Frame, FiledDate = FormatTwo ( FieldList )

if Frame == 'none':
	Shares, Frame, FiledDate = FormatThree ( FieldList, 'shares' )

if Frame == 'none':
	print ( f'-- {ticker} unrecognizable json file' )
	print ( f'-- https://data.sec.gov/api/xbrl/companyfacts/CIK{cik}.json' )
	quit()
	
CurrentDate = datetime.today().strftime('%Y-%m-%d')

try:
	Days = days_between ( FiledDate, CurrentDate )
except:
	print ( f'-- {ticker} days_between failed' )
	print ( f'-- https://data.sec.gov/api/xbrl/companyfacts/CIK{cik}.json' )
	quit()

if Days < 100 and frame == Frame[0:8]:
    print ( f'-- No new filing for {ticker}' )
    quit ()
    
if frame > Frame[0:8]:
	if Debug:
		print ( f'-- Wacky {ticker} {frame} > {Frame}' )
    #quit ()
	Frame = frame

if Debug:
	print ( f'-- {ticker} Shares {Shares}, {Frame}, {FiledDate}' )

if Days > 100:
	print ( f'-- {ticker} processing old filed date {FiledDate}' )
	
##
## Functions for getting other data fields
##
def GetDataList ( Fields ):
	for Field in Fields:
		try:
			data = companyFacts.json()['facts']['us-gaap'][f'{Field}']['units']['USD']
			df = pd.DataFrame.from_dict(data)
			df = df.dropna()	
			if Debug:
				print ( f'-- Get Data List: found {Field} in USD' )
			return( df )
		except:
			try:
				data = companyFacts.json()['facts']['us-gaap'][f'{Field}']['units']['USD/shares']
				df = pd.DataFrame.from_dict(data)
				df = df.dropna()	
				if Debug:
					print ( f'-- Get Data List: found {Field} in USD/shares' )
				return( df )
			except:
				continue
	return pd.DataFrame()

def GetLatesetFiscalYear(df):
	Number = 0
	if df.empty:
		return Number
	Length = len(df)
	ndx = Length - 1
	while ndx >= 0:
		if 'fp' in df.columns and df.iloc[ndx]['fp'] == 'FY':
			Number = int(df.iloc[ndx]['val'])
			return Number
		ndx -= 1
	return Number

def MakeArray ( dfNetCash, dfCapEx ):
	Numbers = []
	if dfNetCash.empty:
		return Numbers
	lenNetCash = len(dfNetCash)
	ndx = 0
	xo = -1
	if dfCapEx.empty:
		lenCapEx = 0
	else:
		lenCapEx = len(dfCapEx)

	while ndx < lenNetCash:
		Numbers.append(dfNetCash.iloc[ndx]['val'])
		xo = xo + 1
		ThisDate = (dfNetCash.iloc[ndx]['end'])
		if not dfCapEx.empty:
			for xc in range(lenCapEx):
				if ThisDate == dfCapEx.iloc[xc]['end']:
					Numbers[xo] = Numbers[xo] - dfCapEx.iloc[xc]['val']
					break
		ndx = ndx + 1
	return Numbers

## 
## Get data fields
##

## 
## Free Cash Flow is commonly calculated as:
## 
## FCF=NetCashfromOperatingActivities−CapitalExpendituresFCF=NetCashfromOperatingActivities−CapitalExpenditures
## 
## Here are some XBRL tags that you might use:
## 
##     Net Cash Provided by Operating Activities:
##         XBRL Tag: us-gaap:NetCashProvidedByUsedInOperatingActivities
## 
##     Capital Expenditures (Capital Expenditures are typically part of Investing Activities):
##         XBRL Tag: us-gaap:PaymentsToAcquirePropertyPlantAndEquipmen/
## 
	
FieldList = { 'NetCashProvidedByUsedInOperatingActivities' }
dfNetCash = GetDataList ( FieldList )

if Debug:
	print ( f'{ticker} NetCashProvidedByUsedInOperatingActivities' )
	print ( dfNetCash.to_string() )

if not dfNetCash.empty:
	dfNetCash = dfNetCash.drop(dfNetCash[dfNetCash.fp != 'FY'].index)
else:
	print ( f'-- {ticker} missing NetCashProvidedByUsedInOperatingActivities' )
	quit()

NetCash = GetLatesetFiscalYear(dfNetCash)

FieldList = { 'PaymentsToAcquirePropertyPlantAndEquipment' }
dfCapEx = GetDataList ( FieldList )
if not dfCapEx.empty:
	dfCapEx = dfCapEx.drop(dfCapEx[dfCapEx.fp != 'FY'].index)
if Debug:
	print ( f'{ticker} PaymentsToAcquirePropertyPlantAndEquipment' )
	print ( dfCapEx.to_string() )
CapEx = GetLatesetFiscalYear(dfCapEx)

FreeCashFlow = NetCash - CapEx;
if Format == 2:
	print ( f'-- {ticker} NetCash {NetCash} - CapEx {CapEx} = FreeCashFlow {FreeCashFlow}')

FCF_Array = MakeArray ( dfNetCash, dfCapEx )

if len(FCF_Array) == 0:
	quit()

arr = np.array(FCF_Array)

xl = len(arr)
slope, intercept, r, p, std_err = 0.0, 0.0, 0.0, 0.0, 0.0
tangent = 0.0
MEAN = 0.0
CV = 0
if xl > 6:
	try:
		SD = np.std(FCF_Array)
		MEAN = np.mean(FCF_Array)
		if MEAN != 0:
			CV = SD / MEAN
		years = []
		value = 1
		for i in range(xl):
			years.append(value)
			value = value + 1
		slope, intercept, r, p, std_err = linregress(years, arr)
		if intercept != 0:
			tangent = slope / intercept
	except:
		slope, intercept, r, p, std_err = 0.0, 0.0, 0.0, 0.0, 0.0

if Format == 1 and FreeCashFlow != 0:
	tangent = tangent * 100.0
	if pd.isna(tangent) or np.isinf(tangent):
		tangent = 0.0;
	if pd.isna(CV) or np.isinf(CV):
		CV = 0.0;
	print ( f"update fundamental set Ffreecash = {FreeCashFlow}, Ffcfgrow = {tangent:.2f}, Ffcfcv = {CV:.2f}, Fupdated = {systime} where Fticker = '{ticker}';" );

if Format == 2:
	print ( f'-- {ticker} StdDev {SD:.0f}  Mean {MEAN:.0f}   CV {CV:.3f}' )
	print ( f'-- {ticker} slope {slope}, intercept {intercept}, r {r}, p {p}, std_err {std_err}' )
	x = 0
	for i in range(xl):
		x = years[i]
		y = intercept + slope * x
		print ( arr[i], ',',  y )
		
	for i in range(10):
		y = intercept + slope * (x + i + 1)
		print ( ' ', ',',  y )
	
if Format == 3 and xl > 8:
	print ( f'{ticker}, {xl}, {CV:.3f}, {slope:.0f}, {intercept:.0f}, {tangent:.3f}, {FreeCashFlow}' )

if Format == 4 and xl > 0:
	x = xl
	y0 = intercept + slope * (x)
	x = x + 1
	y1 = intercept + slope * (x)
	x = x + 1
	y2 = intercept + slope * (x)
	x = x + 1
	y3 = intercept + slope * (x)
	x = x + 1
	y4 = intercept + slope * (x)
	x = x + 1
	y5 = intercept + slope * (x)
	x = x + 1
	y6 = intercept + slope * (x)
	x = x + 1
	y7 = intercept + slope * (x)
	x = x + 1
	y8 = intercept + slope * (x)
	x = x + 1
	y9 = intercept + slope * (x)
	print ( f'{ticker}, {y0:.0f}, {y1:.0f}, {y2:.0f}, {y3:.0f}, {y4:.0f}, {y5:.0f}, {y6:.0f}, {y7:.0f}, {y8:.0f}, {y9:.0f}' )

