/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfile.h
\ingroup Core
\brief 平台无关的文件抽象。
\version r1891;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-24 23:14:41 +0800;
\par 修改时间:
	2012-04-30 22:41 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Core::YFile;
*/


#ifndef YSL_INC_CORE_YFILE_H_
#define YSL_INC_CORE_YFILE_H_

#include "../Core/ycutil.h"

YSL_BEGIN

/*!
\brief 文件基类。
\since build 206 。
*/
class File : private noncopyable
{
protected:
	std::FILE* fp; //!< 默认文件指针。
	size_t fsize; //!< 文件大小。

public:
	/*!
	\brief 构造。
	\note 得到空对象。
	*/
	File();
	/*!
	\brief 构造：使用指定文件路径初始化对象。
	\note 自动打开文件。
	\param is_text 文本方式。
	*/
	explicit
	File(const_path_t, bool is_text = false);
	/*!
	\brief 构造：使用指定文件路径初始化对象。
	\note 自动打开文件。
	\param is_text 文本方式。
	\since build 305 。
	*/
	explicit
	File(const String&, bool is_text = false);
	/*!
	\brief 析构。
	\note 自动关闭文件。
	*/
	virtual
	~File();

	DefPred(const ynothrow, Valid, fp) //!< 判断文件指针是否有效。

	DefGetter(const ynothrow, FILE*, Ptr, fp) //!< 取文件指针。
	DefGetter(const ynothrow, size_t, Size, fsize) //!< 取文件大小。
	/*!
	\brief 取文件指针的位置，返回值语义同 std::ftell 。
	*/
	DefGetter(const ynothrow, ptrdiff_t, Position, std::ftell(fp))

	/*!
	\brief 设置文件指针位置。
	\note 参数和返回值语义同 std::fseek 。
	\since build 273 。
	*/
	PDefH(int, Seek, ptrdiff_t offset, int whence) const
		ImplRet(std::fseek(fp, offset, whence))

	/*!
	\brief 检测文件结束符。
	\note 参数和返回值语义同 std::feof() 。
	*/
	PDefH(int, CheckEOF) const
		ImplRet(std::feof(fp))

private:
	/*!
	\brief 检查文件有效长度后读位置返回文件起始。
	\since build 305 。
	*/
	void
	CheckSize();

public:
	/*!
	\brief 关闭文件。
	\note 清除文件指针。
	*/
	void
	Close();

	/*!
	\brief 以指定方式打开指定路径的文件。
	\param is_text 文本方式。
	*/
	bool
	Open(const_path_t, bool is_text = false);
	/*!
	\brief 以指定方式打开指定路径的文件。
	\param is_text 文本方式。
	\since build 305 。
	*/
	bool
	Open(const String&, bool is_text = false);

	/*!
	\brief 连续读 nmemb 个大小为 size 文件块到 ptr 中，语义同 std::fread 。
	\return 返回成功读取的文件块数。
	\since build 290 。
	*/
	PDefH(size_t, Read, void* ptr, size_t size = 1U, size_t nmemb = 1U) const
		ImplRet(std::fread(ptr, size, nmemb, fp))

	/*!
	\brief 文件指针返回到文件头，语义同 std::rewind 。
	*/
	PDefH(void, Rewind) const
		ImplRet(std::rewind(fp))
};

YSL_END

#endif

