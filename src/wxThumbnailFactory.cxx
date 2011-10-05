/////////////////////////////////////////////////////////////////////////////
// Name:        wxThumbnailFactory.cxx
// Purpose:     wxThumbnailFactory class
// Author:      Alex Thuering
// Created:		15.02.2003
// RCS-ID:      $Id: ThumbnailFactory.cpp,v 1.15 2005/08/15 18:35:36 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wxThumbnailFactory.h"
#include "wxThumb.h"
#include "wxJPGHandler.h"
#include <wx/dir.h>
#include <wx/app.h>
#include <wx/wx.h>
#include <sys/stat.h>

#ifdef __WXMAC__
#include <sys/types.h>
#include <sys/wait.h>
#endif

#ifdef GNOME2
#include <glib.h>
#include <libgnomevfs/gnome-vfs.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkdrawable.h>
#include <libgnomeui/gnome-ui-init.h>
#include <libgnomeui/gnome-thumbnail.h>
static GnomeThumbnailFactory* thumbnail_factory = NULL;
#else
#define THUMBNAILS_DIR wxGetHomeDir() + wxFILE_SEP_PATH + _T(".thumbnails2")
const wxString thumbnailerCmd = _T("totem-video-thumbnailer \"$FILE_IN\" \"$FILE_OUT\"");
#ifdef __WXGTK__
#include <sys/types.h>
#include <sys/wait.h>
#endif
#endif

#ifdef __WXMSW__
#undef  wxBITMAP
#define wxBITMAP(icon) wxBitmap(icon##_xpm)
#endif


class ThumbInfo
{
  public:
	ThumbInfo(wxString filename, wxWindow* parent, int width, int height)
	{
	  this->filename = filename;
	  this->parent = parent;
	  this->width = width;
	  this->height = height;
#ifdef GNOME2
	  pixbuf = NULL;
#endif
	}
	
	~ThumbInfo()
	{
#ifdef GNOME2
	  if (pixbuf)
		g_object_unref(pixbuf);
#endif
	}
	
	wxString filename;
	wxWindow* parent;
	time_t mtime;
	int width;
	int height;
	wxString mimeType;
#ifdef GNOME2
	char* uri;
	char* mime_type;
	GdkPixbuf* pixbuf;
#else
	wxString uri;
#endif
};

ThumbInfoArray wxThumbnailFactory::m_queue;
wxMutex wxThumbnailFactory::m_queueMutex;
wxThumbnailFactory* wxThumbnailFactory::thread = NULL;
#ifdef GNOME2
int wxThumbnailFactory::canGenerate = 1;
#else
int wxThumbnailFactory::canGenerate = -1;
#endif
int wxThumbnailFactory::maxFileSize = 102400; // immediately render files with size < 100K

void wxThumbnailFactory::InitGnome(const char* appName, const char* appVersion,
  int argc, char** argv)
{
#ifdef GNOME2
  gnome_init(appName, appVersion, argc, argv);
  gnome_vfs_init();
  thumbnail_factory = gnome_thumbnail_factory_new(GNOME_THUMBNAIL_SIZE_NORMAL);
#endif
}

wxImage wxThumbnailFactory::GetThumbnail(wxWindow* parent,
 wxString filename, int width, int height)
{
  wxImage img;
  
  if (!filename.length())
	return img;
  
  ThumbInfo* info = GetThumbInfo(filename, parent, width, height);
  img = LoadThumbnail(*info);
  
  delete info;
  return img;
}

wxThread::ExitCode wxThumbnailFactory::Entry()
{
  while (1)
  {
	m_queueMutex.Lock();
	if (!m_queue.Count())
	  break;
	ThumbInfo* info = m_queue.Item(0);
	m_queue.RemoveAt(0);
	m_queueMutex.Unlock();
	
	wxImage img = LoadThumbnail(*info);
	if (!img.Ok())
	{
	  img = GenerateThumbnail(*info);
	  if (img.Ok())
		SaveThumbnail(*info, img);
	  else
		CreateFailedThumbnail(*info);
	}
	wxCommandEvent evt(EVT_COMMAND_THUMBNAILS_THUMB_CHANGED);
	evt.SetString(info->filename);
	evt.SetClientData(new wxImage(img));
	wxPostEvent(info->parent, evt);
	delete info;
  }
  thread = NULL;
  m_queueMutex.Unlock();
  return 0;
}

void wxThumbnailFactory::ClearQueue(wxWindow* parent)
{
  wxMutexLocker locker(m_queueMutex);
  for (int i=0; i<(int)m_queue.Count();)
  {
	if (m_queue[i]->parent == parent)
	{
	  delete m_queue[i];
	  m_queue.RemoveAt(i);
	}
	else
	  i++;
  }
}


#ifdef GNOME2
#else
#include "thumb_md5.h"
#endif

ThumbInfo* wxThumbnailFactory::GetThumbInfo(wxString filename, wxWindow* parent,
  int width, int height)
{
  ThumbInfo* info = new ThumbInfo(filename, parent, width, height);
  if (wxImage::FindHandler(filename.AfterLast('.').Lower(), -1))
	info->mimeType = _T("image");
  else
	info->mimeType = _T("video/mpeg");
#ifdef GNOME2
  info->uri = gnome_vfs_get_uri_from_local_path(info->filename.mb_str());
  // get mtime & mime_type
  info->mtime = 0;
  GnomeVFSFileInfo *file_info = gnome_vfs_file_info_new();
  gnome_vfs_get_file_info(info->uri, file_info,
	(GnomeVFSFileInfoOptions) (GNOME_VFS_FILE_INFO_FOLLOW_LINKS));
  if (file_info->valid_fields & GNOME_VFS_FILE_INFO_FIELDS_MTIME)
	info->mtime = file_info->mtime;
  info->mime_type = "video/mpeg";
  gnome_vfs_file_info_unref(file_info);
#else
  info->uri = _T("file://");
  info->uri += wxString(escape_string(info->filename.mb_str()), *wxConvCurrent);
#endif
  return info;
}

wxString wxThumbnailFactory::GetThumbPath(ThumbInfo& info, ThumbType type)
{
#ifdef GNOME2
  char* thumbnail_path =
    gnome_thumbnail_factory_lookup(thumbnail_factory, info.uri, info.mtime);
  if (!thumbnail_path || thumbnail_path[0] != '/')
	return wxEmptyString;
  return wxString(thumbnail_path, *wxConvCurrent);
#else
  unsigned char digest[16];
  thumb_md5(info.uri.mb_str(), digest);
  wxString md5 = wxString(thumb_digest_to_ascii(digest), *wxConvCurrent);
  wxString file = md5 + _T(".png");
  wxString dir = THUMBNAILS_DIR;
  dir += wxFILE_SEP_PATH;
  dir += type == THUMBNAIL_FAILED ? _T("fail") : _T("normal");
  return dir + wxFILE_SEP_PATH + file;
#endif
}

wxImage wxThumbnailFactory::LoadThumbnail(ThumbInfo& info, ThumbType type)
{
  wxImage img;
  
  if (info.mimeType == _T("image"))
  {
	wxStructStat fInfo;
	if (wxStat(info.filename, &fInfo) == 0 && fInfo.st_size < maxFileSize)
	  img = GenerateThumbnail(info);
	return img;
  }
  
  // load thumbnail for video
  wxString thumbPath = GetThumbPath(info, type);
#ifdef GNOME2
  GdkPixbuf* pixbuf = NULL;
  if (thumbPath.Length())
	pixbuf = gdk_pixbuf_new_from_file(thumbPath.mb_str(), NULL);
  if (pixbuf)
  {
	if (gnome_thumbnail_is_valid(pixbuf, info.uri, info.mtime))
	{
	  wxLogNull log;
	  img.LoadFile(thumbPath); // img = pixbuf2image(pixbuf); failed on some systems
	}
	g_object_unref(pixbuf);
  }
#else
  wxLogNull log;
  img.LoadFile(thumbPath);
#endif
  return img;
}

bool wxThumbnailFactory::CanThumbnail(ThumbInfo& info)
{
  if (info.mimeType == _T("image"))
	return true;
#ifdef GNOME2
  return gnome_thumbnail_factory_can_thumbnail(
    thumbnail_factory, info.uri, info.mime_type, info.mtime);
#else
  if (canGenerate == -1)
  {
	wxLogNull log;
#ifdef __WXMSW__
	canGenerate =
	  wxExecute(thumbnailerCmd.BeforeFirst(wxT(' ')), wxEXEC_SYNC) == 1;
#else
	long pid = wxExecute(thumbnailerCmd.BeforeFirst(wxT(' ')));
	int status;
	waitpid(pid, &status, 0);
	canGenerate = WEXITSTATUS(status) == 1;
#endif
	if (canGenerate)
	{
	  wxMkdir(THUMBNAILS_DIR);
	  wxMkdir(THUMBNAILS_DIR + wxFILE_SEP_PATH + _T("normal"));
	  wxMkdir(THUMBNAILS_DIR + wxFILE_SEP_PATH + _T("fail"));
	}
  }
  if (!canGenerate)
	return false;
  wxString thumbPath = GetThumbPath(info, THUMBNAIL_FAILED);
  return !wxFileExists(thumbPath);
#endif
}

wxImage wxThumbnailFactory::GenerateThumbnail(ThumbInfo& info)
{
  wxImage img;
  if (info.mimeType == _T("image"))
  {
	wxLogNull log;
	img.Create(1,1);
	img.SetOption(_T("max_width"), info.width);
	img.SetOption(_T("max_height"), info.height);
	LoadImageFile(img, info.filename);
	return img;
  }
#ifdef GNOME2
  info.pixbuf = gnome_thumbnail_factory_generate_thumbnail(
    thumbnail_factory, info.uri, info.mime_type);
  if (info.pixbuf)
	img.Create(1,1);//img = pixbuf2image(info.pixbuf); failed on some systems
#else
  // video - use totem-video-thumbnailer to create thumbnails
  wxString thumbPath = GetThumbPath(info);
  wxString cmd = thumbnailerCmd;
  cmd.Replace(_T("$FILE_IN"), info.filename);
  cmd.Replace(_T("$FILE_OUT"), thumbPath);
  wxLogNull log;
#ifdef __WXMSW__
  wxExecute(cmd, wxEXEC_SYNC);
#else
  long pid = wxExecute(cmd);
  waitpid(pid, NULL, 0);
#endif
  img.LoadFile(thumbPath);
#endif
  return img;
}

bool wxThumbnailFactory::SaveThumbnail(ThumbInfo& info, wxImage& image)
{
  if (info.mimeType == _T("image"))
	return true;
#ifdef GNOME2
  gnome_thumbnail_factory_save_thumbnail(thumbnail_factory,
    info.pixbuf, info.uri, info.mtime);
  image.LoadFile(GetThumbPath(info)); // see comment in GenerateThumbnail
  return true;
#else
  return image.SaveFile(GetThumbPath(info), wxBITMAP_TYPE_PNG);
#endif
}

void wxThumbnailFactory::CreateFailedThumbnail(ThumbInfo& info)
{
  if (info.mimeType == _T("image"))
	return;
#ifdef GNOME2
  gnome_thumbnail_factory_create_failed_thumbnail(
    thumbnail_factory, info.uri, info.mtime);
#else
  wxImage image(1,1);
  image.SaveFile(GetThumbPath(info, THUMBNAIL_FAILED), wxBITMAP_TYPE_PNG);
  wxRemoveFile(GetThumbPath(info));
#endif
}
