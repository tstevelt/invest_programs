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

Debug = 0

if len(sys.argv) < 6:
	print ( "USAGE: getfundSEC.py TICKER PRICE CIK FRAME UNIXTIME" )
	if not Debug:
		quit()
	
	ticker = 'IBM'
	price = 158.00
	cik = '0000051143'

	ticker = 'SKW'
	price = 94.0
	cik = '0000093556'

	ticker = 'BA'
	price = 195.00
	cik = '0000012927'
	
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
					print ( f'-- FormatOne: Found shares at {Field}' )
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
					print ( f'-- FormatTwo: Found shares at {Field}' )
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
					print ( f'-- FormatThree: Found shares at {Field}' )
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
	if Length >= 10:
		print(df.iloc[Length-10:Length])
	else:
		print(df)

SubFields = { 'USD', 'USD/shares', 'shares' }
def GetData ( Field ):
	for SubField in SubFields:
		try:
			data = companyFacts.json()['facts']['us-gaap'][f'{Field}']['units'][f'{SubField}']
			df = pd.DataFrame.from_dict(data)
			df = df.dropna()	
			return( df )
		except:
			continue
	return pd.DataFrame()

def GetDataList ( Fields ):
	for Field in Fields:
		try:
			data = companyFacts.json()['facts']['us-gaap'][f'{Field}']['units']['USD']
			df = pd.DataFrame.from_dict(data)
			df = df.dropna()	
			if Debug:
				print ( f'-- GetDataList: found {Field} in USD' )
			return( df )
		except:
			try:
				data = companyFacts.json()['facts']['us-gaap'][f'{Field}']['units']['USD/shares']
				df = pd.DataFrame.from_dict(data)
				df = df.dropna()	
				if Debug:
					print ( f'-- GetDataList: found {Field} in USD/shares' )
				return( df )
			except:
				continue
	return pd.DataFrame()

def GetValueTTM ( df ):
	if df.empty:
		return 0
	try:
		Length = len(df)
		if Length < 7:
			return 0
	except:
		return 0
	ndx = Length - 1
	Value = 0
	if df.iloc[ndx]['fp'] == 'FY':
		Value = df.iloc[ndx]['val']
	elif df.iloc[ndx]['fp'] == 'Q1':
		for x in range(ndx-1,Length):
			Value += df.iloc[x]['val']
		Value -= df.iloc[ndx-4]['val']
	elif df.iloc[ndx]['fp'] == 'Q2':
		for x in range(ndx-2,Length):
			Value += df.iloc[x]['val']
		Value -= df.iloc[ndx-4]['val']
		Value -= df.iloc[ndx-5]['val']
	elif df.iloc[ndx]['fp'] == 'Q3':
		for x in range(ndx-3,Length):
			Value += df.iloc[x]['val']
		Value -= df.iloc[ndx-4]['val']
		Value -= df.iloc[ndx-5]['val']
		Value -= df.iloc[ndx-6]['val']
	return Value

def GetValueLastFourQtr ( df ):
	if df.empty:
		return 0
	try:
		Length = len(df)
		if Length < 10:
			return 0
	except:
		return 0
	Value = 0
	Count = 0
	ndx = Length - 1
	try:
		while Count < 4 and ndx >= 0:
			if df.iloc[ndx]['fp'] == 'Q1' or df.iloc[ndx]['fp'] == 'Q2' or df.iloc[ndx]['fp'] == 'Q3' or df.iloc[ndx]['fp'] == 'Q4':
				Value += df.iloc[ndx]['val']
				Count += 1
			ndx -= 1
	except:
		print  ( f'GetValueLastFourQtr failed on {ticker}, length {Length}' );
	return Value

def GetValueLast ( df ):
	if df.empty:
		return 0
	try:
		Length = len(df)
	except:
		return 0
	Value = df.iloc[Length-1]['val']
	return Value

def GetQuarters(df):
	Numbers = []
	if df.empty:
		return Numbers
	Length = len(df)
	if Length < 4:
		return Numbers
	ndx = Length - 1
	while ndx >=4:
		if df.iloc[ndx]['fp'] == 'FY':
			QtrValue = df.iloc[ndx]['val'] - (df.iloc[ndx-1]['val'] + df.iloc[ndx-2]['val']  + df.iloc[ndx-3]['val'])
			Numbers.append(int(QtrValue))
		else:
			Numbers.append(df.iloc[ndx]['val'])
		ndx = ndx - 1
	return Numbers

def CalcYoYPercent (Numbers):
	Length = len(Numbers)
	if Length < 5:
		return 0.0
	if Numbers[4] <= 0 or Numbers[0] <= 0:
		return (0.0)
	Answer = 100.0 * (Numbers[0] - Numbers[4]) / Numbers[4]
	return Answer
	
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
if Shares < 1000:
	df = GetData('CommonStockSharesIssued')
	if Debug > 1:
		print ( df )
	Shares = GetValueLast ( df )
	
df = GetDataList ( {'EarningsPerShareDiluted','EarningsPerShareBasic'} )
if Debug > 1:
	PrintLastTen ( df )
EPS = GetValueTTM ( df )

MyArray = GetQuarters ( df )
EarnGrowPct = CalcYoYPercent(MyArray)
if Debug:
	print ( f'MyArray {MyArray}')
	print ( f'EarnGrowPct {EarnGrowPct}')

FieldList = { 'Revenues', 'RevenuesNetOfInterestExpense',  'RevenueFromContractWithCustomerExcludingAssessedTax' }
df = GetDataList ( FieldList )
if Debug > 1:
	PrintLastTen ( df )
Revenue = GetValueTTM ( df )
if Revenue <= 0:
	Revenue = GetValueLastFourQtr ( df )

MyArray = GetQuarters ( df )
RevenueGrowPct = CalcYoYPercent(MyArray)
if Debug:
	print ( f'MyArray {MyArray}')
	print ( f'RevenueGrowPct {RevenueGrowPct}')

df = GetData ( 'Assets' )
Assets = GetValueLast ( df )

df = GetDataList({'StockholdersEquity','StockholdersEquityIncludingPortionAttributableToNoncontrollingInterest'})
StockholdersEquity = GetValueLast(df)

df = GetData('LiabilitiesAndStockholdersEquity')
LiabilitiesAndStockholdersEquity = GetValueLast(df)

df = GetData('LongTermDebt')
LongTermDebt = GetValueLast(df)

# df = GetData('IncomeLossFromContinuingOperations')
#df = GetData('OperatingIncomeLoss')
#Earnings = GetValueLast ( df )

df = GetData('InterestExpense')
Interest = GetValueTTM ( df )

FieldList = { 'IncomeTaxesPaid', 'IncomeTaxesPaidNet' }
df = GetDataList(FieldList)
Taxes  = GetValueTTM ( df )

FieldList = { 'Depreciation', 'DepreciationAndAmortization', 'NetIncomeLossFromContinuingOperationsAvailableToCommonShareholdersBasic' }
df = GetDataList(FieldList)
Depreciation = GetValueTTM ( df )

df = GetData ( 'AmortizationOfFinancingCosts' )
AmortFinance = GetValueTTM ( df )

df = GetData ( 'AmortizationOfIntangibleAssets' )
AmortIntangible = GetValueTTM ( df )

df = GetData('MinorityInterest')
MinorityInterest = GetValueLast ( df )

df = GetData('LongTermDebtNoncurrent')
LongTermDebtNoncurrent = GetValueLast ( df )

df = GetData('OperatingLeaseLiabilityCurrent')
OperatingLeaseLiabilityCurrent = GetValueLast ( df )

df = GetData('AccountsPayableCurrent')
AccountsPayableCurrent = GetValueLast ( df )

df = GetData('CashAndCashEquivalentsAtCarryingValue')
CashEquiv = GetValueLast ( df )

FieldList = { 'NetIncomeLoss', 'NetIncomeLossAvailableToCommonStockholdersDiluted' }
df = GetDataList ( FieldList )
NetIncome = GetValueTTM ( df )

df = GetData ( 'AssetsCurrent' )
AssetsCurrent = GetValueLast ( df )

df = GetData ( 'TreasuryStockShares' )
TreasuryStockShares = GetValueLast ( df )

FieldList = { 'Liabilities', 'LiabilitiesCurrent' }
df = GetDataList ( FieldList )
Liabilities = GetValueLast ( df )

FieldList = { 'NetCashProvidedByUsedInOperatingActivities' }
df = GetDataList ( FieldList )
NetCash = GetLatesetFiscalYear(df)

FieldList = { 'PaymentsToAcquirePropertyPlantAndEquipment' }
df = GetDataList ( FieldList )
CapEx = GetLatesetFiscalYear(df)

if Debug:
	print ( '-- After extract all data' )
	print ( f'-- Shares {Shares}' )
	print ( f'-- EPS {EPS}' )
	print ( f'-- Assets {Assets}' )
	print ( f'-- StockholdersEquity {StockholdersEquity}' )
	print ( f'-- LiabilitiesAndStockholdersEquity {LiabilitiesAndStockholdersEquity}' )
	print ( f'-- LongTermDebt {LongTermDebt}' )
	print ( f'-- LongTermDebtNoncurrent {LongTermDebtNoncurrent}' )
	print ( f'-- Interest {Interest}' )
	print ( f'-- Taxes {Taxes}' )
	print ( f'-- Depreciation {Depreciation}' )
	print ( f'-- AmortFinance {AmortFinance}' )
	print ( f'-- AmortIntangible {AmortIntangible}' )
	print ( f'-- MinorityInterest {MinorityInterest}' )
	print ( f'-- OperatingLeaseLiabilityCurrent {OperatingLeaseLiabilityCurrent}' )
	print ( f'-- AccountsPayableCurrent {AccountsPayableCurrent}' )
	print ( f'-- CashEquiv {CashEquiv}' )
	print ( f'-- NetIncome {NetIncome}' )
	print ( f'-- Revenue {Revenue}' )
	print ( f'-- AssetsCurrent {AssetsCurrent}' )
	print ( f'-- TreasuryStockShares {TreasuryStockShares}' )
	print ( f'-- Liabilities {Liabilities}\n\n' )
	print ( f'-- NetCash {NetCash}' )
	print ( f'-- CapEx {CapEx}' )

##
## Calculations
##
if Debug:
	print ( '-- Calculations' )

if StockholdersEquity == 0:
	StockholdersEquity = LiabilitiesAndStockholdersEquity - Liabilities

Fdebteq = 0.0
if StockholdersEquity > 0:
	Fdebteq = LongTermDebt / StockholdersEquity
	if Debug:
		print ( f'-- Fdebteq {Fdebteq}' )

Earnings = Shares * EPS
if Debug:
	print ( f'-- Earnings     {Earnings}' )

EBIT   = Earnings + Interest + Taxes
EBITDA = EBIT + Depreciation + AmortFinance + AmortIntangible

if Debug:
	print ( f'-- EBIT         {EBIT}' )
	print ( f'-- Depreciation {Depreciation}' )
	print ( f'-- AmortFinance {AmortFinance}' )
	print ( f'-- AmortIntangible {AmortIntangible}' )
	print ( f'-- EBITDA       {EBITDA}' )

MarketCap = Shares * price;

EnterpriseValue = MarketCap + MinorityInterest + LongTermDebtNoncurrent + OperatingLeaseLiabilityCurrent + AccountsPayableCurrent-CashEquiv
if Debug:
	print ( f'-- ticker {MarketCap} + {MinorityInterest} + {LongTermDebtNoncurrent} + {OperatingLeaseLiabilityCurrent} + {AccountsPayableCurrent} + {CashEquiv} = {EnterpriseValue}' )

if EBITDA > 0:
	StockValue = EnterpriseValue / EBITDA
else:
	StockValue = 0.0
	# this is normal for ETF

if Assets > 0:
	ReturnAssets = 100.0 * NetIncome / Assets
else:
	ReturnAssets = 0.0
	print ( f'-- {ticker} No Assets!!!' )

if Revenue > 0:
	GrossMargin = 100.0 * NetIncome / Revenue
else:
	GrossMargin = 0.0
	print ( f'-- {ticker} No Revenue!!!' )


"""
Quick Ratio from https://www.investopedia.com/terms/q/quickratio.asp
"""
TenPercent = Shares * 0.1	
if TreasuryStockShares > TenPercent:
	MarketableSecurities = price * TenPercent 
	if Debug:
		print ( f'-- price {price}  10% Shares {TenPercent} MarketableSecurities {MarketableSecurities}' )
else:
	MarketableSecurities = price * TreasuryStockShares
	if Debug:
		print ( f'-- price {price} TreasuryStockShares {TreasuryStockShares} MarketableSecurities {MarketableSecurities}' )

if Debug:
	print ( f'-- MarketableSecurities', MarketableSecurities)

if Liabilities > 0:
	QuickRatio = ( AssetsCurrent + CashEquiv + MarketableSecurities) / (Liabilities )
else:
	QuickRatio = 0.0
	print ( f'-- {ticker} No Liabilities!!!' )

if LongTermDebtNoncurrent == 0 and LongTermDebt > 0:
	LongTermDebtNoncurrent = LongTermDebt

FreeCashFlow = NetCash - CapEx;

print ( f"update fundamental set Fdebteq = {Fdebteq:.2f}, Ftotasst = {Assets}, FreturnA = {ReturnAssets:.2f}, Fmargin = {GrossMargin:.2f}, Fequity = {StockholdersEquity:.0f}, Febitda = {EBITDA}, Fminority = {MinorityInterest}, FdebtLT = {LongTermDebt}, FdebtNC = {LongTermDebtNoncurrent}, Fliab = {Liabilities}, Fpayables = {AccountsPayableCurrent}, Fcurasst = {AssetsCurrent}, Fcash = {CashEquiv}, Ftreasury = {TreasuryStockShares:.0f}, Frevenuegrow = {RevenueGrowPct:.2f}, Fearngrow = {EarnGrowPct:.2f}, Fframe = '{Frame[0:8]}', Ffreecash = {FreeCashFlow}, Fupdated = {systime} where Fticker = '{ticker}';" )

