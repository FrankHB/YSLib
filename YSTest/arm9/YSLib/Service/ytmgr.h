// YSLib::Service::YTextManager by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-1-5 17:48:09;
// UTime = 2010-7-30 20:55;
// Version = 0.3891;


#ifndef INCLUDED_YTMGR_H_
#define INCLUDED_YTMGR_H_

// YTextManager ：平台无关的基础文本管理服务。

#include "../Core/ystring.h"
#include "../Core/yftext.h"
#include <map>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

//文本缓冲区。
class MTextBuffer : private NonCopyable
{
public:
	typedef size_t SizeType; //字符数类型。

private:
	const SizeType mlen; //最大长度（字符数）。

protected:
	//文本缓冲区的首地址和长度。
	uchar_t* const text;
	SizeType len;

	explicit
	MTextBuffer(SizeType);
	~MTextBuffer();

public:
	DefGetter(size_t, SizeOfBuffer, sizeof(uchar_t) * mlen) //取文本缓冲区的大小。
	DefGetter(uchar_t*, Ptr, text) //取文本缓冲区的指针。
	SizeType
	GetPrevChar(SizeType o, uchar_t c); //从文本缓冲区下标 o （不含）起逆序查找字符 c 。返回结果的直接后继下标；查找失败时返回 0 。
	SizeType
	GetNextChar(SizeType o, uchar_t c); //从文本缓冲区下标 o （含）起顺序查找字符 c 。返回结果的下标；查找失败时返回缓冲区长度（o 原值小于缓冲区长度）或 o 原值（大于等于缓冲区长度时）。
	SizeType
	GetPrevNewline(SizeType o); //从文本缓冲区下标 o （不含）起逆序查找换行符。返回结果的直接后继下标；查找失败时返回 0 。
	SizeType
	GetNextNewline(SizeType o); //从文本缓冲区下标 o （含）起顺序查找字符 c 。返回结果的下标；查找失败时返回缓冲区长度（o 原值小于缓冲区长度）或 o 原值（大于等于缓冲区长度时）。

	void
	ClearText(); //清空缓冲区。

	bool
	Load(const uchar_t* s); //从起始地址 s 中读取连续的 mlen 个 uchar_t 字符。
	bool
	Load(const uchar_t* s, SizeType n); //从起始地址 s 中读取连续的 n 个 uchar_t 字符。
	SizeType
	Load(YTextFile& f); //从文本文件 f 中读取连续的 mlen 个字符（自动校验换行并转换为 Unix / Linux 格式），并返回成功读取的字符数。
	SizeType
	Load(YTextFile& f, SizeType n); //从文本文件 f 中读取连续的 n 个字符，并返回成功读取的字符数。

	SizeType
	LoadN(YTextFile& f, SizeType n); //从文本文件 f 中读取连续的 n 个字节，并返回成功读取的字符数。

	bool
	Output(uchar_t* d, SizeType p, SizeType n) const; //从偏移 p 个字符起输出 n 个 uchar_t 字符到 d 。

};

inline
MTextBuffer::~MTextBuffer()
{
	delete[] text;
}

inline MTextBuffer::SizeType
MTextBuffer::GetPrevNewline(SizeType o)
{
	return GetPrevChar(o, '\n');
}
inline MTextBuffer::SizeType
MTextBuffer::GetNextNewline(SizeType o)
{
	return GetNextChar(o, '\n');
}

inline void
MTextBuffer::ClearText()
{
	memset(text, 0, GetSizeOfBuffer());
}
inline bool
MTextBuffer::Load(const uchar_t* s)
{
	return Load(s, mlen);
}
inline MTextBuffer::SizeType
MTextBuffer::Load(YTextFile& f)
{
	return Load(f, mlen);
}


//文本缓冲块。
class MTextBlock : public MTextBuffer
{
public:
	typedef u16 IndexType;

	IndexType Index;

	MTextBlock(IndexType, SizeType);
	virtual
	~MTextBlock();
};

inline
MTextBlock::MTextBlock(IndexType i, SizeType tlen)
: MTextBuffer(tlen), Index(i)
{}
inline
MTextBlock::~MTextBlock()
{}


//文本缓冲块映射。
class MTextMap
{
public:
	typedef MTextBuffer::SizeType SizeType;
	typedef MTextBlock::IndexType IndexType;
	typedef std::map<IndexType, MTextBlock*> MapType;

protected:
	MapType Map;

public:
	virtual
	~MTextMap();

	MTextBlock*
	operator[](const IndexType&);
	void
	operator+=(MTextBlock&);
	bool
	operator-=(const IndexType&);

	void
	clear();
};

inline
MTextMap::~MTextMap()
{
	clear();
}

inline MTextBlock*
MTextMap::operator[](const IndexType& id)
{
	return Map[id];
}

inline void
MTextMap::operator+=(MTextBlock& item)
{
	Map.insert(std::make_pair(item.Index, &item));
}
inline bool
MTextMap::operator-=(const IndexType& i)
{
	return Map.erase(i) != 0;
}


//文本文件块缓冲区。
class MTextFileBuffer : public MTextMap
{
public:
	static const SizeType nBlockSize = 0x2000; //文本块容量。

	//只读文本迭代器类。
	class TextIterator
	{
		friend class MTextFileBuffer;

	public:
		typedef MTextFileBuffer ContainerType;
		typedef ContainerType::SizeType SizeType;

		static const SizeType nBlockSize = MTextFileBuffer::nBlockSize;

	private:
		MTextFileBuffer* pBuf;
		SizeType nPos; //文本读取位置。

	public:
		explicit
		TextIterator(MTextFileBuffer&, SizeType = 0) ythrow(); //指定文本读取位置初始化。

		TextIterator&
		operator++() ythrow();

		TextIterator&
		operator--() ythrow();

		uchar_t
		operator*() ythrow();

		TextIterator
		operator+(std::ptrdiff_t);

		TextIterator
		operator-(std::ptrdiff_t);

		friend std::ptrdiff_t
		operator-(TextIterator, TextIterator);

		TextIterator&
		operator+=(std::ptrdiff_t);

		TextIterator&
		operator-=(std::ptrdiff_t);

		DefGetter(SizeType, Position, nPos)
		const uchar_t*
		GetTextPtr() ythrow();
	};

	YTextFile& File; //文本文件。

private:
	const SizeType nLen; //文本区段长度。

public:
	explicit
	MTextFileBuffer(YTextFile&);

	MTextBlock&
	operator[](const IndexType&);

	DefGetter(SizeType, TextLength, nLen)
};

inline std::ptrdiff_t
operator-(MTextFileBuffer::TextIterator::TextIterator a, MTextFileBuffer::TextIterator::TextIterator b)
{
	return a.nPos - b.nPos;
}

inline MTextFileBuffer::TextIterator
MTextFileBuffer::TextIterator::operator-(std::ptrdiff_t o)
{
	return *this + -o;
}

inline MTextFileBuffer::TextIterator&
MTextFileBuffer::TextIterator::operator-=(std::ptrdiff_t o)
{
	return *this += -o;
}

YSL_END_NAMESPACE(Text)

YSL_END

#endif

