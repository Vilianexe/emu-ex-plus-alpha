/******************************************************************************/
/* Mednafen - Multi-system Emulator                                           */
/******************************************************************************/
/* CDAFReader_Vorbis.cpp:
**  Copyright (C) 2010-2016 Mednafen Team
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <mednafen/mednafen.h>
#include "CDAFReader.h"
#include "CDAFReader_Vorbis.h"

#if defined ARCH_X86 || defined __aarch64__
#define CONFIG_PACKAGE_LIBVORBIS
#endif

#ifdef CONFIG_PACKAGE_LIBVORBIS
#include <vorbis/vorbisfile.h>
#else
#include <tremor/ivorbisfile.h>
#endif

#include <imagine/io/api/vorbis.hh>

class CDAFReader_Vorbis final : public CDAFReader
{
 public:
 CDAFReader_Vorbis(IO *fp);
 ~CDAFReader_Vorbis();

 uint64 Read_(int16 *buffer, uint64 frames) override;
 bool Seek_(uint64 frame_offset) override;
 uint64 FrameCount(void) override;

 private:
 OggVorbis_File ovfile;
 IO *fw;
};


static size_t iov_read_func(void *ptr, size_t size, size_t nmemb, void *user_data)
{
 Stream *fw = (Stream*)user_data;

 if(!size)
  return(0);

 try
 {
  return fw->read(ptr, size * nmemb, false) / size;
 }
 catch(...)
 {
  return(0);
 }
}

static int iov_seek_func(void *user_data, ogg_int64_t offset, int whence)
{
 Stream *fw = (Stream*)user_data;

 try
 {
  fw->seek(offset, whence);
  return(0);
 }
 catch(...)
 {
  return(-1);
 }
}

static int iov_close_func(void *user_data)
{
 Stream *fw = (Stream*)user_data;

 try
 {
  fw->close();
  return(0);
 }
 catch(...)
 {
  return EOF;
 }
}

static long iov_tell_func(void *user_data)
{
 Stream *fw = (Stream*)user_data;

 try
 {
  return fw->tell();
 }
 catch(...)
 {
  return(-1);
 }
}

CDAFReader_Vorbis::CDAFReader_Vorbis(IO *fp) : fw(fp)
{
 ov_callbacks cb = IOAPI::vorbisNoClose;

 if(ov_open_callbacks(fp, &ovfile, NULL, 0, cb))
  throw(0);
}

CDAFReader_Vorbis::~CDAFReader_Vorbis()
{
 ov_clear(&ovfile);
}

uint64 CDAFReader_Vorbis::Read_(int16 *buffer, uint64 frames)
{
 uint8 *tw_buf = (uint8 *)buffer;
 int cursection = 0;
 long toread = frames * sizeof(int16) * 2;

 while(toread > 0)
 {
	#ifdef MSB_FIRST
	int endianPack = 1;
	#else
	int endianPack = 0;
	#endif
  long didread =
	 #ifdef CONFIG_PACKAGE_LIBVORBIS
	 ov_read(&ovfile, (char*)tw_buf, toread, endianPack, 2, 1, &cursection);
	 #else
	 ov_read(&ovfile, (char*)tw_buf, toread, &cursection);
	 #endif

  if(didread == 0)
   break;

  tw_buf = (uint8 *)tw_buf + didread;
  toread -= didread;
 }

 return(frames - toread / sizeof(int16) / 2);
}

bool CDAFReader_Vorbis::Seek_(uint64 frame_offset)
{
 ov_pcm_seek(&ovfile, frame_offset);
 return(true);
}

uint64 CDAFReader_Vorbis::FrameCount(void)
{
 return(ov_pcm_total(&ovfile, -1));
}

CDAFReader* CDAFR_Vorbis_Open(IO* fp)
{
 return new CDAFReader_Vorbis(fp);
}