#ifdef __GNUG__
#pragma implementation "wxJPGHandler.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#if wxUSE_IMAGE && wxUSE_LIBJPEG

#include "wxJPGHandler.h"
#include "wx/bitmap.h"
#include "wx/debug.h"
#include "wx/log.h"
#include "wx/app.h"
#include "wx/msgdlg.h"

// NB: Some compilers define boolean type in Windows headers 
//     (e.g. Watcom C++, but not Open Watcom).
//     This causes a conflict with jmorecfg.h header from libjpeg, so we have
//     to make sure libjpeg won't try to define boolean itself. This is done by
//     defining HAVE_BOOLEAN.
#if defined(__WXMSW__) && (defined(__MWERKS__) || (defined(__WATCOMC__) && __WATCOMC__ < 1200))
    #define HAVE_BOOLEAN
    #include <windows.h>
#endif

extern "C"
{
#include "jconfig.h"
#define JCONFIG_INCLUDED
#include "jpeglib.h"
}

#include "wx/filefn.h"
#include "wx/wfstream.h"
#include "wx/intl.h"
#include "wx/module.h"

// For memcpy
#include <string.h>
// For JPEG library error handling
#include <setjmp.h>

#ifdef __SALFORDC__
#undef FAR
#endif

// ----------------------------------------------------------------------------
// types
// ----------------------------------------------------------------------------

// the standard definition of METHODDEF(type) from jmorecfg.h is "static type"
// which means that we can't declare the method functions as extern "C" - the
// compiler (rightfully) complains about the multiple storage classes in
// declaration
//
// so we only add extern "C" when using our own, modified, jmorecfg.h - and use
// whatever we have in the system headers if this is what we use hoping that it
// should be ok (can't do anything else)

#ifdef JPEG_METHOD_LINKAGE
    #define CPP_METHODDEF(type) extern "C" METHODDEF(type)
#else // not using our jmorecfg.h header
    #define CPP_METHODDEF(type) METHODDEF(type)
#endif

//-----------------------------------------------------------------------------
// wxJPGHandler
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxJPGHandler,wxImageHandler)

#if wxUSE_STREAMS

//------------- JPEG Data Source Manager

#define JPEG_IO_BUFFER_SIZE   2048

typedef struct {
    struct jpeg_source_mgr pub;   /* public fields */

    JOCTET* buffer;               /* start of buffer */
    wxInputStream *stream;
} wx_source_mgr;

typedef wx_source_mgr * wx_src_ptr;

// JPEG error manager:

struct wx_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */

  jmp_buf setjmp_buffer;    /* for return to caller */
};

typedef struct wx_error_mgr * wx_error_ptr;




typedef struct {
    struct jpeg_destination_mgr pub;

    wxOutputStream *stream;
    JOCTET * buffer;
} wx_destination_mgr;

typedef wx_destination_mgr * wx_dest_ptr;

#define OUTPUT_BUF_SIZE  4096    /* choose an efficiently fwrite'able size */

#if !wxCHECK_VERSION(2,5,0)
#define wx_init_source my_init_source
#define wx_fill_input_buffer my_fill_input_buffer
#define wx_skip_input_data my_skip_input_data
#define wx_term_source my_term_source
#define wx_error_exit my_error_exit
#define wx_init_destination init_destination
#define wx_empty_output_buffer empty_output_buffer
#define wx_term_destination term_destination
#endif


#ifdef JPEG_METHOD_LINKAGE
extern "C"
{
CPP_METHODDEF(void) wx_init_source ( j_decompress_ptr WXUNUSED(cinfo) );
CPP_METHODDEF(boolean) wx_fill_input_buffer ( j_decompress_ptr cinfo );
CPP_METHODDEF(void) wx_skip_input_data(j_decompress_ptr cinfo, long num_bytes);
CPP_METHODDEF(void) wx_term_source ( j_decompress_ptr cinfo );
CPP_METHODDEF(void) wx_error_exit (j_common_ptr cinfo);
CPP_METHODDEF(void) wx_init_destination (j_compress_ptr cinfo);
CPP_METHODDEF(boolean) wx_empty_output_buffer (j_compress_ptr cinfo);
CPP_METHODDEF(void) wx_term_destination (j_compress_ptr cinfo);
}
#else

CPP_METHODDEF(void) wx_init_source ( j_decompress_ptr WXUNUSED(cinfo) )
{
}

CPP_METHODDEF(boolean) wx_fill_input_buffer ( j_decompress_ptr cinfo )
{
    wx_src_ptr src = (wx_src_ptr) cinfo->src;

    src->pub.next_input_byte = src->buffer;
    src->pub.bytes_in_buffer = src->stream->Read(src->buffer, JPEG_IO_BUFFER_SIZE).LastRead();

    if (src->pub.bytes_in_buffer == 0) // check for end-of-stream
    {
        // Insert a fake EOI marker
        src->buffer[0] = 0xFF;
        src->buffer[1] = JPEG_EOI;
        src->pub.bytes_in_buffer = 2;
    }
    return TRUE;
}

CPP_METHODDEF(void) wx_skip_input_data ( j_decompress_ptr cinfo, long num_bytes )
{
    if (num_bytes > 0)
    {
        wx_src_ptr src = (wx_src_ptr) cinfo->src;

        while (num_bytes > (long)src->pub.bytes_in_buffer)
        {
            num_bytes -= (long) src->pub.bytes_in_buffer;
            src->pub.fill_input_buffer(cinfo);
        }
        src->pub.next_input_byte += (size_t) num_bytes;
        src->pub.bytes_in_buffer -= (size_t) num_bytes;
    }
}

CPP_METHODDEF(void) wx_term_source ( j_decompress_ptr cinfo )
{
    wx_src_ptr src = (wx_src_ptr) cinfo->src;

    if (src->pub.bytes_in_buffer > 0)
        src->stream->SeekI(-(long)src->pub.bytes_in_buffer, wxFromCurrent);
    delete[] src->buffer;
}

/*
 * Here's the routine that will replace the standard error_exit method:
 */

CPP_METHODDEF(void) wx_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a wx_error_mgr struct, so coerce pointer */
  wx_error_ptr myerr = (wx_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  if (cinfo->err->output_message) (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

CPP_METHODDEF(void) wx_init_destination (j_compress_ptr cinfo)
{
    wx_dest_ptr dest = (wx_dest_ptr) cinfo->dest;

    /* Allocate the output buffer --- it will be released when done with image */
    dest->buffer = (JOCTET *)
        (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
        OUTPUT_BUF_SIZE * sizeof(JOCTET));
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}

CPP_METHODDEF(boolean) wx_empty_output_buffer (j_compress_ptr cinfo)
{
    wx_dest_ptr dest = (wx_dest_ptr) cinfo->dest;

    dest->stream->Write(dest->buffer, OUTPUT_BUF_SIZE);
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
    return TRUE;
}

CPP_METHODDEF(void) wx_term_destination (j_compress_ptr cinfo)
{
    wx_dest_ptr dest = (wx_dest_ptr) cinfo->dest;
    size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;
    /* Write any data remaining in the buffer */
    if (datacount > 0)
        dest->stream->Write(dest->buffer, datacount);
}


#endif

void jpeg_wxio_src1( j_decompress_ptr cinfo, wxInputStream& infile )
{
    wx_src_ptr src;

    if (cinfo->src == NULL) {    /* first time for this JPEG object? */
        cinfo->src = (struct jpeg_source_mgr *)
            (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
            sizeof(wx_source_mgr));
        src = (wx_src_ptr) cinfo->src;
    }
    src = (wx_src_ptr) cinfo->src;
    src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
    src->buffer = new JOCTET[JPEG_IO_BUFFER_SIZE];
    src->pub.next_input_byte = NULL; /* until buffer loaded */
    src->stream = &infile;

    src->pub.init_source = wx_init_source;
    src->pub.fill_input_buffer = wx_fill_input_buffer;
    src->pub.skip_input_data = wx_skip_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
    src->pub.term_source = wx_term_source;
} 



// temporarily disable the warning C4611 (interaction between '_setjmp' and
// C++ object destruction is non-portable) - I don't see any dtors here
#ifdef __VISUALC__
    #pragma warning(disable:4611)
#endif /* VC++ */

bool wxJPGHandler::LoadFile( wxImage *image, wxInputStream& stream, bool verbose, int WXUNUSED(index) )
{
    struct jpeg_decompress_struct cinfo;
    struct wx_error_mgr jerr;
    JSAMPARRAY tempbuf;
    unsigned char *ptr;
    unsigned stride;
    
	int maxWidth = image->HasOption(wxT("max_width")) ?
	  image->GetOptionInt(wxT("max_width")) : -1;
	int maxHeight = image->HasOption(wxT("max_height")) ?
	  image->GetOptionInt(wxT("max_height")) : -1;

    image->Destroy();
    cinfo.err = jpeg_std_error( &jerr.pub );
    jerr.pub.error_exit = wx_error_exit;

    if (!verbose) cinfo.err->output_message=NULL;

    /* Establish the setjmp return context for wx_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer)) {
      /* If we get here, the JPEG code has signaled an error.
       * We need to clean up the JPEG object, close the input file, and return.
       */
      if (verbose)
        wxLogError(_("JPEG: Couldn't load - file is probably corrupted."));
      (cinfo.src->term_source)(&cinfo);
      jpeg_destroy_decompress(&cinfo);
      if (image->Ok()) image->Destroy();
      return FALSE;
    }
	
    jpeg_create_decompress( &cinfo );
    jpeg_wxio_src1( &cinfo, stream );
    jpeg_read_header( &cinfo, TRUE );
    cinfo.out_color_space = JCS_RGB;
	if (maxWidth > 0 && maxHeight > 0) {
		while (cinfo.scale_denom < 8 && // 1,2,4,8
			((int)(cinfo.image_width/cinfo.scale_denom)>>1 > maxWidth ||
			(int)(cinfo.image_height/cinfo.scale_denom)>>1 > maxHeight))
		  cinfo.scale_denom = cinfo.scale_denom<<1;
	}
    jpeg_start_decompress( &cinfo );

    image->Create( cinfo.output_width, cinfo.output_height );
    if (!image->Ok()) {
        jpeg_finish_decompress( &cinfo );
        jpeg_destroy_decompress( &cinfo );
        return FALSE;
    }
    image->SetMask( FALSE );
    ptr = image->GetData();
    stride = cinfo.output_width * 3;
    tempbuf = (*cinfo.mem->alloc_sarray)
        ((j_common_ptr) &cinfo, JPOOL_IMAGE, stride, 1 );

    while ( cinfo.output_scanline < cinfo.output_height ) {
        jpeg_read_scanlines( &cinfo, tempbuf, 1 );
        memcpy( ptr, tempbuf[0], stride );
        ptr += stride;
    }
    jpeg_finish_decompress( &cinfo );
    jpeg_destroy_decompress( &cinfo );
    return TRUE;
}


GLOBAL(void) jpeg_wxio_dest1 (j_compress_ptr cinfo, wxOutputStream& outfile)
{
    wx_dest_ptr dest;

    if (cinfo->dest == NULL) {    /* first time for this JPEG object? */
        cinfo->dest = (struct jpeg_destination_mgr *)
            (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
            sizeof(wx_destination_mgr));
    }

    dest = (wx_dest_ptr) cinfo->dest;
    dest->pub.init_destination = wx_init_destination;
    dest->pub.empty_output_buffer = wx_empty_output_buffer;
    dest->pub.term_destination = wx_term_destination;
    dest->stream = &outfile;
} 

bool wxJPGHandler::SaveFile( wxImage *image, wxOutputStream& stream, bool verbose )
{
    struct jpeg_compress_struct cinfo;
    struct wx_error_mgr jerr;
    JSAMPROW row_pointer[1];    /* pointer to JSAMPLE row[s] */
    JSAMPLE *image_buffer;
    int stride;                /* physical row width in image buffer */

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = wx_error_exit;

    if (!verbose) cinfo.err->output_message=NULL;

    /* Establish the setjmp return context for wx_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer))
    {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and return.
         */
         if (verbose)
            wxLogError(_("JPEG: Couldn't save image."));
         jpeg_destroy_compress(&cinfo);
         return FALSE;
    }

    jpeg_create_compress(&cinfo);
    jpeg_wxio_dest1(&cinfo, stream);

    cinfo.image_width = image->GetWidth();
    cinfo.image_height = image->GetHeight();
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);

    // TODO: 3rd parameter is force_baseline, what value should this be?
    // Code says: "If force_baseline is TRUE, the computed quantization table entries
    // are limited to 1..255 for JPEG baseline compatibility."
    // 'Quality' is a number between 0 (terrible) and 100 (very good).
    // The default (in jcparam.c, jpeg_set_defaults) is 75,
    // and force_baseline is TRUE.
    if (image->HasOption(wxT("quality")))
        jpeg_set_quality(&cinfo, image->GetOptionInt(wxT("quality")), TRUE);

    jpeg_start_compress(&cinfo, TRUE);

    stride = cinfo.image_width * 3;    /* JSAMPLEs per row in image_buffer */
    image_buffer = image->GetData();
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &image_buffer[cinfo.next_scanline * stride];
        jpeg_write_scanlines( &cinfo, row_pointer, 1 );
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    return TRUE;
}

#ifdef __VISUALC__
    #pragma warning(default:4611)
#endif /* VC++ */

bool wxJPGHandler::DoCanRead( wxInputStream& stream )
{
    unsigned char hdr[2];

    if ( !stream.Read(hdr, WXSIZEOF(hdr)) )
        return FALSE;

    return hdr[0] == 0xFF && hdr[1] == 0xD8;
}

#endif   // wxUSE_STREAMS

#endif   // wxUSE_LIBJPEG

/// Load image
bool LoadImageFile(wxImage& img, wxInputStream& stream, long type, int index)
{
    wxImageHandler *handler;

    if ( type == wxBITMAP_TYPE_ANY )
    {
        wxList &list = wxImage::GetHandlers();
        for ( wxList::Node* node = list.GetFirst(); node; node = node->GetNext() )
        {
             handler=(wxImageHandler*)node->GetData();
             if ( handler->CanRead(stream) )
                 return handler->LoadFile(&img, stream, true/*verbose*/, index);
        }
        wxLogWarning( _("No handler found for image type.") );
        return false;
    }

    handler = wxImage::FindHandler(type);
    if (handler == 0)
    {
        wxLogWarning( _("No image handler for type %d defined."), type );
        return false;
    }

    return handler->LoadFile(&img, stream, true/*verbose*/, index);
}

bool LoadImageFile(wxImage& img, const wxString& filename, long type, int index)
{
#if wxUSE_STREAMS
    if (wxFileExists(filename))
    {
        wxFileInputStream stream(filename);
        wxBufferedInputStream bstream( stream );
        return LoadImageFile(img, bstream, type, index);
    }
    else
    {
        wxLogError( _("Can't load image from file '%s': file does not exist."), filename.c_str() );
        return false;
    }
#else // !wxUSE_STREAMS
    return false;
#endif // wxUSE_STREAMS
}




