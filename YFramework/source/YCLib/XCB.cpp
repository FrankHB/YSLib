/*
	© 2014 FrankHB.

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
\version r365
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2014-12-14 14:14:31 +0800
\par 修改时间:
	2014-12-31 08:17 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::XCB
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Platform
#if YF_Use_XCB
#	include YFM_YCLib_XCB
#	include YFM_YCLib_Mutex
#	include <xcb/xcb.h>

using namespace YSLib;
using namespace Drawing;
using namespace platform::Concurrency;
using ystdex::cast_array;

namespace platform_ex
{

namespace XCB
{

//! \since build 562
static_assert(std::is_same<Atom::NativeType, ::xcb_atom_t>::value,
	"Invalid type found.");
//! \since build 562
static_assert(std::is_same<WindowData::ID, ::xcb_window_t>::value
	&& std::is_same<WindowData::ID, ::xcb_drawable_t>::value,
	"Invalid type found.");

//! \since build 561
namespace
{

//! \since build 562
//@{
static mutex TableMutex;

map<::xcb_connection_t*, map<string, Atom>>&
FetchGlobalTableRef()
{
	static map<::xcb_connection_t*, map<string, Atom>> m;

	return m;
}

pair<string, Atom>
MakeAtomPair(::xcb_connection_t& c_ref, const string& name, bool e)
{
	return {name, Atom(c_ref, name, e)};
}

locked_ptr<map<string, Atom>>
LockAtoms(::xcb_connection_t& c_ref)
{
	auto& m(FetchGlobalTableRef());
	unique_lock<mutex> lck(TableMutex);

	if(m.find(&c_ref) == m.cend())
		m[&c_ref] = {MakeAtomPair(c_ref, "WM_PROTOCOLS", true),
			MakeAtomPair(c_ref, "WM_DELETE_WINDOW", {})};
	return {&m[&c_ref], std::move(lck)};
}
//@}


class XCBErrorCategory : public std::error_category
{
public:
	PDefH(const char*, name, ) const ynothrow override
		ImplRet("XCBError")
	// TODO: Parse error.
	PDefH(std::string, message, int) const ynothrow override
		ImplRet("XCB error occurred.")
};


const XCBErrorCategory&
FetchXCBErrorCategory()
{
	static const XCBErrorCategory ecat{};

	return ecat;
}

//! \since build 564
template<typename _tParam>
YB_NORETURN void
ThrowGeneralXCBException(_tParam&& arg, Exception::LevelType lv = Err)
{
	throw Exception(0, FetchXCBErrorCategory(), yforward(arg), lv);
}

void
CheckRequest(::xcb_connection_t& c_ref, ::xcb_void_cookie_t cookie,
	Exception::LevelType lv = Err)
{
	if(const auto p
		= unique_raw(::xcb_request_check(&c_ref, cookie), std::free))
		throw XCBException("xcb_request_check", p->response_type, p->error_code,
			p->sequence, p->resource_id, p->minor_code, p->major_code,
			p->full_sequence, lv);
}

unique_ptr<::xcb_get_geometry_reply_t, void(&)(void*)>
FetchGeometry(::xcb_connection_t& c_ref, ::xcb_drawable_t id,
	Exception::LevelType lv = Err)
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

} // unnamed namespace;


XCBException::XCBException(const string& msg, std::uint8_t resp,
	std::uint8_t ec, std::uint16_t seq, std::uint32_t rid, std::uint16_t minor,
	std::uint8_t major, std::uint32_t full_seq, LevelType lv)
	: Exception(resp, FetchXCBErrorCategory(), msg, lv),
	error_code(ec), sequence(seq), resource_id(rid), minor_code(minor),
	major_code(major), full_sequence(full_seq)
{}


bool
ConnectionReference::IsOnError() const ynothrow
{
	return ::xcb_connection_has_error(Nonnull(get())) != 0;
}

int
ConnectionReference::GetFileDescriptor() const ynothrow
{
	return ::xcb_get_file_descriptor(Nonnull(get()));
}
const ::xcb_setup_t&
ConnectionReference::GetSetup() const
{
	if(const auto p = get())
	{
		if(const auto p_setup = ::xcb_get_setup(p))
			return *p_setup;
		ThrowGeneralXCBException("Getting XCB setup failed.");
	}
	ThrowGeneralXCBException("Accessing XCB connection failed.");
}

std::uint32_t
ConnectionReference::GenerateID() const ynothrow
{
	return ::xcb_generate_id(Nonnull(get()));
}


Connection::Connection(const char* disp_name, int* p_scr)
	: Connection((YTraceDe(Informative, "Creating XCB connection with display"
	" name = '%s', screen number = '%s'", disp_name ? disp_name : "[NULL]",
	p_scr ? to_string(*p_scr).c_str() : "[NULL]"),
	::xcb_connect(disp_name, p_scr)))
{}
Connection::Connection(::xcb_auth_info_t* p_auth, const char* disp_name,
	int* p_scr)
	: Connection((YTraceDe(Informative, "Creating authorized XCB connection"
	" with display name = '%s', screen number = '%s'", disp_name ? disp_name
	: "[NULL]", p_scr ? to_string(*p_scr).c_str() : "[NULL]"),
	::xcb_connect_to_display_with_auth_info(disp_name, p_auth, p_scr)))
{}
Connection::Connection(ConnectionReference c_ref)
	: ConnectionReference(c_ref)
{
	if(!get())
		ThrowGeneralXCBException("XCB Connection failed.");
}
Connection::~Connection()
{
	{
		lock_guard<mutex> lck(TableMutex);

		FetchGlobalTableRef().erase(get());
	}
	::xcb_disconnect(Nonnull(get()));
}


Atom::Atom(::xcb_connection_t& c_ref, const YSLib::string& n,
	bool only_if_exists) ynothrow
	: atom([&](::xcb_intern_atom_cookie_t cookie)->::xcb_atom_t{
		if(const auto p = unique_raw(
			::xcb_intern_atom_reply(&c_ref, cookie, {}), std::free))
			return p->atom;
		return XCB_ATOM_NONE;
	}(::xcb_intern_atom_unchecked(&c_ref, only_if_exists, n.length(),
		n.c_str())))
{}


Atom::NativeType
LookupAtom(::xcb_connection_t& c_ref, const string& name)
{
	return Deref(LockAtoms(c_ref)).at(name);
}


WindowData::WindowData(::xcb_connection_t& c_ref, const Rect& r)
	: WindowData(c_ref, r, [this]{
		if(const auto p = ::xcb_setup_roots_iterator(
			&GetConnectionRef().GetSetup()).data)
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
	CheckRequest(c_ref, ::xcb_create_window_checked(&c_ref, 32, GetID(), parent,
		r.X, r.Y, r.Width, r.Height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
		XCB_COPY_FROM_PARENT, 0, {}));
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
UpdatePixmapBuffer(WindowData& wnd, const YSLib::Drawing::Rect& r,
	const ConstGraphics& g)
{
	auto& c_ref(wnd.DerefConn());
	GContext gc(c_ref);

	CheckRequest(c_ref, ::xcb_put_image_checked(&c_ref,
		XCB_IMAGE_FORMAT_Z_PIXMAP, wnd.GetID(), gc.GetID(), r.Width, r.Height,
		r.X, r.Y, 0, Pixel::Trait::XYZBitsN, g.GetSizeOfBuffer(),
		reinterpret_cast<const byte*>(g.GetBufferPtr())));
}

} // namespace XCB;

} // namespace platform_ex;

#endif

