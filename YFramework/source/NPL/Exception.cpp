/*
	© 2016-2018, 2020-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Exception.cpp
\ingroup NPL
\brief NPL 异常。
\version r4856
\author FrankHB <frankhb1989@gmail.com>
\since build 936
\par 创建时间:
	2022-01-21 01:59:50 +0800
\par 修改时间:
	2023-01-25 20:46 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Exception
*/


#include "NPL/YModules.h"
#include YFM_NPL_Exception // for YSLib::RecordLevel, string, string_view,
//	size_t, EscapeLiteral, Literalize, ystdex::sfmt, make_shared, std::string,
//	YSLib::share_move;
#include YFM_NPL_NPLA // for TermToStringWithReferenceMark;

//! \since build 903
using YSLib::RecordLevel;

namespace NPL
{

//! \since build 726
namespace
{

//! \since build 934
string
InitBadIdentifierExceptionString(string_view id, size_t n)
{
	return (n != 0 ? (n == 1 ? "Bad identifier: '" : "Duplicate identifier: '")
		: "Unknown identifier: '") + EscapeLiteral(id) + "'.";
}

} // unnamed namespace;


ImplDeDtor(NPLException)


ImplDeDtor(TypeError)


ImplDeDtor(ValueCategoryMismatch)


ImplDeDtor(ListTypeError)


ImplDeDtor(ListReductionFailure)


ImplDeDtor(InvalidSyntax)


ImplDeDtor(ParameterMismatch)


ArityMismatch::ArityMismatch(size_t e, size_t r, RecordLevel lv)
	: ParameterMismatch(
	ystdex::sfmt("Arity mismatch: expected %zu, received %zu.", e, r), lv),
	expected(e), received(r)
{}
ImplDeDtor(ArityMismatch)


BadIdentifier::BadIdentifier(const char* id, size_t n, RecordLevel lv)
	: InvalidSyntax(InitBadIdentifierExceptionString(id, n), lv),
	p_identifier(make_shared<std::string>(id))
{}
BadIdentifier::BadIdentifier(string_view id, size_t n, RecordLevel lv)
	: InvalidSyntax(InitBadIdentifierExceptionString(id, n), lv),
	// XXX: Not as %string, %std::string may not support the direct
	//	initialization from %string_view without WG21 P0254R2 support.
	p_identifier(make_shared<std::string>(id.data(), id.size()))
{}
ImplDeDtor(BadIdentifier)

void
BadIdentifier::ReplaceAllocator(default_allocator<yimpl(byte)> a)
{
	auto& p_src(Source.first);

	if(p_src)
		p_src = YSLib::share_move(a, *p_src);
}


ImplDeDtor(InvalidReference)


void
ThrowInsufficientTermsError(const TermNode& term, bool has_ref, size_t n_skip)
{
	throw ParameterMismatch(ystdex::sfmt(
		"Insufficient subterms found in '%s' for the list parameter.",
		TermToStringWithReferenceMark(term, has_ref, n_skip).c_str()));
}

void
ThrowListTypeErrorForAtom(const TermNode& term, bool has_ref, size_t n_skip)
{
	throw ListTypeError(ystdex::sfmt("Expected a pair, got '%s'.",
		TermToStringWithReferenceMark(term, has_ref, n_skip).c_str()));
}

void
ThrowListTypeErrorForInvalidType(const char* name, const TermNode& term,
	bool has_ref, size_t n_skip)
{
	throw ListTypeError(ystdex::sfmt("Expected a value of type '%s', got '%s'.",
		name, TermToStringWithReferenceMark(term, has_ref, n_skip).c_str()));
}
void
ThrowListTypeErrorForInvalidType(const type_info& ti,
	const TermNode& term, bool has_ref, size_t n_skip)
{
	ThrowListTypeErrorForInvalidType(ti.name(), term, has_ref, n_skip);
}

void
ThrowListTypeErrorForNonList(const TermNode& term, bool has_ref, size_t n_skip)
{
	throw ListTypeError(ystdex::sfmt("Expected a list, got '%s'.",
		TermToStringWithReferenceMark(term, has_ref, n_skip).c_str()));
}

void
ThrowTypeErrorForInvalidType(const char* name, const char* rep)
{
	throw TypeError(ystdex::sfmt("Expected a value of type '%s', got '%s'.",
		name, rep));
}
void
ThrowTypeErrorForInvalidType(const char* name, const TermNode& term,
	bool has_ref, size_t n_skip)
{
	ThrowTypeErrorForInvalidType(name,
		TermToStringWithReferenceMark(term, has_ref, n_skip).c_str());
}
void
ThrowTypeErrorForInvalidType(const type_info& ti, const TermNode& term,
	bool has_ref, size_t n_skip)
{
	ThrowTypeErrorForInvalidType(ti.name(), term, has_ref, n_skip);
}

} // namespace NPL;

