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

if len(sys.argv) < 6:
	print ( "USAGE: FCF.py TICKER PRICE CIK FRAME UNIXTIME" )
	#fixit
	quit()
	
	ticker = 'BA'
	price = 195.00
	cik = '0000012927'
	
	ticker = 'IBM'
	price = 158.00
	cik = '0000051143'

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

#fixit
Debug = 0
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
def PrintLastTen ( df ):
	Length = len(df)
	if Length >= 15:
		print(df.iloc[Length-15:Length].to_string())
	else:
		print(df.to_string())

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
df = GetDataList ( FieldList )
if Debug:
	print ( f'{ticker} NetCashProvidedByUsedInOperatingActivities' )
	PrintLastTen ( df )
NetCash = GetLatesetFiscalYear(df)

FieldList = { 'PaymentsToAcquirePropertyPlantAndEquipment' }
df = GetDataList ( FieldList )
if Debug:
	print ( f'{ticker} PaymentsToAcquirePropertyPlantAndEquipment' )
	PrintLastTen ( df )
CapEx = GetLatesetFiscalYear(df)

FreeCashFlow = NetCash - CapEx;

print ( f'-- {ticker} NetCash {NetCash} - CapEx {CapEx} = FreeCashFlow {FreeCashFlow}')
if FreeCashFlow != 0:
	print ( f"update fundamental set Ffreecash = {FreeCashFlow}, Fupdated = {systime} where Fticker = '{ticker}';" );




