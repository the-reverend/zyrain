/// @file documentation.h
/// @author rev

#pragma once

/// @mainpage Zyrain
///
/// @section Introduction
///
/// Zyrain is a proxy-style helper for TradeWars 2002.
///
/// @subsection Contributors
///
/// @li Ron Wilson
///

/*
CREATE TABLE ZA(
	actor_ integer primary key, 
	actorName, 
	type_, 
	corpNum, 
	actor$ integer DEFAULT (cast(strftime('%s', 'now') as integer)), 
	guid_ DEFAULT 1, 
	UNIQUE(actorName, corpNum, type_, guid_));

CREATE TABLE ZI(
	actor_, 
	infoKey, 
	infoValue, 
	info$ integer DEFAULT (cast(strftime('%s', 'now') as integer)), 
	guid_ DEFAULT 1, 
	UNIQUE(actor_, infoKey, guid_));

CREATE TABLE ZT(
	type_ integer primary key, 
	typeName, 
	type$ integer DEFAULT (cast(strftime('%s', 'now') as integer)), 
	guid_ DEFAULT 1, 
	UNIQUE(typeName, guid_));

CREATE TABLE ZR(
	sector_, 
	actor_, 
	report$ integer DEFAULT (cast(strftime('%s', 'now') as integer)), 
	guid_ DEFAULT 1, 
	UNIQUE(sector_, actor_, guid_));

CREATE TABLE ZF(
	sector_, 
	figCount, 
	actor_, 
	fig$ integer DEFAULT (cast(strftime('%s', 'now') as integer)), 
	guid_ DEFAULT 1, 
	UNIQUE(sector_, guid_));

CREATE TABLE ZG(
	guid_ integer primary key, 
	guid, 
	UNIQUE(guid));

CREATE TABLE ZH(
	port_, 
	product, 
	exp, 
	tradeType, 
	iteration, 
	offer, 
	counter, 
	percent, 
	haggle$ integer DEFAULT (cast(strftime('%s', 'now') as integer)), 
	guid_ DEFAULT 1, 
	UNIQUE(port_, product, haggle$, guid_));

CREATE TABLE ZL(
	sector_, 
	limpetCount, 
	actor_, 
	limpet$ integer DEFAULT (cast(strftime('%s', 'now') as integer)), 
	guid_ DEFAULT 1, 
	UNIQUE(sector_, guid_));

CREATE TABLE ZM(
	sector_, 
	mineCount, 
	actor_, 
	mine$ integer DEFAULT (cast(strftime('%s', 'now') as integer)), 
	guid_ DEFAULT 1, 
	UNIQUE(sector_, guid_));

CREATE TABLE ZP(
	port_ integer primary key, 
	sector_, 
	portName, 
	type_, 
	ore, 
	oreMax, 
	oreMcic, 
	org, 
	orgMax, 
	orgMcic, 
	equ, 
	equMax, 
	equMcic, 
	days, 
	actor_, 
	port$ integer DEFAULT (cast(strftime('%s', 'now') as integer)), 
	guid_ DEFAULT 1, 
	UNIQUE(sector_, portName, type_, guid_));

CREATE TABLE ZS(
	sector_ integer, 
	port_, 
	beacon, 
	space, 
	sector$ integer DEFAULT (cast(strftime('%s', 'now') as integer)), 
	guid_ DEFAULT 1, 
	UNIQUE (sector_, guid_));

CREATE TABLE ZW(
	sector_, 
	warp, 
	warp$ integer DEFAULT (cast(strftime('%s', 'now') as integer)), 
	guid_ DEFAULT 1, 
	UNIQUE(src, dst, guid_));

CREATE TABLE ZO(
	actor_, 
	isOnline, 
	online1$ integer, 
	online2$ integer, 
	guid_ DEFAULT 1);

*/