/////////////////////////////////////////////////////////////////////////////
// Name:        wxThumbnailFactory.h
// Purpose:     wxThumbnailFactory class
// Author:      Alex Thuering
// Created:		15.02.2003
// RCS-ID:      $Id: ThumbnailFactory.h,v 1.4 2004/11/08 20:18:06 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _wxThumbnailFactory_h_
#define _wxThumbnailFactory_h_

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/thread.h>
#include <wx/dynarray.h>

enum ThumbType
{
  THUMBNAIL_NORMAL,
  THUMBNAIL_FAILED
};

class ThumbInfo;

WX_DEFINE_ARRAY(ThumbInfo*, ThumbInfoArray);

class wxThumbnailFactory: public wxThread
{
  public:
	static void InitGnome(const char* appName, const char* appVersion,
	  int argc, char** argv);
	static wxImage GetThumbnail(wxWindow* parent,
	  wxString filename, int width, int height);
	static void ClearQueue(wxWindow* parent);
	
  protected:
	static ThumbInfoArray m_queue;
	static wxMutex m_queueMutex;
	static wxThumbnailFactory* thread;
	static int canGenerate;
	static int maxFileSize;
	wxThread::ExitCode Entry();
	static ThumbInfo* GetThumbInfo(wxString filename, wxWindow* parent, int width, int height);
	static wxImage LoadThumbnail(ThumbInfo& info, ThumbType type = THUMBNAIL_NORMAL);
	static bool CanThumbnail(ThumbInfo& info);
	static wxString GetThumbPath(ThumbInfo& info, ThumbType type = THUMBNAIL_NORMAL);
	static wxImage GenerateThumbnail(ThumbInfo& info);
	static bool SaveThumbnail(ThumbInfo& info, wxImage& image);
	static void CreateFailedThumbnail(ThumbInfo& info);
};

#endif _wxThumbnailFactory_h_
