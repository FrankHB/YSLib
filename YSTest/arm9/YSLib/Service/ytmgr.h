// YSLib::Service::YTextManager by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-01-05 17:48:09 + 08:00;
// UTime = 2010-10-05 15:29 + 08:00;
// Version = 0.4117;


#ifndef INCLUDED_YTMGR_H_
#define INCLUDED_YTMGR_H_

// YTextManager ：平台无关的基础文本管理服务。

#include "../Core/ystring.h"
#include "../Core/yftext.h"
#include <map>

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

	explicit
	TextBuffer(SizeType);
	~TextBuffer();

public:
	uchar_t&
	operator[](SizeType) ythrow(); //按下标返回字符，无运行期范围检查。

	DefGetter(SizeType, Capacity, capacity) //取最大文本长度。
	DefGetter(SizeType, SizeOfBuffer, sizeof(uchar_t) * capacity) //取文本缓冲区的大小。
	DefGetter(uchar_t*, Ptr, text) //取文本缓冲区的指针。
	DefGetter(SizeType, Length, len) //取文本缓冲区的长度。
	SizeType
	GetPrevChar(SizeType o, uchar_t c); //从文本缓冲区下标 o （不含）起逆序查找字符 c 。返回结果的直接后继下标；查找失败时返回 0 。
	SizeType
	GetNextChar(SizeType o, uchar_t c); //从文本缓冲区下标 o （含）起顺序查找字符 c 。返回结果的下标；查找失败时返回缓冲区长度（o 原值小于缓冲区长度）或 o 原值（大于等于缓冲区长度时）。
	SizeType
	GetPrevNewline(SizeType o); //从文本缓冲区下标 o （不含）起逆序查找换行符。返回结果的直接后继下标；查找失败时返回 0 。
	SizeType
	GetNextNewline(SizeType o); //从文本缓冲区下标 o （含）起顺序查找字符 c 。返回结果的下标；查找失败时返回缓冲区长度（o 原值小于缓冲区长度）或 o 原值（大于等于缓冲区长度时）。

	uchar_t&
	at(SizeType) ythrow(std::out_of_range); //按下标返回字符，当越界时抛出 std::out_of_range 。

	void
	ClearText(); //清空缓冲区。

	bool
	Load(const uchar_t* s); //从起始地址 s 中读取连续的 capacity 个 uchar_t 字符。
	bool
	Load(const uchar_t* s, SizeType n); //从起始地址 s 中读取连续的 n 个（超过最大长度则放弃读取） uchar_t 字符。
	SizeType
	Load(YTextFile& f); //从文本文件 f 中读取连续的 capacity 个字符（自动校验换行并转换为 Unix / Linux 格式），并返回成功读取的字符数。
	SizeType
	Load(YTextFile& f, SizeType n); //从文本文件 f 中读取连续的 n 个（超过最大长度则放弃读取）uchar_t 字符（自动校验换行并转换为 Unix / Linux 格式），并返回成功读取的字符数。

	SizeType
	LoadN(YTextFile& f, SizeType n); //从文本文件 f 中读取连续的 n 个字节，并返回成功读取的字符数。

	bool
	Output(uchar_t* d, SizeType p, SizeType n) const; //从偏移 p 个字符起输出 n 个 uchar_t 字符到 d 。
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


//文本缓冲块。
class TextBlock : public TextBuffer
{
public:
	typedef u16 BlockSizeType;

	BlockSizeType Index;

	TextBlock(BlockSizeType, SizeType);
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


//文本缓冲块映射。
class TextMap
{
public:
	typedef TextBlock::BlockSizeType BlockSizeType;
	typedef std::map<BlockSizeType, TextBlock*> MapType;

protected:
	MapType Map;

public:
	virtual
	~TextMap();

	TextBlock*
	operator[](const BlockSizeType&);
	void
	operator+=(TextBlock&);
	bool
	operator-=(const BlockSizeType&);

	void
	clear();
};

inline
TextMap::~TextMap()
{
	clear();
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
	typedef TextMap::BlockSizeType BlockSizeType; //块索引类型。

	static const SizeType nBlockSize = 0x2000; //文本块容量。

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
		HText(TextFileBuffer* = NULL, BlockSizeType = 0, SizeType = 0) ythrow(); //指定文本读取位置初始化。

		HText&
		operator++() ythrow();

		HText&
		operator--() ythrow();

		uchar_t
		operator*() ythrow();

		HText
		operator+(std::ptrdiff_t);

		HText
		operator-(std::ptrdiff_t);

		friend bool
		operator==(const HText&, const HText&) ythrow();

		friend bool
		operator!=(const HText&, const HText&) ythrow();

		friend bool
		operator<(const HText&, const HText&) ythrow();

		friend bool
		operator>(const HText&, const HText&) ythrow();

		friend bool
		operator<=(const HText&, const HText&) ythrow();

		friend bool
		operator>=(const HText&, const HText&) ythrow();

		HText&
		operator+=(std::ptrdiff_t);

		HText&
		operator-=(std::ptrdiff_t);

		DefGetter(TextFileBuffer*, BufferPtr, pBuffer)
		DefGetter(BlockSizeType, BlockN, blk)
		DefGetter(SizeType, IndexN, idx)
		const uchar_t*
		GetTextPtr() const ythrow();
		SizeType
		GetBlockLength() const ythrow();
		SizeType
		GetBlockLength(BlockSizeType) const ythrow();
	};

	YTextFile& File; //文本文件。

private:
	const SizeType nTextSize; //文本区段长度。
	const BlockSizeType nBlock; //文本区块数。

public:
	explicit
	TextFileBuffer(YTextFile&);

	TextBlock&
	operator[](const BlockSizeType&);

	DefGetter(SizeType, TextSize, nTextSize)

	HText
	begin() ythrow();

	HText
	end() ythrow();
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

