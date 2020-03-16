// ==========================================================
// FreeImage Plugin Interface
//
// Design and implementation by
// - Floris van den Berg (flvdberg@wxs.nl)
// - Rui Lopes (ruiglopes@yahoo.com)
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
// ==========================================================

// Modified by FrankHB <frankhb1989@gmail.com>, 2013-06-26:
//	Disabled unused plugins and make placeholders in plugin list in "Source/Plugin.h" and "Source/FreeImage/Plugin.cpp".
// Modified by FrankHB <frankhb1989@gmail.com>, 2013-07-20:
//	Normalized all EOL marker as CR+LF.
//	Removed all spaces at end of lines.
//	Saved as UTF8 + BOM.
// Modified by FrankHB <frankhb1989@gmail.com>, 2013-11-10:
//	Removed unnecessary class name declaration "Plugin".
//	Moved struct "PluginNode" to public header.
//	Removed declaration of function "FreeImage_stricmp".
//	Removed including "Utilities.h".
//	Added typedef declaration of "PluginNode" to keep source compatibility.
// Modified by FrankHB <frankhb1989@gmail.com>, 2020-03-06:
//	Normalized leading spaces to tabs.
//	Unified function declartion syltes. Added space lines between some function declarations.
//	Merged changes from base version 3.18.0, except the declaration of function "FreeImage_stricmp".

#ifdef _MSC_VER
#pragma warning (disable : 4786) // identifier was truncated to 'number' characters
#endif

#ifndef PLUGIN_H
#define PLUGIN_H

#include "FreeImage.h"
#include <memory>

typedef FI_PluginNodeRec PluginNode;

// =====================================================================
//  Internal Plugin List
// =====================================================================

class PluginList
{
public :
	std::map<int, std::unique_ptr<PluginNode>> m_plugin_map;

	PluginList();
	~PluginList();

	FREE_IMAGE_FORMAT
	AddNode(FI_InitProc proc, void *instance = {},
		const char* format = {}, const char *description = {},
		const char* extension = {}, const char *regexpr = {});

	// Make placeholder for unused plugin.
	FREE_IMAGE_FORMAT
	AddFakeNode(const char* format);

	PluginNode*
	FindNodeFromFormat(const char *format);

	PluginNode*
	FindNodeFromMime(const char *mime);

	PluginNode*
	FindNodeFromFIF(int node_id);
};

// ==========================================================
//   Plugin Initialisation Callback
// ==========================================================

void DLL_CALLCONV
FreeImage_OutputMessage(int fif, const char *message, ...);

// ==========================================================
//   Internal functions
// ==========================================================

extern "C" {
	BOOL DLL_CALLCONV
	FreeImage_ValidateFIF(FREE_IMAGE_FORMAT fif, FreeImageIO *io, fi_handle handle);

	void* DLL_CALLCONV
	FreeImage_Open(PluginNode *node, FreeImageIO *io, fi_handle handle, BOOL open_for_reading);

	void DLL_CALLCONV
	FreeImage_Close(PluginNode *node, FreeImageIO *io, fi_handle handle, void *data); // plugin.cpp

	PluginList*
	DLL_CALLCONV FreeImage_GetPluginList(); // plugin.cpp
}

// ==========================================================
//   Internal plugins
// ==========================================================

void DLL_CALLCONV
InitBMP(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitCUT(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitICO(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitIFF(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitJPEG(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitKOALA(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitLBM(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitMNG(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitPCD(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitPCX(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitPNG(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitPNM(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitPSD(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitRAS(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitTARGA(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitTIFF(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitWBMP(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitXBM(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitXPM(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitDDS(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitGIF(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitHDR(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitG3(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitSGI(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitEXR(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitJ2K(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitJP2(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitPFM(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitPICT(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitRAW(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitJNG(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitWEBP(Plugin *plugin, int format_id);

void DLL_CALLCONV
InitJXR(Plugin *plugin, int format_id);

#endif //!PLUGIN_H

