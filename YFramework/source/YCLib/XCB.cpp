﻿/*
	© 2014-2016, 2018, 2020, 2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file XCB.cpp
\ingroup YCLib
\ingroup YCLibLimitedPlatforms
\brief XCB GUI 接口。
\version r614
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2014-12-14 14:14:31 +0800
\par 修改时间:
	2023-05-07 04:16 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::XCB
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Platform // for YSLib::Drawing, YSLib::RecordLevel,
//	YSLib::Err, YSLib::Informative, platform::Deref, platform::Nonnull,
//	ordered_linked_map, YTraceDe;
#if YF_Use_XCB
#	include <ystdex/array.hpp> // for ystdex::cast_array;
#	include YFM_YCLib_XCB // for YSLib::FilterExceptions;
#	include YFM_YCLib_Mutex // for platform::Concurrency, platform::Threading;
#	include <xcb/xcb.h> // for ::uint32_t, ::uint8_t;
#	include <ystdex/addressof.hpp> // for ystdex::pvoid;
#	include <ystdex/type_pun.hpp> // for ystdex::aligned_store_cast;

using namespace YSLib::Drawing;
//! \since build 845
//!@{
using YSLib::RecordLevel;
using YSLib::Err;
using YSLib::Informative;
using platform::Deref;
using platform::Nonnull;
//!@}
using namespace platform::Concurrency;
//! \since build 845
using namespace platform::Threading;
using ystdex::cast_array;

namespace platform_ex
{

namespace XCB
{

//! \since build 562
static_assert(std::is_same<Atom::NativeType, ::xcb_atom_t>(),
	"Invalid type found.");
//! \since build 562
static_assert(ystdex::and_<std::is_same<WindowData::ID, ::xcb_window_t>,
	std::is_same<WindowData::ID, ::xcb_drawable_t>>(),
	"Invalid type found.");

//! \since build 561
namespace
{

// XXX: For performance, %value_map is not used, though there are no
//	requirements about invalidation at current.
//! \since build 565
using GlobalTable = ordered_linked_map<ConnectionReference::NativeHandle,
	ordered_linked_map<string, Atom>>;

//! \since build 565
shared_ptr<GlobalTable>
FetchGlobalTablePtr()
{
	static auto p(make_shared<GlobalTable>());

	return p;
}

//! \since build 562
//!@{
static mutex TableMutex;

//! \since build 565
GlobalTable&
FetchGlobalTableRef()
{
	return Deref(FetchGlobalTablePtr());
}

//! \since build 659
pair<string, Atom>
MakeAtomPair(::xcb_connection_t& c_ref, string_view name, bool e) ynothrowv
{
	return pair<string, Atom>{name, Atom(c_ref, name, e)};
}

locked_ptr<GlobalTable::mapped_type>
LockAtoms(::xcb_connection_t& c_ref)
{
	return {&FetchGlobalTableRef().at(make_observer(&c_ref)), TableMutex};
}
//!@}


//! \since build 564
//!@{
class XCBConnectionErrorCategory : public std::error_category
{
public:
	PDefH(const char*, name, ) const ynothrow override
		ImplRet("XCBConnectionError")

	std::string
	message(int) const override;
};

std::string
XCBConnectionErrorCategory::message(int err) const
{
	switch(err)
	{
	case XCB_CONN_ERROR:
		return
			"connection errors because of socket, pipe and other stream errors";
	case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:
		return "shutdown because of extension not supported";
	case XCB_CONN_CLOSED_MEM_INSUFFICIENT:
		return "malloc(), calloc() and realloc() error upon failure";
	case XCB_CONN_CLOSED_REQ_LEN_EXCEED:
		return "exceeding request length that server accepts";
	case XCB_CONN_CLOSED_PARSE_ERR:
		return "error during parsing display string";
	case XCB_CONN_CLOSED_INVALID_SCREEN:
		return "the server does not have a screen matching the display";
	case XCB_CONN_CLOSED_FDPASSING_FAILED:
		return "some FD passing operation failed";
	default:
		return "unknown";
	}
}
//!@}


class XCBErrorCategory : public std::error_category
{
public:
	PDefH(const char*, name, ) const ynothrow override
		ImplRet("XCBError")

	//! \since build 564
	std::string
	message(int) const override;
};

std::string
XCBErrorCategory::message(int err) const
{
	return std::string("XCB ") + [&]()->const char*{
		switch(err)
		{
#	define YCL_Impl_XCB_ErrCategory(_x) \
		case XCB_##_x: \
			return #_x;
		YCL_Impl_XCB_ErrCategory(REQUEST)
		YCL_Impl_XCB_ErrCategory(VALUE)
		YCL_Impl_XCB_ErrCategory(WINDOW)
		YCL_Impl_XCB_ErrCategory(PIXMAP)
		YCL_Impl_XCB_ErrCategory(ATOM)
		YCL_Impl_XCB_ErrCategory(CURSOR)
		YCL_Impl_XCB_ErrCategory(FONT)
		YCL_Impl_XCB_ErrCategory(MATCH)
		YCL_Impl_XCB_ErrCategory(DRAWABLE)
		YCL_Impl_XCB_ErrCategory(ACCESS)
		YCL_Impl_XCB_ErrCategory(ALLOC)
		YCL_Impl_XCB_ErrCategory(COLORMAP)
		YCL_Impl_XCB_ErrCategory(G_CONTEXT)
		YCL_Impl_XCB_ErrCategory(ID_CHOICE)
		YCL_Impl_XCB_ErrCategory(NAME)
		YCL_Impl_XCB_ErrCategory(LENGTH)
		YCL_Impl_XCB_ErrCategory(IMPLEMENTATION)
#	undef YCL_Impl_XCB_ErrCategory
		default:
			return "unknown";
		}
	}() + " error";
}


//! \since build 564
const XCBConnectionErrorCategory&
FetchXCBConnectionErrorCategory()
{
	static const XCBConnectionErrorCategory ecat{};

	return ecat;
}

const XCBErrorCategory&
FetchXCBErrorCategory()
{
	static const XCBErrorCategory ecat{};

	return ecat;
}

//! \since build 624
//!@{
template<typename _tParam>
YB_NORETURN void
ThrowGeneralXCBException(_tParam&& arg, int err = 0, const std::error_category&
	ecat = FetchXCBErrorCategory(), RecordLevel lv = Err)
{
	throw Exception(err, ecat, yforward(arg), lv);
}

void
CheckRequest(::xcb_connection_t& c_ref, ::xcb_void_cookie_t cookie,
	RecordLevel lv = Err)
{
	if(const auto p
		= unique_raw(::xcb_request_check(&c_ref, cookie), std::free))
		throw XCBException("xcb_request_check", p->response_type, p->error_code,
			p->sequence, p->resource_id, p->minor_code, p->major_code,
			p->full_sequence, lv);
}

//! \since build 844
unique_ptr<::xcb_get_geometry_reply_t, decltype(std::free)&>
FetchGeometry(::xcb_connection_t& c_ref, ::xcb_drawable_t id,
	RecordLevel lv = Err)
{
	const auto cookie(::xcb_get_geometry(&c_ref, id));
	::xcb_generic_error_t* p_err{};
	auto p_reply(unique_raw(::xcb_get_geometry_reply(&c_ref, cookie, &p_err),
		std::free));

	if(const auto p = unique_raw(p_err, std::free))
		throw XCBException("xcb_get_geometry_reply", p->response_type,
			p->error_code, p->sequence, p->resource_id, p->minor_code,
			p->major_code, p->full_sequence, lv);
	if(YB_UNLIKELY(!p_reply))
		ThrowGeneralXCBException("XCB reply is null without error detected.");
	return p_reply;
}
//!@}

} // unnamed namespace;


XCBException::XCBException(const char* msg, std::uint8_t resp,
	std::uint8_t ec, std::uint16_t seq, std::uint32_t rid, std::uint16_t minor,
	std::uint8_t major, std::uint32_t full_seq, RecordLevel lv)
	: Exception(resp, FetchXCBErrorCategory(), msg, lv),
	error_code(ec), sequence(seq), resource_id(rid), minor_code(minor),
	major_code(major), full_sequence(full_seq)
{}
XCBException::XCBException(string_view msg, std::uint8_t resp,
	std::uint8_t ec, std::uint16_t seq, std::uint32_t rid, std::uint16_t minor,
	std::uint8_t major, std::uint32_t full_seq, RecordLevel lv)
	: Exception(resp, FetchXCBErrorCategory(), msg, lv),
	error_code(ec), sequence(seq), resource_id(rid), minor_code(minor),
	major_code(major), full_sequence(full_seq)
{}
ImplDeDtor(XCBException)


int
ConnectionReference::GetError() const ynothrow
{
	return ::xcb_connection_has_error(Nonnull(get()).get()) != 0;
}
void
ConnectionReference::Check() const
{
	const int err(GetError());

	if(err != 0)
		ThrowGeneralXCBException("XCB connection check failed.",
			err, FetchXCBConnectionErrorCategory());
}

int
ConnectionReference::GetFileDescriptor() const ynothrow
{
	return ::xcb_get_file_descriptor(Nonnull(get()).get());
}
const ::xcb_setup_t&
ConnectionReference::GetSetup() const
{
	if(const auto p = get())
	{
		if(const auto p_setup = ::xcb_get_setup(p.get()))
			return *p_setup;
		ThrowGeneralXCBException("Getting XCB setup failed.");
	}
	ThrowGeneralXCBException("Accessing XCB connection failed.");
}

std::uint32_t
ConnectionReference::GenerateID() const ynothrow
{
	return ::xcb_generate_id(Nonnull(get()).get());
}


Connection::Connection(const char* disp_name, int* p_scr)
	: Connection({}, disp_name, p_scr)
{}
Connection::Connection(::xcb_auth_info_t* p_auth, const char* disp_name,
	int* p_scr)
	: Connection((YTraceDe(Informative, "Creating XCB connection"
	" with authorization information pointer = %p, display name = '%s',"
	" screen number = '%s'", ystdex::pvoid(p_auth), disp_name ? disp_name
	: "[NULL]", p_scr ? to_string(*p_scr).c_str() : "[NULL]"), make_observer(
	::xcb_connect_to_display_with_auth_info(disp_name, p_auth, p_scr))))
{}
Connection::Connection(ConnectionReference conn_ref)
	: ConnectionReference(conn_ref),
	p_shared([this]{
		const auto p_conn(get());

		if(!p_conn || IsOnError())
			ThrowGeneralXCBException("XCB connection failed.",
				Deref(reinterpret_cast<int*>(p_conn.get())),
				FetchXCBConnectionErrorCategory());
		return FetchGlobalTablePtr();
	}())
{
	auto& c_ref(Deref(get()));
	lock_guard<mutex> lck(TableMutex);

	Deref(static_cast<GlobalTable*>(p_shared.get()))[make_observer(&c_ref)]
		= {MakeAtomPair(c_ref, "WM_PROTOCOLS", true),
		MakeAtomPair(c_ref, "WM_DELETE_WINDOW", {})};
}
Connection::~Connection()
{
	{
		lock_guard<mutex> lck(TableMutex);

		FetchGlobalTableRef().erase(get());
	}
	::xcb_disconnect(Nonnull(get()).get());
}


Atom::Atom(::xcb_connection_t& c_ref, string_view n,
	bool only_if_exists) ynothrowv
	: atom([&](::xcb_intern_atom_cookie_t cookie) -> ::xcb_atom_t {
		if(const auto p = unique_raw(
			::xcb_intern_atom_reply(&c_ref, cookie, {}), std::free))
			return p->atom;
		return XCB_ATOM_NONE;
	}(::xcb_intern_atom_unchecked(&c_ref, only_if_exists, n.length(),
		Nonnull(n.data()))))
{}


Atom::NativeType
LookupAtom(::xcb_connection_t& c_ref, const string& name)
{
	// TODO: Blocked. Use %string_view as argument using C++14 heterogeneous
	//	%find template.
	return Deref(LockAtoms(c_ref)).at(name);
}


WindowData::WindowData(::xcb_connection_t& c_ref, const Rect& r)
	: WindowData(c_ref, r, [&c_ref]{
		if(const auto p = ::xcb_setup_roots_iterator(
			&ConnectionReference(make_observer(&c_ref)).GetSetup()).data)
			return *p;
		ThrowGeneralXCBException("No XCB screen found.");
	}())
{}
WindowData::WindowData(::xcb_connection_t& c_ref, const Rect& r,
	const ::xcb_screen_t& scr)
	: WindowData(c_ref, r, scr.root)
{}
WindowData::WindowData(::xcb_connection_t& c_ref, const Rect& r,
	WindowData::ID parent)
	: Drawable(c_ref)
{
	CheckRequest(c_ref, ::xcb_create_window_checked(&c_ref,
		XCB_COPY_FROM_PARENT, GetID(), parent, r.X, r.Y, r.Width, r.Height, 0,
		XCB_WINDOW_CLASS_INPUT_OUTPUT, XCB_COPY_FROM_PARENT, 0, {}));
}
WindowData::~WindowData()
{
	auto& c_ref(DerefConn());

	YSLib::FilterExceptions([&]{
		CheckRequest(c_ref, ::xcb_destroy_window_checked(&c_ref, GetID()));
	});
}

Rect
WindowData::GetBounds() const
{
	auto p_rep(FetchGeometry(DerefConn(), GetID()));
	auto& rep(Deref(p_rep));

	return {rep.x, rep.y, rep.width, rep.height};
}

void
WindowData::SetBounds(const Rect& r)
{
	auto& c_ref(DerefConn());
	const auto values(cast_array<std::uint32_t>(r.X, r.Y, r.Width, r.Height));

	CheckRequest(c_ref, ::xcb_configure_window_checked(&c_ref, GetID(),
		XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH
		| XCB_CONFIG_WINDOW_HEIGHT, values.cbegin()));
}

void
WindowData::Close()
{
	auto& c_ref(DerefConn());
	const auto lptr(LockAtoms(c_ref));
	const auto& atoms(Deref(lptr));
	::xcb_client_message_event_t ev{XCB_CLIENT_MESSAGE, 32, 0, GetID(),
		::xcb_atom_t(atoms.at("WM_PROTOCOLS")), {}};

	yunseq(ev.data.data32[0] = ::xcb_atom_t(atoms.at("WM_DELETE_WINDOW")),
		ev.data.data32[1] = XCB_CURRENT_TIME);
	CheckRequest(c_ref, ::xcb_send_event_checked(&c_ref, {}, GetID(),
		XCB_EVENT_MASK_PROPERTY_CHANGE, reinterpret_cast<const char*>(&ev)));
}

void
WindowData::Hide()
{
	auto& c_ref(DerefConn());

	CheckRequest(c_ref, ::xcb_unmap_window_checked(&c_ref, GetID()));
}

void
WindowData::Invalidate(const Rect& r)
{
	auto& c_ref(DerefConn());
	::xcb_expose_event_t ev{XCB_EXPOSE, 0, 0, GetID(), std::uint16_t(r.X),
		std::uint16_t(r.Y), r.Width, r.Height, 0, {0, 0}};

	CheckRequest(c_ref, ::xcb_send_event_checked(&c_ref, {}, GetID(),
		XCB_EVENT_MASK_EXPOSURE, reinterpret_cast<const char*>(&ev)));
}

void
WindowData::Move(const Point& pt)
{
	auto& c_ref(DerefConn());
	const auto values(cast_array<std::uint32_t>(pt.X, pt.Y));

	CheckRequest(c_ref, ::xcb_configure_window_checked(&c_ref, GetID(),
		XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values.cbegin()));
}

void
WindowData::Resize(const Size& s)
{
	auto& c_ref(DerefConn());
	const auto values(cast_array<std::uint32_t>(s.Width, s.Height));

	CheckRequest(c_ref, ::xcb_configure_window_checked(&c_ref, GetID(),
		XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values.cbegin()));
}

void
WindowData::Show()
{
	auto& c_ref(DerefConn());

	CheckRequest(c_ref, ::xcb_map_window_checked(&c_ref, GetID()));
}


GContext::~GContext()
{
	::xcb_free_gc_checked(&DerefConn(), GetID());
}


void
UpdatePixmapBuffer(WindowData& wnd, const Rect& r, const ConstGraphics& g)
{
	auto& c_ref(wnd.DerefConn());
	GContext gc(c_ref);

	CheckRequest(c_ref, ::xcb_put_image_checked(&c_ref,
		XCB_IMAGE_FORMAT_Z_PIXMAP, wnd.GetID(), gc.GetID(), r.Width, r.Height,
		r.X, r.Y, 0, Pixel::traits_type::XYZBitsN,
		::uint32_t(g.GetSizeOfBuffer()),
		ystdex::aligned_store_cast<const ::uint8_t*>(g.GetBufferPtr())));
}

} // namespace XCB;

} // namespace platform_ex;

#endif

