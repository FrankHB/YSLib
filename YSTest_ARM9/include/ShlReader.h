/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ShlReader.h
\ingroup YReader
\brief Shell 阅读器框架。
\version r1601;
\author FrankHB<frankhb1989@gmail.com>
\since build 263 。
\par 创建时间:
	2011-11-24 17:08:33 +0800;
\par 修改时间:
	2011-12-15 12:54 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YReader::ShlReader;
*/


#ifndef INCLUDED_SHLREADER_H_
#define INCLUDED_SHLREADER_H_

#include "Shells.h"
#include "DSReader.h"
#include "HexBrowser.h"

YSL_BEGIN
YSL_END

YSL_BEGIN_NAMESPACE(YReader)

class ShlReader;


class ReaderBox : public Control
{
public:
	ShlReader& Shell;
	Button btnClose;
	HorizontalTrack trReader;
	Label lblProgress;

	ReaderBox(const Rect&, ShlReader&);

	IWidget*
	GetTopWidgetPtr(const Point&, bool(&)(const IWidget&));

	virtual Rect
	Refresh(const PaintContext&);
};


class TextInfoBox : public Control
{
public:
	ShlReader& Shell;
	Button btnClose;
	Label lblInfo;

	TextInfoBox(const Rect&, ShlReader&);

	IWidget*
	GetTopWidgetPtr(const Point&, bool(&)(const IWidget&));

	virtual Rect
	Refresh(const PaintContext&);

	void
	UpdateData(DualScreenReader&);
};


class FileInfoPanel : public Panel
{
public:
	Label lblPath, lblSize, lblAccessTime, lblModifiedTime, lblOperations;
	
	FileInfoPanel();
};


class ReaderManager
{
public:
	static string path;
	static bool is_text;

	ShlReader& Shell;

	ReaderManager(ShlReader&);
	DefDelCopyCtor(ReaderManager)
	DefDelMoveCtor(ReaderManager)
	virtual DefEmptyDtor(ReaderManager)
	DeclIEntry(void Activate())
	DeclIEntry(void Deactivate())
};


class TextReaderManager : public ReaderManager
{
public:
	DualScreenReader Reader;
	ReaderBox boxReader;
	TextInfoBox boxTextInfo;
	TextFile* pTextFile;
	MenuHost mhMain;

	TextReaderManager(ShlReader&);

	virtual void
	Activate();

	virtual void
	Deactivate();

private:
	void
	ExcuteReadingCommand(IndexEventArgs::ValueType);

	void
	ShowMenu(Menu::ID, const Point&);

	void
	OnClick(TouchEventArgs&&);

	void
	OnKeyDown(KeyEventArgs&&);
};


class HexReaderManager : public ReaderManager
{
public:
	HexViewArea HexArea;
	FileInfoPanel pnlFileInfo;

	HexReaderManager(ShlReader&);

	virtual void
	Activate();

	virtual void
	Deactivate();

	void
	UpdateInfo();
};


class ShlReader : public ShlDS
{
public:
	typedef ShlDS ParentType;

	shared_ptr<Image> hUp, hDn;

private:
	unique_ptr<ReaderManager> pManager;

public:
	ShlReader();

	virtual int
	OnActivated(const Message&);

	virtual int
	OnDeactivated(const Message&);
};

YSL_END_NAMESPACE(YReader)

#endif

