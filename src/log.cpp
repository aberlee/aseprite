/* ASE - Allegro Sprite Editor
 * Copyright (C) 2001-2011  David Capello
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"

#include <allegro/file.h>
#include <allegro/unicode.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include "gui/base.h"

#include "app.h"
#include "log.h"
#include "resource_finder.h"

/* DOS and Windows needs "log" files (because their poor stderr support) */
#if defined ALLEGRO_DOS || defined ALLEGRO_WINDOWS
#  define NEED_LOG
#endif

/* in DOS, print in stderr is like print in the video screen */
#if defined ALLEGRO_DOS
#  define NO_STDERR
#endif

#ifdef NEED_LOG
/* log file info */
static std::string log_filename;
static FILE *log_fileptr = NULL;
#endif

LoggerModule::LoggerModule(bool verbose)
  : m_verbose(verbose)
{
#ifdef NEED_LOG
  ResourceFinder rf;
  rf.findInBinDir("aseprite.log");
  log_filename = rf.first();
#endif
}

LoggerModule::~LoggerModule()
{
  PRINTF("Logger module: shutting down (this is the last line)\n");

#ifdef NEED_LOG
  if (log_fileptr) {
    fclose(log_fileptr);
    log_fileptr = NULL;
  }
#endif
}

//////////////////////////////////////////////////////////////////////

void verbose_printf(const char *format, ...)
{
  if (!App::instance()->getLogger()->isVerbose())
    return;

#ifdef NEED_LOG
  if (!log_fileptr)
    if (log_filename.size() > 0)
      log_fileptr = fopen(log_filename.c_str(), "w");

  if (log_fileptr)
#endif
    {
      va_list ap;
      va_start(ap, format);

#ifndef NO_STDERR
      vfprintf(stderr, format, ap);
      fflush(stderr);
#endif

#ifdef NEED_LOG
      vfprintf(log_fileptr, format, ap);
      fflush(log_fileptr);
#endif

      va_end(ap);
    }
}
