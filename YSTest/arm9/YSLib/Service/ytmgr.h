// YSLib::Service::YTextManager by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-01-05 17:48:09 + 08:00;
// UTime = 2010-10-24 19:54 + 08:00;
// Version = 0.4280;


#ifndef INCLUDED_YTMGR_H_
#define INCLUDED_YTMGR_H_

// YTextManager ：平台无关的基础文本管理服务。

#include "../Core/ystring.h"
#include "../Core/yftext.h"
//#include <map>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

//文本缓冲区。
class TextBuffer : private NonCopyable
{
private:
	const SizeType capacity; //最大长度（字符数）。

protected:
	//文本缓冲区的首地址和长度。
	uchar_t* const text;
	SizeType len;

	//********************************
	//名称:		TextBuffer
	//全名:		YSLib::Text::TextBuffer::TextBuffer
	//可访问性:	protected 
	//返回类型:	
	//修饰符:	
	//形式参数:	SizeType
	//功能概要:	构造：使用指定最大长度。
	//备注:		保护实现。
	//********************************
	explicit
	TextBuffer(SizeType);
	//********************************
	//名称:		~TextBuffer
	//全名:		YSLib::Text::TextBuffer::~TextBuffer
	//可访问性:	protected 
	//返回类型:	
	//修饰符:	
	//功能概要:	析构：释放资源。
	//备注:		保护实现。
	//********************************
	~TextBuffer();

public:
	//********************************
	//名称:		operator[]
	//全名:		YSLib::Text::TextBuffer::operator[]
	//可访问性:	public 
	//返回类型:	uchar_t&
	//修饰符:	ythrow()
	//形式参数:	SizeType
	//功能概要:	返回指定下标的字符。
	//备注:		无运行期范围检查。
	//********************************
	uchar_t&
	operator[](SizeType) ythrow();

	DefGetter(SizeType, Capacity, capacity) //取最大文本长度。
	DefGetter(SizeType, SizeOfBuffer, sizeof(uchar_t) * capacity) //取文本缓冲区的大小。
	DefGetter(uchar_t*, Ptr, text) //取文本缓冲区的指针。
	DefGetter(SizeType, Length, len) //取文本缓冲区的长度。
	//********************************
	//名称:		GetPrevChar
	//全名:		YSLib::Text::TextBuffer::GetPrevChar
	//可访问性:	public 
	//返回类型:	YSLib::Text::SizeType
	//修饰符:	
	//形式参数:	SizeType o
	//形式参数:	uchar_t c
	//功能概要:	文本缓冲区下标 o （不含）起逆序查找字符 c 。
	//备注:		从返回结果的直接后继下标；查找失败时返回 0 。
	//********************************
	SizeType
	GetPrevChar(SizeType o, uchar_t c);
	//********************************
	//名称:		GetNextChar
	//全名:		YSLib::Text::TextBuffer::GetNextChar
	//可访问性:	public 
	//返回类型:	YSLib::Text::SizeType
	//修饰符:	
	//形式参数:	SizeType o
	//形式参数:	uchar_t c
	//功能概要:	从文本缓冲区下标 o （含）起顺序查找字符 c 。
	//备注:		返回结果的下标；
	//			查找失败时返回缓冲区长度（o 原值小于缓冲区长度）
	//			或 o 原值（大于等于缓冲区长度时）。
	//********************************
	SizeType
	GetNextChar(SizeType o, uchar_t c);
	//********************************
	//名称:		GetPrevNewline
	//全名:		YSLib::Text::TextBuffer::GetPrevNewline
	//可访问性:	public 
	//返回类型:	YSLib::Text::SizeType
	//修饰符:	
	//形式参数:	SizeType o
	//功能概要:	从文本缓冲区下标 o （不含）起逆序查找换行符。
	//备注:		返回结果的直接后继下标；查找失败时返回 0 。
	//********************************
	SizeType
	GetPrevNewline(SizeType o);
	//********************************
	//名称:		GetNextNewline
	//全名:		YSLib::Text::TextBuffer::GetNextNewline
	//可访问性:	public 
	//返回类型:	YSLib::Text::SizeType
	//修饰符:	
	//形式参数:	SizeType o
	//功能概要:	从文本缓冲区下标 o （含）起顺序查找字符 c 。
	//备注:		返回结果的下标；
	//			查找失败时返回缓冲区长度（o 原值小于缓冲区长度）
	//			或 o 原值（大于等于缓冲区长度时）。
	//********************************
	SizeType
	GetNextNewline(SizeType o);

	//********************************
	//名称:		at
	//全名:		YSLib::Text::TextBuffer::at
	//可访问性:	public 
	//返回类型:	uchar_t&
	//修饰符:	ythrow(std::out_of_range)
	//形式参数:	SizeType
	//功能概要:	返回指定下标的字符。
	//备注:		当越界时抛出 std::out_of_range 异常。
	//********************************
	uchar_t&
	at(SizeType) ythrow(std::out_of_range);

	//********************************
	//名称:		ClearText
	//全名:		YSLib::Text::TextBuffer::ClearText
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	清空缓冲区。
	//备注:		
	//********************************
	void
	ClearText();

	//********************************
	//名称:		Load
	//全名:		YSLib::Text::TextBuffer::Load
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	const uchar_t * s
	//功能概要:	从起始地址 s 中读取连续的 capacity 个 uchar_t 字符。
	//备注:		
	//********************************
	bool
	Load(const uchar_t* s);
	//********************************
	//名称:		Load
	//全名:		YSLib::Text::TextBuffer::Load
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	const uchar_t * s
	//形式参数:	SizeType n
	//功能概要:	从起始地址 s 中读取连续的 n 个 uchar_t 字符。
	//备注:		超过最大长度则放弃读取。
	//********************************
	bool
	Load(const uchar_t* s, SizeType n);
	//********************************
	//名称:		Load
	//全名:		YSLib::Text::TextBuffer::Load
	//可访问性:	public 
	//返回类型:	YSLib::Text::SizeType
	//修饰符:	
	//形式参数:	YTextFile & f
	//功能概要:	从文本文件 f 中读取连续的 capacity 个字符，
	//			并返回成功读取的字符数。
	//备注:		自动校验换行并转换为 Unix / Linux 格式。
	//********************************
	SizeType
	Load(YTextFile& f);
	//********************************
	//名称:		Load
	//全名:		YSLib::Text::TextBuffer::Load
	//可访问性:	public 
	//返回类型:	YSLib::Text::SizeType
	//修饰符:	
	//形式参数:	YTextFile & f
	//形式参数:	SizeType n
	//功能概要:	从文本文件 f 中读取连续的 n 个（）uchar_t 字符，
	//			并返回成功读取的字符数。
	//备注:		超过最大长度则放弃读取；
	//			自动校验换行并转换为 Unix / Linux 格式。
	//********************************
	SizeType
	Load(YTextFile& f, SizeType n);

	//********************************
	//名称:		LoadN
	//全名:		YSLib::Text::TextBuffer::LoadN
	//可访问性:	public 
	//返回类型:	YSLib::Text::SizeType
	//修饰符:	
	//形式参数:	YTextFile & f
	//形式参数:	SizeType n
	//功能概要:	从文本文件 f 中读取连续的 n 个字节，并返回成功读取的字符数。
	//备注:		
	//********************************
	SizeType
	LoadN(YTextFile& f, SizeType n);

	//********************************
	//名称:		Output
	//全名:		YSLib::Text::TextBuffer::Output
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	uchar_t * d
	//形式参数:	SizeType p
	//形式参数:	SizeType n
	//功能概要:	从偏移 p 个字符起输出 n 个 uchar_t 字符到 d 。
	//备注:		
	//********************************
	bool
	Output(uchar_t* d, SizeType p, SizeType n) const;
};

inline
TextBuffer::~TextBuffer()
{
	delete[] text;
}

inline SizeType
TextBuffer::GetPrevNewline(SizeType o)
{
	return GetPrevChar(o, '\n');
}
inline SizeType
TextBuffer::GetNextNewline(SizeType o)
{
	return GetNextChar(o, '\n');
}

inline void
TextBuffer::ClearText()
{
	memset(text, 0, GetSizeOfBuffer());
}
inline bool
TextBuffer::Load(const uchar_t* s)
{
	return Load(s, capacity);
}
inline SizeType
TextBuffer::Load(YTextFile& f)
{
	return Load(f, capacity);
}


//文本缓冲区块。
class TextBlock : public TextBuffer
{
public:
	typedef u16 BlockSizeType;

	BlockSizeType Index;

	//********************************
	//名称:		TextBlock
	//全名:		YSLib::Text::TextBlock::TextBlock
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	BlockSizeType
	//形式参数:	SizeType
	//功能概要:	构造：使用区块的大小和总长度。
	//备注:		
	//********************************
	TextBlock(BlockSizeType, SizeType);
	//********************************
	//名称:		~TextBlock
	//全名:		YSLib::Text::TextBlock::~TextBlock
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	
	//功能概要:	析构。
	//备注:		空实现。
	//********************************
	virtual
	~TextBlock();
};

inline
TextBlock::TextBlock(BlockSizeType i, SizeType tlen)
	: TextBuffer(tlen),
	Index(i)
{}
inline
TextBlock::~TextBlock()
{}


//文本缓冲区块映射。
class TextMap
{
public:
	typedef TextBlock::BlockSizeType BlockSizeType;
	typedef map<BlockSizeType, TextBlock*> MapType;

protected:
	MapType Map;

public:
	//********************************
	//名称:		~TextMap
	//全名:		YSLib::Text::TextMap::~TextMap
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	
	//功能概要:	析构。
	//备注:		清理文本区块和映射。
	//********************************
	virtual
	~TextMap();

	//********************************
	//名称:		operator[]
	//全名:		YSLib::Text::TextMap::operator[]
	//可访问性:	public 
	//返回类型:	TextBlock*
	//修饰符:	
	//形式参数:	const BlockSizeType &
	//功能概要:	取指定区块号的文本区块。
	//备注:		
	//********************************
	TextBlock*
	operator[](const BlockSizeType&);
	//********************************
	//名称:		operator+=
	//全名:		YSLib::Text::TextMap::operator+=
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	TextBlock &
	//功能概要:	添加文本区块。
	//备注:		
	//********************************
	void
	operator+=(TextBlock&);
	//********************************
	//名称:		operator-=
	//全名:		YSLib::Text::TextMap::operator-=
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	const BlockSizeType &
	//功能概要:	移除指定区块号的文本区块。
	//备注:		
	//********************************
	bool
	operator-=(const BlockSizeType&);

	//********************************
	//名称:		Clear
	//全名:		YSLib::Text::TextMap::Clear
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	清理文本区块和映射。
	//备注:		
	//********************************
	void
	Clear();
};

inline
TextMap::~TextMap()
{
	Clear();
}

inline TextBlock*
TextMap::operator[](const BlockSizeType& id)
{
	return Map[id];
}

inline void
TextMap::operator+=(TextBlock& item)
{
	Map.insert(std::make_pair(item.Index, &item));
}
inline bool
TextMap::operator-=(const BlockSizeType& i)
{
	return Map.erase(i) != 0;
}


//文本文件块缓冲区。
class TextFileBuffer : public TextMap
{
public:
	typedef TextMap::BlockSizeType BlockSizeType; //区块索引类型。

	static const SizeType nBlockSize = 0x2000; //文本区块容量。

	//只读文本循环迭代器类。
	class HText
	{
		friend class TextFileBuffer;

	public:
		typedef TextFileBuffer ContainerType;
		typedef ContainerType::BlockSizeType BlockSizeType;

	private:
		TextFileBuffer* pBuffer;
		//文本读取位置。
		BlockSizeType blk;
		SizeType idx;

	public:
		//********************************
		//名称:		HText
		//全名:		YSLib::Text::TextFileBuffer::HText::HText
		//可访问性:	public 
		//返回类型:	
		//修饰符:	ythrow()
		//形式参数:	TextFileBuffer *
		//形式参数:	BlockSizeType
		//形式参数:	SizeType
		//功能概要:	构造：指定文本读取位置。
		//备注:		无异常抛出。
		//********************************
		HText(TextFileBuffer* = NULL, BlockSizeType = 0, SizeType = 0) ythrow();

		//********************************
		//名称:		operator++
		//全名:		YSLib::Text::TextFileBuffer::HText::operator++
		//可访问性:	public 
		//返回类型:	HText&
		//修饰符:	ythrow()
		//功能概要:	迭代：循环向后遍历。
		//备注:		无异常抛出。
		//********************************
		HText&
		operator++() ythrow();

		//********************************
		//名称:		operator--
		//全名:		YSLib::Text::TextFileBuffer::HText::operator--
		//可访问性:	public 
		//返回类型:	HText&
		//修饰符:	ythrow()
		//功能概要:	迭代：循环向前遍历。
		//备注:		无异常抛出。
		//********************************
		HText&
		operator--() ythrow();

		uchar_t
		operator*() ythrow();

		//********************************
		//名称:		operator+
		//全名:		YSLib::Text::TextFileBuffer::HText::operator+
		//可访问性:	public 
		//返回类型:	YSLib::Text::TextFileBuffer::HText
		//修饰符:	
		//形式参数:	std::ptrdiff_t
		//功能概要:	迭代：重复循环向后遍历。
		//备注:		构造新迭代器进行迭代并返回。
		//********************************
		HText
		operator+(std::ptrdiff_t);

		//********************************
		//名称:		operator-
		//全名:		YSLib::Text::TextFileBuffer::HText::operator-
		//可访问性:	public 
		//返回类型:	YSLib::Text::TextFileBuffer::HText
		//修饰符:	
		//形式参数:	std::ptrdiff_t
		//功能概要:	迭代：重复循环向前遍历。
		//备注:		构造新迭代器进行迭代并返回。
		//********************************
		HText
		operator-(std::ptrdiff_t);

		//********************************
		//名称:		operator==
		//全名:		YSLib::Drawing::operator==
		//可访问性:	public 
		//返回类型:	friend bool
		//修饰符:	ythrow()
		//形式参数:	const HText &
		//形式参数:	const HText &
		//功能概要:	比较：相等关系。
		//备注:		
		//********************************
		friend bool
		operator==(const HText&, const HText&) ythrow();

		//********************************
		//名称:		operator!=
		//全名:		YSLib::Text::operator!=
		//可访问性:	public 
		//返回类型:	friend bool
		//修饰符:	ythrow()
		//形式参数:	const HText &
		//形式参数:	const HText &
		//功能概要:	比较：不等关系。
		//备注:		
		//********************************
		friend bool
		operator!=(const HText&, const HText&) ythrow();

		//********************************
		//名称:		operator<
		//全名:		std::operator<
		//可访问性:	public 
		//返回类型:	friend bool
		//修饰符:	ythrow()
		//形式参数:	const HText &
		//形式参数:	const HText &
		//功能概要:	比较：严格偏序递增关系。
		//备注:		
		//********************************
		friend bool
		operator<(const HText&, const HText&) ythrow();

		//********************************
		//名称:		operator>
		//全名:		YSLib::Text::operator>
		//可访问性:	public 
		//返回类型:	friend bool
		//修饰符:	ythrow()
		//形式参数:	const HText &
		//形式参数:	const HText &
		//功能概要:	比较：严格偏序递减关系。
		//备注:		
		//********************************
		friend bool
		operator>(const HText&, const HText&) ythrow();

		//********************************
		//名称:		operator<=
		//全名:		YSLib::Text::operator<=
		//可访问性:	public 
		//返回类型:	friend bool
		//修饰符:	ythrow()
		//形式参数:	const HText &
		//形式参数:	const HText &
		//功能概要:	比较：非严格偏序递增关系。
		//备注:		
		//********************************
		friend bool
		operator<=(const HText&, const HText&) ythrow();

		//********************************
		//名称:		operator>=
		//全名:		YSLib::Text::operator>=
		//可访问性:	public 
		//返回类型:	friend bool
		//修饰符:	ythrow()
		//形式参数:	const HText &
		//形式参数:	const HText &
		//功能概要:	比较：非严格偏序递减关系。
		//备注:		
		//********************************
		friend bool
		operator>=(const HText&, const HText&) ythrow();

		//********************************
		//名称:		operator+=
		//全名:		YSLib::Text::TextFileBuffer::HText::operator+=
		//可访问性:	public 
		//返回类型:	HText&
		//修饰符:	
		//形式参数:	std::ptrdiff_t
		//功能概要:	迭代：重复循环向后遍历。
		//备注:		
		//********************************
		HText&
		operator+=(std::ptrdiff_t);

		//********************************
		//名称:		operator-=
		//全名:		YSLib::Text::TextFileBuffer::HText::operator-=
		//可访问性:	public 
		//返回类型:	HText&
		//修饰符:	
		//形式参数:	std::ptrdiff_t
		//功能概要:	迭代：重复循环向前遍历。
		//备注:		
		//********************************
		HText&
		operator-=(std::ptrdiff_t);

		DefGetter(TextFileBuffer*, BufferPtr, pBuffer)
		DefGetter(BlockSizeType, BlockN, blk)
		//********************************
		//名称:		GetTextPtr
		//全名:		YSLib::Text::TextFileBuffer::HText::GetTextPtr
		//可访问性:	public 
		//返回类型:	
		//修饰符:	const uchar_t* GetTextPtr() const ythrow()
		//形式参数:	SizeType
		//形式参数:	IndexN
		//形式参数:	idx
		//功能概要:	取文字缓冲区指针。
		//备注:		无异常抛出。
		//********************************
		DefGetter(SizeType, IndexN, idx)
		const uchar_t*
		GetTextPtr() const ythrow();
		//********************************
		//名称:		GetBlockLength
		//全名:		YSLib::Text::TextFileBuffer::HText::GetBlockLength
		//可访问性:	public 
		//返回类型:	YSLib::Text::SizeType
		//修饰符:	const ythrow()
		//功能概要:	取当前区块的长度。
		//备注:		无异常抛出。
		//********************************
		SizeType
		GetBlockLength() const ythrow();
		//********************************
		//名称:		GetBlockLength
		//全名:		YSLib::Text::TextFileBuffer::HText::GetBlockLength
		//可访问性:	public 
		//返回类型:	YSLib::Text::SizeType
		//修饰符:	const ythrow()
		//形式参数:	BlockSizeType
		//功能概要:	取指定区块号的区块的长度。
		//备注:		无异常抛出。
		//********************************
		SizeType
		GetBlockLength(BlockSizeType) const ythrow();
	};

	YTextFile& File; //文本文件。

private:
	const SizeType nTextSize; //文本区段长度。
	const BlockSizeType nBlock; //文本区块数。

public:
	//********************************
	//名称:		TextFileBuffer
	//全名:		YSLib::Text::TextFileBuffer::TextFileBuffer
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	YTextFile &
	//功能概要:	构造：使用文本文件。
	//备注:		
	//********************************
	explicit
	TextFileBuffer(YTextFile&);

	//********************************
	//名称:		operator[]
	//全名:		YSLib::Text::TextFileBuffer::operator[]
	//可访问性:	public 
	//返回类型:	TextBlock&
	//修饰符:	
	//形式参数:	const BlockSizeType &
	//功能概要:	取指定区块号的区块。
	//备注:		
	//********************************
	TextBlock&
	operator[](const BlockSizeType&);

	DefGetter(SizeType, TextSize, nTextSize)

	HText
	begin() //********************************
	//名称:		begin
	//全名:		YSLib::Text::TextFileBuffer::begin
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	取起始迭代器。
	//备注:		
	//********************************
	ythrow();

	HText
	end() //********************************
	//名称:		end
	//全名:		YSLib::Text::TextFileBuffer::end
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	取终止迭代器。
	//备注:		
	//********************************
	ythrow();
};

inline bool
operator!=(const TextFileBuffer::HText& lhs, const TextFileBuffer::HText& rhs) ythrow()
{
	return !(lhs == rhs);
}

inline bool
operator>(const TextFileBuffer::HText& lhs, const TextFileBuffer::HText& rhs) ythrow()
{
	return rhs < lhs;
}
inline bool
operator<=(const TextFileBuffer::HText& lhs, const TextFileBuffer::HText& rhs) ythrow()
{
	return !(rhs < lhs);
}

inline bool
operator>=(const TextFileBuffer::HText& lhs, const TextFileBuffer::HText& rhs) ythrow()
{
	return !(lhs < rhs);
}

inline TextFileBuffer::HText
TextFileBuffer::HText::operator-(std::ptrdiff_t o)
{
	return *this + -o;
}

inline TextFileBuffer::HText&
TextFileBuffer::HText::operator-=(std::ptrdiff_t o)
{
	return *this += -o;
}

inline SizeType
TextFileBuffer::HText::GetBlockLength() const ythrow()
{
	return GetBlockLength(blk);
}

inline TextFileBuffer::HText
TextFileBuffer::begin() ythrow()
{
	return TextFileBuffer::HText(this);
}

inline TextFileBuffer::HText
TextFileBuffer::end() ythrow()
{
	return TextFileBuffer::HText(this, (nTextSize + nBlockSize - 1) / nBlockSize);
}

YSL_END_NAMESPACE(Text)

YSL_END

#endif

