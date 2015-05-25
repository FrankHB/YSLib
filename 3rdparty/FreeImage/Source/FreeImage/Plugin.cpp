// =====================================================================
// FreeImage Plugin Interface
//
// Design and implementation by
// - Floris van den Berg (floris@geekhq.nl)
// - Rui Lopes (ruiglopes@yahoo.com)
// - Detlev Vendt (detlev.vendt@brillit.de)
// - Petr Pytelka (pyta@lightcomp.com)
//
// This file is part of FreeImage 3
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// =====================================================================

// Modified by FrankHB <frankhb1989@gmail.com>, 2013-06-26:
//	Disabled unused plugins and make placeholders in plugin list in "Source/Plugin.h" and "Source/FreeImage/Plugin.cpp".
// Modified by FrankHB <frankhb1989@gmail.com>, 2013-07-20:
//	Normalized all EOL marker as CR+LF.
//	Removed all spaces at end of lines.
//	Saved as UTF8 + BOM.
//	Removed definitions of function only supported in Win32: "FreeImage_LoadU", "FreeImage_SaveU" and "FreeImage_GetFIFFromFilenameU".
// Modified by FrankHB <frankhb1989@gmail.com>, 2013-11-10:
//  Added definition of function "FreeImageEx_GetPluginNodeFromFIF".
//  Removed definitions of function: "FreeImage_GetFormatFromFIF", "FreeImage_GetFIFMimeType", "FreeImage_GetFIFExtensionList", "FreeImage_GetFIFDescription", "FreeImage_GetFIFRegExpr", "FreeImage_FIFSupportsReading", "FreeImage_FIFSupportsWriting", "FreeImage_FIFSupportsExportBPP", "FreeImage_FIFSupportsExportType", "FreeImage_FIFSupportsICCProfiles", "FreeImage_FIFSupportsNoPixels", "FreeImage_SetPluginEnabled", "FreeImage_IsPluginEnabled".
//	Simplified implementation using C++11.
//	Made "PluginList::AddFakeNode" call "FreeImage_OutputMessageProc" on fail.
// Modified by FrankHB <frankhb1989@gmail.com>, 2014-07-19:
//	Added fake node as entries in base version 3.16.0: "WEBP" and "JXR".
// Modified by FrankHB <frankhb1989@gmail.com>, 2015-05-25:
//	Removed external plugin initialization for Windows.

#ifdef _MSC_VER
#pragma warning (disable : 4786) // identifier was truncated to 'number' characters
#endif

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <ctype.h>
#endif // _WIN32

#include "FreeImage.h"
#include "Utilities.h"
#include "FreeImageIO.h"
#include "Plugin.h"
#include <string>

#include "../Metadata/FreeImageTag.h"

// =====================================================================

using namespace std;

// =====================================================================
// Plugin search list
// =====================================================================

const char* s_search_list[] = {"", "plugins\\",};

static auto s_search_list_size = sizeof(s_search_list) / sizeof(char*);
static PluginList* s_plugins = {};
static size_t s_plugin_reference_count = 0;


// =====================================================================
// Reimplementation of stricmp (it is not supported on some systems)
// =====================================================================

static int
FreeImage_stricmp(const char *s1, const char *s2)
{
	int c1, c2;

	do
	{
		c1 = tolower(*s1++);
		c2 = tolower(*s2++);
	}while(c1 && c1 == c2);
	return c1 - c2;
}

// =====================================================================
//  Implementation of PluginList
// =====================================================================

PluginList::PluginList()
	: m_plugin_map()
{}

FREE_IMAGE_FORMAT
PluginList::AddNode(FI_InitProc init_proc, void* instance, const char *format,
	const char *description, const char *extension, const char *regexpr)
{
	if(init_proc)
		try
		{
			unique_ptr<PluginNode> p_node_orig(new PluginNode);
			unique_ptr<Plugin> p_plugin(new Plugin());
			const int id(m_plugin_map.size());

			// fill-in the plugin structure
			// note we have value-initialized *p_plugin, so all unset pointers
			//	should be null
			init_proc(p_plugin.get(), id);
			// get the format string (two possible ways)
			// add the node if it wasn't there already
			if(const char *the_format = format ? format : p_plugin->format_proc
				? p_plugin->format_proc() : nullptr)
			{
				auto& p_node(m_plugin_map[id]);

				p_node.reset(p_node_orig.release());
				*p_node = {id, instance, p_plugin.release(), true, format,
					description, extension, regexpr};
				return ::FREE_IMAGE_FORMAT(id);
			}
		}
		catch(bad_alloc&)
		{
			FreeImage_OutputMessageProc(FIF_UNKNOWN, FI_MSG_ERROR_MEMORY);
		}
		catch(...)
		{}
	return FIF_UNKNOWN;
}

FREE_IMAGE_FORMAT
PluginList::AddFakeNode(const char* format)
{
	try
	{
		const int id(m_plugin_map.size());
		auto& p_node(m_plugin_map[id]);

		p_node.reset(new PluginNode());
		p_node->m_id = int(m_plugin_map.size()),
		p_node->m_format = format;
		return ::FREE_IMAGE_FORMAT(id);
	}
	catch(bad_alloc&)
	{
		FreeImage_OutputMessageProc(FIF_UNKNOWN, FI_MSG_ERROR_MEMORY);
	}
	return FIF_UNKNOWN;
}

PluginNode*
PluginList::FindNodeFromFormat(const char *format)
{
	for(const auto& pr : m_plugin_map)
	{
		const auto& p(pr.second);

		assert(p);

		const auto the_format(p->m_format ? p->m_format
			: p->m_plugin->format_proc());

		if(p->m_enabled && FreeImage_stricmp(the_format, format) == 0)
			return p.get();
	}
	return {};
}

PluginNode*
PluginList::FindNodeFromMime(const char *mime)
{
	for(const auto& pr : m_plugin_map)
	{
		const auto& p(pr.second);

		assert(p);

		const auto& node(*p);
		const auto p_m_plugin(node.m_plugin);
		
		assert(p_m_plugin);

		const auto mime_proc(p_m_plugin->mime_proc);
		const auto the_mime(mime_proc ? mime_proc() : "");

		if(node.m_enabled)
			if(the_mime && strcmp(the_mime, mime) == 0)
				return p.get();
	}
	return {};
}

PluginNode*
PluginList::FindNodeFromFIF(int node_id)
{
	const auto i(m_plugin_map.find(node_id));

	return i != m_plugin_map.end() ? i->second.get() : nullptr;
}

PluginList::~PluginList()
{
	for(const auto& pr : m_plugin_map)
	{
		const auto& p(pr.second);

		assert(p);
#ifdef _WIN32
		if(p->m_instance)
			::FreeLibrary(::HINSTANCE(p->m_instance));
#endif
		delete p->m_plugin;
	}
}

// =====================================================================
// Retrieve a pointer to the plugin list container
// =====================================================================

PluginList* DLL_CALLCONV
FreeImage_GetPluginList()
{
	return s_plugins;
}

// =====================================================================
// Plugin System Initialization
// =====================================================================

void DLL_CALLCONV
FreeImage_Initialise(BOOL load_local_plugins_only)
{
	if(s_plugin_reference_count++ == 0)
	{
		// initialise the TagLib singleton
		TagLib& s = TagLib::instance();

		// internal plugin initialization

		s_plugins = new(std::nothrow) PluginList;

		if(s_plugins)
		{
			/* NOTE :
			The order used to initialize internal plugins below MUST BE the same order
			as the one used to define the FREE_IMAGE_FORMAT enum.
			*/
			s_plugins->AddNode(InitBMP);
			s_plugins->AddNode(InitICO);
			s_plugins->AddNode(InitJPEG);
			s_plugins->AddFakeNode("JNG");
			s_plugins->AddFakeNode("KOALA");
			s_plugins->AddFakeNode("IFF");
			s_plugins->AddFakeNode("MNG");
			s_plugins->AddFakeNode("PBM");
		//	s_plugins->AddNode(InitPNM, {}, "PBM", "Portable Bitmap (ASCII)", "pbm", "^P1");
			s_plugins->AddFakeNode("PBMRAW");
		//	s_plugins->AddNode(InitPNM, {}, "PBMRAW", "Portable Bitmap (RAW)", "pbm", "^P4");
			s_plugins->AddFakeNode("PCD");
			s_plugins->AddFakeNode("PCX");
			s_plugins->AddFakeNode("PGM");
		//	s_plugins->AddNode(InitPNM, {}, "PGM", "Portable Greymap (ASCII)", "pgm", "^P2");
			s_plugins->AddFakeNode("PGMRAW");
		//	s_plugins->AddNode(InitPNM, {}, "PGMRAW", "Portable Greymap (RAW)", "pgm", "^P5");
			s_plugins->AddNode(InitPNG);
			s_plugins->AddFakeNode("PPM");
		//	s_plugins->AddNode(InitPNM, {}, "PPM", "Portable Pixelmap (ASCII)", "ppm", "^P3");
			s_plugins->AddFakeNode("PPMRAW");
		//	s_plugins->AddNode(InitPNM, {}, "PPMRAW", "Portable Pixelmap (RAW)", "ppm", "^P6");
			s_plugins->AddFakeNode("RAS");
			s_plugins->AddFakeNode("TAGRA");
			s_plugins->AddFakeNode("TIFF");
			s_plugins->AddFakeNode("WBMP");
			s_plugins->AddFakeNode("PSD");
			s_plugins->AddFakeNode("CUT");
			s_plugins->AddFakeNode("XBM");
			s_plugins->AddFakeNode("XPM");
			s_plugins->AddFakeNode("DDS");
			s_plugins->AddNode(InitGIF);
			s_plugins->AddFakeNode("HDR");
			s_plugins->AddFakeNode("G3");
			s_plugins->AddFakeNode("SGI");
			s_plugins->AddFakeNode("EXR");
			s_plugins->AddFakeNode("J2K");
			s_plugins->AddFakeNode("JP2");
			s_plugins->AddFakeNode("PFM");
			s_plugins->AddFakeNode("PICT");
			s_plugins->AddFakeNode("RAW");
			s_plugins->AddFakeNode("WEBP");
#if !(defined(_MSC_VER) && (_MSC_VER <= 1310))
			s_plugins->AddFakeNode("JXR");
#endif // unsupported by MS Visual Studio 2003 !!!
		}
	}
}

void DLL_CALLCONV
FreeImage_DeInitialise()
{
	if(--s_plugin_reference_count == 0)
		delete s_plugins;
}

// =====================================================================
// Open and close a bitmap
// =====================================================================

static Plugin&
FI_GetPlugin(PluginNode* p_node)
{
	assert(p_node);

	const auto p_plugin(p_node->m_plugin);

	assert(p_plugin);

	return *p_plugin;
}

void* DLL_CALLCONV
FreeImage_Open(PluginNode* p_node, FreeImageIO* io, fi_handle handle,
	BOOL open_for_reading)
{
	if(const auto p_proc = FI_GetPlugin(p_node).open_proc)
		return p_proc(io, handle, open_for_reading);
	return {};
}

void DLL_CALLCONV
FreeImage_Close(PluginNode* p_node, FreeImageIO *io, fi_handle handle,
	void* data)
{
	if(const auto p_proc = FI_GetPlugin(p_node).close_proc)
		p_proc(io, handle, data);
}

// =====================================================================
// Plugin System Load/Save Functions
// =====================================================================

FIBITMAP* DLL_CALLCONV
FreeImage_LoadFromHandle(FREE_IMAGE_FORMAT fif, FreeImageIO *io,
	fi_handle handle, int flags)
{
	if((fif >= 0) && (fif < FreeImage_GetFIFCount()))
		if(const auto p_node = s_plugins->FindNodeFromFIF(fif))
			if(p_node->m_plugin->load_proc)
			{
				void* data(::FreeImage_Open(p_node, io, handle, true));
				const auto p_proc(FI_GetPlugin(p_node).load_proc);

				assert(p_proc);

				const auto bitmap(p_proc(io, handle, -1, flags, data));

				::FreeImage_Close(p_node, io, handle, data);
				return bitmap;
			}
	return {};
}

FIBITMAP* DLL_CALLCONV
FreeImage_Load(FREE_IMAGE_FORMAT fif, const char *filename, int flags)
{
	FreeImageIO io;
	SetDefaultIO(&io);

	FILE *handle = fopen(filename, "rb");

	if(handle)
	{
		const auto bitmap(::FreeImage_LoadFromHandle(fif, &io,
			::fi_handle(handle), flags));

		fclose(handle);
		return bitmap;
	}
	else
		FreeImage_OutputMessageProc(int(fif),
			"FreeImage_Load: failed to open file %s", filename);
	return {};
}

BOOL DLL_CALLCONV
FreeImage_SaveToHandle(FREE_IMAGE_FORMAT fif, FIBITMAP*dib, FreeImageIO *io,
	fi_handle handle, int flags)
{
	// cannot save "header only" formats
	if(!FreeImage_HasPixels(dib))
	{
		FreeImage_OutputMessageProc(int(fif),
			"FreeImage_SaveToHandle: cannot save \"header only\" formats");
		return {};
	}
	if((fif >= 0) && (fif < FreeImage_GetFIFCount()))
		if(const auto p_node = s_plugins->FindNodeFromFIF(fif))
			if(p_node->m_plugin->save_proc)
			{
				const auto data(::FreeImage_Open(p_node, io, handle, false));
				const bool result(p_node->m_plugin->save_proc(io, dib, handle,
					-1, flags, data));

				::FreeImage_Close(p_node, io, handle, data);
				return result;
			}
	return {};
}


BOOL DLL_CALLCONV
FreeImage_Save(FREE_IMAGE_FORMAT fif, FIBITMAP*dib, const char *filename,
	int flags)
{
	FreeImageIO io;
	SetDefaultIO(&io);

	if(const auto fp = fopen(filename, "w+b"))
	{
		const bool success(FreeImage_SaveToHandle(fif, dib, &io,
			::fi_handle(fp), flags));

		fclose(fp);
		return success;
	}
	else
		FreeImage_OutputMessageProc(int(fif),
			"FreeImage_Save: failed to open file %s", filename);
	return {};
}

// =====================================================================
// Plugin construction + enable/disable functions
// =====================================================================

FREE_IMAGE_FORMAT DLL_CALLCONV
FreeImage_RegisterLocalPlugin(FI_InitProc proc_address, const char *format,
	const char *description, const char *extension, const char *regexpr)
{
	return s_plugins->AddNode(proc_address, {}, format, description,
		extension, regexpr);
}

#ifdef _WIN32
FREE_IMAGE_FORMAT DLL_CALLCONV
FreeImage_RegisterExternalPlugin(const char *path, const char *format,
	const char *description, const char *extension, const char *regexpr)
{
	if(path)
		if(const auto instance = ::LoadLibrary(path))
		{
			const auto proc = GetProcAddress(instance, "_Init@8");

			const auto result(s_plugins->AddNode(
				::FI_InitProc(proc), static_cast<void*>(instance),
				format, description, extension, regexpr));

			if(result == FIF_UNKNOWN)
				::FreeLibrary(instance);
			return result;
		}
	return FIF_UNKNOWN;
}
#endif // _WIN32

// =====================================================================
// Plugin Access Functions
// =====================================================================

static ::FREE_IMAGE_FORMAT
FI_GetFIFFromPluginNodePtr(PluginNode* p_node)
{
	return p_node ? ::FREE_IMAGE_FORMAT(p_node->m_id) : FIF_UNKNOWN;
}

static const char*
FI_GetFIFExtensionList(PluginNode& node)
{
	return node.m_extension ? node.m_extension : node.m_plugin->extension_proc
		? node.m_plugin->extension_proc() : "";
}

int DLL_CALLCONV
FreeImage_GetFIFCount()
{
	return s_plugins ? s_plugins->m_plugin_map.size() : 0;
}

FREE_IMAGE_FORMAT DLL_CALLCONV
FreeImage_GetFIFFromFormat(const char* format)
{
	return s_plugins ? FI_GetFIFFromPluginNodePtr(
		s_plugins->FindNodeFromFormat(format)) : FIF_UNKNOWN;
}

FREE_IMAGE_FORMAT DLL_CALLCONV
FreeImage_GetFIFFromMime(const char *mime)
{
	return s_plugins ? FI_GetFIFFromPluginNodePtr(
		s_plugins->FindNodeFromMime(mime)) : FIF_UNKNOWN;
}

const PluginNode* DLL_CALLCONV
FreeImageEx_GetPluginNodeFromFIF(::FREE_IMAGE_FORMAT fif)
{
	return s_plugins ? s_plugins->FindNodeFromFIF(fif) : nullptr;
}

FREE_IMAGE_FORMAT DLL_CALLCONV
FreeImage_GetFIFFromFilename(const char *filename)
{
	if(filename)
	{
		const char* extension;
		// get the proper extension if we received a filename
		auto place(strrchr((char*)filename, '.'));

		extension = place ? ++place : filename;

		// look for the extension in the plugin table
		size_t count(FreeImage_GetFIFCount());

		for(int i = 0; i < count; ++i)
			if(const auto node = s_plugins->FindNodeFromFIF(i))
				if(node->m_enabled)
				{
					// compare the format id with the extension
					if(FreeImage_stricmp(node->m_format ? node->m_format
						: node->m_plugin->format_proc(), extension) == 0)
						return ::FREE_IMAGE_FORMAT(i);
					else
					{
						// make a copy of the extension list and split it
						string copy_str(FI_GetFIFExtensionList(*node));

						for(auto token(strtok(&copy_str[0], ",")); token;
							token = strtok({}, ","))
						if(FreeImage_stricmp(token, extension) == 0)
							return ::FREE_IMAGE_FORMAT(i);
					}
				}
	}
	return FIF_UNKNOWN;
}

BOOL DLL_CALLCONV
FreeImage_Validate(FREE_IMAGE_FORMAT fif, FreeImageIO *io, fi_handle handle)
{
	if(s_plugins)
		if(const auto p_node = s_plugins->FindNodeFromFIF(fif))
		{
			const bool validated(p_node && p_node->m_enabled && p_node->m_plugin
				&& p_node->m_plugin->validate_proc
				? p_node->m_plugin->validate_proc(io, handle) : false);
			const long tell(io->tell_proc(handle));

			io->seek_proc(handle, tell, SEEK_SET);
			return validated;
		}
	return {};
}
