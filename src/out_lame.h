/* MuSE - Multiple Streaming Engine
 * Copyright (C) 2000-2002 Denis Rojo aka jaromil <jaromil@dyne.org>
 *
 * This source code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License as published 
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * Please refer to the GNU Public License for more details.
 *
 * You should have received a copy of the GNU Public License along with
 * this source code; if not, write to:
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * "$Id: out_lame.h,v 1.2 2004/01/24 17:22:52 jaromil Exp $"
 *
 */

#ifndef __OUT_LAME_H__
#define __OUT_LAME_H__

#include <outchannels.h>
#include <config.h>
#ifdef HAVE_LAME

extern "C" {
#include <lame/lame.h>
}

class OutLame : public OutChannel {
  
 private:
  lame_global_flags *enc_flags;
  int16_t pcm[OUT_CHUNK<<5];


 public:
  OutLame();
  ~OutLame();
  int encode();
  bool init();
  void flush();
  bool apply_profile();
  
};


#endif
#endif
