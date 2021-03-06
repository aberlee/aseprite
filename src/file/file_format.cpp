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

#include "file/file_format.h"
#include "file/format_options.h"

#include <algorithm>

FileFormat::FileFormat()
{
}

FileFormat::~FileFormat()
{
}

const char* FileFormat::name() const
{
  return onGetName();
}

const char* FileFormat::extensions() const
{
  return onGetExtensions();
}

bool FileFormat::load(FileOp* fop)
{
  ASSERT(support(FILE_SUPPORT_LOAD));
  return onLoad(fop);
}

bool FileFormat::save(FileOp* fop)
{
  ASSERT(support(FILE_SUPPORT_SAVE));
  return onSave(fop);
}

bool FileFormat::postLoad(FileOp* fop)
{
  return onPostLoad(fop);
}

void FileFormat::destroyData(FileOp* fop)
{
  onDestroyData(fop);
}
