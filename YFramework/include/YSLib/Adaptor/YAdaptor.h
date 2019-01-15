/*
	© 2010-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YAdaptor.h
\ingroup Adaptor
\brief 外部库关联。
\version r2388
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-02-22 20:16:21 +0800
\par 修改时间:
	2019-01-15 11:08 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::YAdaptor
*/


#ifndef YSL_INC_Adaptor_YAdaptor_h_
#define YSL_INC_Adaptor_YAdaptor_h_ 1

#include "YModules.h"
#include YFM_YSLib_Adaptor_YNew // for <cstddef>, <cstdint>, <climits>,
//	<new>, <initializer_list>, <string>, <list>, <map>, <utility>, std::abs;
#include <libdefect/cmath.h> // for <cmath>, std::abs, std::round;
#include <ystdex/algorithm.hpp> // for <ystdex/algorithm.hpp>, <type_traits>,
//	<algorithm>;
#include <ystdex/any.h> // for <ystdex/any.h>, ystdex::any, ystdex::any_ops,
//	ystdex::in_place_type, ystdex::in_place_type_t, ystdex::any_cast,
//	ystdex::unchecked_any_cast, ystdex::unsafe_any_cast;
#include YFM_YCLib_FileIO // for <ystdex/functional.hpp>, <functional>,
//	<ystdex/memory_resource.h>, <array>, <deque>, <forward_list>, <istream>,
//	<ostream>, <queue>, <set>, <stack>, <unordered_map>, <unordered_set>,
//	YFM_YCLib_Container, <vector>, platform::basic_types,
//	platform::basic_utilities, platform::containers, '*string_view', uopen,
//	'uf*', 'up*', etc;
#include YFM_YCLib_Reference // for YFM_YCLib_Reference, platform::references,
//	<memory>, '*_ptr', 'make*_ptr', lref;
#include YFM_YCLib_Keys // for <bitset>, YCLib key space;
#include YFM_YCLib_Timer
#include YFM_YCLib_Mutex // for YFM_YCLib_Mutex, ystdex::noncopyable,
//	ystdex::nonmovable, Threading, Concurrency;
#include YFM_YCLib_FileSystem
#include YFM_YCLib_MemoryMapping // for MappedFile;
#include YFM_YCLib_Video // for MonoType, AlphaType, Color;

/*!
\brief YSLib 命名空间。
\since 早于 build 132
*/
namespace YSLib
{

/*!
\note 引入平台命名空间，并提供部分 std 替代。
\since build 843
*/
//@{
//! \since build 644
using namespace platform::basic_types;
using namespace platform::basic_utilities;
//! \note 引入的容器类型默认使用多态分配器。
using namespace platform::containers;
using namespace platform::references;
//@}


//! \note 替代较新版本的 std 对应接口，以不被当前实现模式支持的旧版 std 限制。
//@{
//! \since build 843
namespace pmr = platform::pmr;

//! \since build 597
using platform::basic_string;

//! \since build 593
using platform::string;
//! \since build 593
using platform::wstring;

//! \since build 644
//@{
using platform::basic_string_view;
using platform::string_view;
//! \since build 646
using platform::u16string_view;
using platform::wstring_view;
//@}
//@}

/*!
\note 替代标准库的类模板和类。
\note 因为提供标准库同名接口不具备的功能，不能被 std 替代。
*/
//@{
//! \since build 850
//@{
using ystdex::any;
using ystdex::function;
//@}

//! \since build 616
//@{
using platform::basic_filebuf;
using platform::filebuf;
using platform::wfilebuf;

//! \since build 619
using platform::basic_ifstream;
//! \since build 619
using platform::basic_ofstream;
using platform::basic_fstream;
//! \since build 619
using platform::ifstream;
//! \since build 619
using platform::ofstream;
using platform::fstream;
//! \since build 619
using platform::wifstream;
//! \since build 619
using platform::wofstream;
using platform::wfstream;
//@}
//@}


//! \since build 650
using ystdex::_t;
//! \since build 209
using ystdex::noncopyable;
//! \since build 373
using ystdex::nonmovable;

/*!
\sa any
\since build 850
*/
//@{
namespace any_ops = ystdex::any_ops;

using ystdex::bad_any_cast;
using ystdex::in_place_type;
using ystdex::in_place_type_t;

//! \note 可能使用 ADL 。
//@{
using ystdex::any_cast;
using ystdex::unchecked_any_cast;
using ystdex::unsafe_any_cast;
//@}
//@}

//! \since build 836
using platform::string_view_t;

//! \since build 593
//@{
//! \note 假定不使用 ADL 。
//@{
using platform::sfmt;
using platform::vsfmt;
//@}

// !\note 可能使用 ADL 。
//@{
using platform::to_string;
using platform::to_wstring;
//@}
//@}


/*!
\brief 解锁删除器：使用线程模型对应的互斥量和锁。
\since build 590
*/
using platform::Threading::unlock_delete;

/*!
\brief 并发操作。
\since build 551
*/
using namespace platform::Concurrency;
/*!
\brief 平台公用描述。
\since build 456
*/
using namespace platform::Descriptions;


/*!
\brief 断言操作。
\pre 允许对操作数使用 ADL 查找同名声明，但要求最终的表达式语义和调用这里的声明等价。
\since build 553
*/
//@{
using platform::Nonnull;
//! \since build 702
using platform::FwdIter;
using platform::Deref;
//@}


// !\note 可能使用 ADL 。
//@{
// XXX: Some declarations of overloads would defined elsewhere, so the following
//	declarations is still needed, even though there are also introduced from the
//	%platform namespace or a subnamespace thereby.
//! \since build 593
using platform::reset;

/*!
\brief 数学库函数。
\note 在标准库中提供默认实现。
\since build 301
*/
//@{
//! \since build 850
using std::abs;
using std::round;
//@}
//@}


//! \note 假定不使用 ADL 。
//@{
/*!
\brief 调试接口和日志。
\since build 510
*/
//@{
//! \since build 742
using platform::Echo;
using platform::Logger;
using platform::FetchCommonLogger;
//@}


/*!
\brief 基本实用例程和配置接口。
\since build 704
*/
//@{
//! \since build 148
using platform::terminate;
//! \since build 837
using platform::uspawn;
//! \since build 547
using platform::usystem;


//! \since build 839
using platform::SetEnvironmentVariable;

//! \since build 704
using platform::SystemOption;
//! \since build 704
using platform::FetchLimit;
//@}

/*!
\brief 文件访问例程。
\since build 299
*/
//@{
//! \since build 549
using platform::uaccess;
//! \since build 549
using platform::uopen;
using platform::ufopen;
using platform::ufexists;
//! \since build 839
using platform::upopen;
//! \since build 839
using platform::upclose;
//! \since build 313
using platform::uchdir;
//! \since build 475
using platform::umkdir;
//! \since build 475
using platform::urmdir;
//! \since build 476
using platform::uunlink;
//! \since build 476
using platform::uremove;

//! \since build 713
using platform::FetchCurrentWorkingDirectory;
//@}
//@}

//! \since build 724
using platform::MappedFile;


//! \note 基本图形定义。
using platform::SPos;
using platform::SDst;

//! \brief 图形处理。
namespace Drawing
{

//! \since build 729
using platform::AXYZTraits;
//! \since build 729
using platform::XYZATraits;
//! \since build 441
using platform::BGRA;
//! \since build 441
using platform::RGBA;
//! \since build 559
using platform::Pixel;

} // namespace Drawing;

namespace IO
{

/*!
\brief 文件访问和文件系统例程。
\note 假定不使用 ADL 。
\since build 639
*/
//@{
//! \since build 648
using platform::MakePathString;
using platform::mode_t;
using platform::FileDescriptor;
using platform::UniqueFile;
using platform::DefaultPMode;
using platform::omode_conv;
using platform::omode_convb;
//! \since build 631
using platform::GetFileAccessTimeOf;
//! \since build 547
using platform::GetFileModificationTimeOf;
//! \since build 631
using platform::GetFileModificationAndAccessTimeOf;
using platform::FetchNumberOfLinks;
using platform::EnsureUniqueFile;
//! \since build 659
//@{
using platform::HaveSameContents;
using platform::IsNodeShared;

//! \since build 474
using platform::NodeCategory;

//! \since build 707
using platform::IsDirectory;

using platform::CreateHardLink;
using platform::CreateSymbolicLink;
//@}
//! \since build 707
using platform::ReadLink;
//! \since build 707
using platform::IterateLink;
//! \since build 411
//@{
using platform::DirectorySession;
using platform::HDirectory;
//@}
//! \since build 706
using platform::FetchSeparator;
//! \since build 706
using platform::IsSeparator;
//! \since build 171
using platform::IsAbsolute;
//! \since build 836
using platform::FetchRootNameEnd;
//! \since build 654
using platform::FetchRootNameLength;
//! \since build 836
using platform::FetchRootPathLength;
//! \since build 707
using platform::TrimTrailingSeperator;
//@}

} // namespace IO;

/*!
\brief 计时器和时钟。
\since build 850
*/
namespace Timers
{

/*!
\note 假定不使用 ADL 。
\since build 301
*/
//@{
using platform::GetTicks;
using platform::GetHighResolutionTicks;
//@}

} // namespace Timers;

//! \note UI 输入类型。
//@{
//! \since build 490
using platform::KeyIndex;
//! \since build 486
using platform::KeyBitsetWidth;
using platform::KeyInput;
//! \since build 489
namespace KeyCategory = platform::KeyCategory;
namespace KeyCodes = platform::KeyCodes;
//@}

//! \note 假定不使用 ADL 。
//@{
//! \since build 486
using platform::FindFirstKey;
//! \since build 487
using platform::FindNextKey;
//! \since build 487
using platform::MapKeyChar;

//! \note UI 基本输出接口。
using platform::InitVideo;
//@}

} // namespace YSLib;

#endif

