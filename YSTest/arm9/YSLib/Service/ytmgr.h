// YSLib::Service::YTextManager by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-1-5 17:48:09;
// UTime = 2010-7-9 9:15;
// Version = 0.3650;


#ifndef INCLUDED_YTMGR_H_
#define INCLUDED_YTMGR_H_

// YTextManager ：平台无关的基础文本管理服务。

#include "../Core/ystring.h"
#include "../Core/yftext.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

class MTextBuffer //文本缓冲区。
{
private:
	const u32 mlen; //最大长度（字符数）。

protected:
	//文本缓冲区的首地址和长度。
	uchar_t* const text;
	u32 len;

	explicit
	MTextBuffer(u32);
	~MTextBuffer();

public:
	size_t
	GetSizeOfBuffer() const; //取文本缓冲区的大小。
	uchar_t*
	GetPtr() const; //取文本缓冲区的指针。
	u32
	GetPrevChar(u32 o, uchar_t c); //从文本缓冲区下标 o （不含）起逆序查找字符 c 。返回结果的直接后继下标；查找失败时返回 0 。
	u32
	GetNextChar(u32 o, uchar_t c); //从文本缓冲区下标 o （含）起顺序查找字符 c 。返回结果的下标；查找失败时返回缓冲区长度（o 原值小于缓冲区长度）或 o 原值（大于等于缓冲区长度时）。
	u32
	GetPrevNewline(u32 o); //从文本缓冲区下标 o （不含）起逆序查找换行符。返回结果的直接后继下标；查找失败时返回 0 。
	u32
	GetNextNewline(u32 o); //从文本缓冲区下标 o （含）起顺序查找字符 c 。返回结果的下标；查找失败时返回缓冲区长度（o 原值小于缓冲区长度）或 o 原值（大于等于缓冲区长度时）。

	void
	ClearText(); //清空缓冲区。
	bool
	Load(const uchar_t* s); //从起始地址 s 中读取连续的 mlen 个 uchar_t 字符。
	bool
	Load(const uchar_t* s, u32 n); //从起始地址 s 中读取连续的 n 个 uchar_t 字符。
	u32
	Load(YTextFile& f); //从文本文件 f 中读取连续的 mlen 个字符（自动校验换行并转换为 Unix / Linux 格式），并返回成功读取的字符数。
	u32
	Load(YTextFile& f, u32 n); //从文本文件 f 中读取连续的 n 个字符，并返回成功读取的字符数。

	bool
	Output(uchar_t* d, u32 p, u32 n) const; //从偏移 p 个字符起输出 n 个 uchar_t 字符到 d 。

};

inline
MTextBuffer::~MTextBuffer()
{
	delete[] text;
}

inline size_t
MTextBuffer::GetSizeOfBuffer() const
{
	return sizeof(uchar_t) * mlen;
}
inline uchar_t*
MTextBuffer::GetPtr() const
{
	return text;
}
inline u32
MTextBuffer::GetPrevNewline(u32 o)
{
	return GetPrevChar(o, '\n');
}
inline u32
MTextBuffer::GetNextNewline(u32 o)
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
inline u32
MTextBuffer::Load(YTextFile& f)
{
	return Load(f, mlen);
}

YSL_END_NAMESPACE(Text)

YSL_END

#endif

