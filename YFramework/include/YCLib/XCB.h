﻿/*
	© 2014-2015, 2018, 2021-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file XCB.h
\ingroup YCLib
\ingroup YCLibLimitedPlatforms
\brief XCB GUI 接口。
\version r414
\author FrankHB <frankhb1989@gmail.com>
\since build 560
\par 创建时间:
	2014-12-14 14:40:34 +0800
\par 修改时间:
	2022-01-25 05:02 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::XCB
*/


#ifndef YCL_INC_XCB_h_
#define YCL_INC_XCB_h_ 1

#include "YCLib/YModules.h"
#include YFM_YCLib_Platform
#if YF_Use_XCB
//	#include <xcb/xcb.h>
#	include YFM_YCLib_Host // for platform::string_view,
//	platform_ex::Exception;
#	include <ystdex/base.h> // for ystdex::noncopyable, ystdex::nonmovable;
#	include YFM_YCLib_Debug // for platform::Deref;
#	include YFM_YSLib_Core_YGraphics
#	include <atomic>

//! \since build 560
//@{
struct xcb_connection_t;
struct xcb_auth_info_t;
struct xcb_setup_t;
//@}
//! \since build 561
struct xcb_screen_t;

namespace platform_ex
{

//! \since build 560
namespace XCB
{

//! \since build 659
using platform::string_view;

//! \brief XCB 异常。
class YF_API XCBException : public Exception
{
private:
	//! \since build 561
	//@{
	std::uint8_t error_code;
	std::uint16_t sequence;
	std::uint32_t resource_id;
	std::uint16_t minor_code;
	std::uint8_t major_code;
	std::uint32_t full_sequence;
	//@}

public:
	/*!
	\pre 间接断言：字符串参数对应的数据指针非空。
	\since build 659
	*/
	//@{
	YB_NONNULL(2)
	XCBException(const char*, std::uint8_t, std::uint8_t, std::uint16_t,
		std::uint32_t, std::uint16_t, std::uint8_t, std::uint32_t,
		YSLib::RecordLevel = YSLib::Emergent);
	XCBException(string_view, std::uint8_t, std::uint8_t, std::uint16_t,
		std::uint32_t, std::uint16_t, std::uint8_t, std::uint32_t,
		YSLib::RecordLevel = YSLib::Emergent);
	//@}
	//! \since build 845
	DefDeCopyCtor(XCBException)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 844
	*/
	~XCBException() override;

	//! \since build 845
	DefDeCopyAssignment(XCBException)

	//! \since build 561
	//@{
	DefGetter(const ynothrow, std::uint8_t, ErrorCode, error_code)
	DefGetter(const ynothrow, std::uint8_t, FullSequence, full_sequence)
	DefGetter(const ynothrow, std::uint8_t, MajorCode, major_code)
	DefGetter(const ynothrow, std::uint16_t, MinorCode, minor_code)
	DefGetter(const ynothrow, std::uint8_t, ResponseType,
		std::uint8_t(code().value()))
	DefGetter(const ynothrow, std::uint32_t, ResourceID, resource_id)
	DefGetter(const ynothrow, std::uint16_t, Sequence, sequence)
	//@}
};


/*!
\brief 连接引用。
\warning 非虚析构。
*/
class YF_API ConnectionReference : private tidy_ptr<::xcb_connection_t>
{
public:
	using NativeHandle = typename tidy_ptr<::xcb_connection_t>::pointer;

	//! \since build 845
	using tidy_ptr<::xcb_connection_t>::tidy_ptr;
	DefDeCopyMoveCtorAssignment(ConnectionReference)

	//! \pre 间接断言： \c bool(*this) 。
	//@{
	//! \brief 检查是否出现不可恢复的错误。
	DefPred(const ynothrow, OnError, GetError() != 0)

	/*!
	\brief 取可能出现的不可恢复的错误。
	\since build 564
	*/
	int
	GetError() const ynothrow;
	//! \brief 取文件描述符。
	int
	GetFileDescriptor() const ynothrow;
	/*!
	\brief 取连接设定：调用 \c ::xcb_get_setup 。
	\throw Exception 连接为空。
	\throw Exception 找不到 XCB 连接设定。
	\since build 563
	*/
	const ::xcb_setup_t&
	GetSetup() const;

	/*!
	\brief 检查可能出现的错误。
	\throw Exception 出错时的异常。
	\since build 564
	*/
	void
	Check() const;
	//! \brief 生成标识（用于创建窗口等）。
	std::uint32_t
	GenerateID() const ynothrow;
	//@}

	//! \since build 845
	using tidy_ptr<::xcb_connection_t>::get;
};


/*!
\brief 连接。
\invariant \c get(*this) 。
*/
class YF_API Connection final : private ConnectionReference,
	private ystdex::noncopyable, private ystdex::nonmovable
{
private:
	//! \since build 593
	shared_ptr<void> p_shared;

public:
	//! \throw Exception 连接失败。
	//@{
	/*!
	\brief 构造：使用显示名称和屏幕编号。
	\note 直接传递至 \c ::xcb_connect 。
	\todo 使用 std::experimental::option 代替指针参数。
	*/
	Connection(const char* = {}, int* = {});
	/*!
	\brief 构造：使用认证信息、显示名称和屏幕编号。
	\note 直接传递至 \c ::xcb_connect_to_display_with_auth_info 。
	\todo 使用封装的类和 std::experimental::option 代替指针参数。
	*/
	Connection(::xcb_auth_info_t*, const char* = {}, int* = {});
	/*!
	\brief 构造：使用连接引用。
	\note 若连接非空则取得所有权，否则总是失败。
	*/
	explicit
	Connection(ConnectionReference);
	//@}
	//! \brief 析构：调用 \c ::xcb_disconnect 释放资源。
	~Connection();

	DefBoolNeg(explicit, !IsOnError())

	using ConnectionReference::IsOnError;

	//! \since build 564
	using ConnectionReference::GetError;
	using ConnectionReference::GetFileDescriptor;
	using ConnectionReference::GetSetup;

	//! \since build 564
	using ConnectionReference::Check;
	using ConnectionReference::GenerateID;

	//! \since build 562
	using ConnectionReference::get;
};


//! \since build 562
class YF_API Atom final
{
public:
	//! \note 同 \c ::xcb_atom_t 。
	using NativeType = std::uint32_t;

private:
	std::uint32_t atom;

public:
	/*!
	\pre 间接断言：字符串参数的数据指针非空。
	\since build 659
	*/
	explicit
	Atom(::xcb_connection_t&, string_view, bool = {}) ynothrowv;

	DefCvt(const ynothrow, NativeType, atom)
};


/*!
\brief 查找通过 \c YCLib::XCB 初始化的连接中指定名称的 Atom 值。
\throw std::out_of_range 没有找到指定的 Atom 。
\since build 563
*/
YF_API Atom::NativeType
LookupAtom(::xcb_connection_t&, const YSLib::string&);


/*!
\brief XCB 可绘制对象。
\since build 563
*/
class YF_API Drawable
{
public:
	/*!
	\note 同 \c ::xcb_drawable_t 、 \c ::xcb_window_t
		和 ::xcb_gcontext_t 等。
	*/
	using ID = std::uint32_t;

private:
	ConnectionReference conn_ref;
	ID id;

protected:
	Drawable(::xcb_connection_t& c_ref) ynothrow
		: conn_ref(make_observer(&c_ref)), id(conn_ref.GenerateID())
	{}
	DefDeDtor(Drawable)

public:
	DefGetter(const ynothrow, ConnectionReference, ConnectionRef, conn_ref)
	DefGetter(const ynothrow, ID, ID, id)

	PDefH(::xcb_connection_t&, DerefConn, ) const ynothrow
		ImplRet(platform::Deref(conn_ref.get()))
};


//! \since build 561
class YF_API WindowData final : public Drawable
{
public:
	/*!
	\throw Exception 连接失败。
	\since build 563
	*/
	//@{
	/*!
	\brief 使用指定连接、边界矩形，以默认屏幕根节点作为父窗口。
	\exception Exception 找不到 XCB 连接设定。
	\throw Exception 找不到屏幕。
	*/
	WindowData(::xcb_connection_t&, const YSLib::Drawing::Rect&);
	//! \brief 使用指定连接、边界矩形，以指定屏幕根节点作为父窗口。
	WindowData(::xcb_connection_t&, const YSLib::Drawing::Rect&,
		const ::xcb_screen_t&);
	//! \brief 使用指定连接、边界矩形和父窗口 ID 。
	WindowData(::xcb_connection_t&, const YSLib::Drawing::Rect&, ID);
	//@}
	/*!
	\brief 析构：销毁窗口。
	\since build 845
	*/
	~WindowData();

	//! \since build 562
	//@{
	YSLib::Drawing::Rect
	GetBounds() const;
	DefGetter(const, YSLib::Drawing::Point, Location, GetBounds().GetPoint())
	DefGetter(const, YSLib::Drawing::Size, Size, GetBounds().GetSize())

	void
	SetBounds(const YSLib::Drawing::Rect&);
	//@}

	//! \since build 562
	void
	Close();

	void
	Hide();

	//! \since build 562
	//@{
	PDefH(void, Invalidate, )
		ImplExpr(Invalidate(GetBounds()))
	void
	Invalidate(const YSLib::Drawing::Rect&);

	//! \since build 563
	PDefH(Atom::NativeType, LookupAtom, const YSLib::string& name)
		ImplRet(XCB::LookupAtom(DerefConn(), name))

	void
	Move(const YSLib::Drawing::Point&);

	void
	Resize(const YSLib::Drawing::Size&);

	void
	Show();
	//@}
};


/*!
\brief 图形上下文。
\since build 563
*/
class YF_API GContext final : public Drawable, private YSLib::noncopyable
{
public:
	GContext(::xcb_connection_t& c_ref) ynothrow
		: Drawable(c_ref)
	{}
	~GContext();
};


//! \since build 563
YF_API void
UpdatePixmapBuffer(WindowData&, const YSLib::Drawing::Rect&,
	const YSLib::Drawing::ConstGraphics&);

} // namespace XCB;

} // namespace platform_ex;

#endif

#endif

