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

#ifndef UI_CONTEXT_H_INCLUDED
#define UI_CONTEXT_H_INCLUDED

#include "context.h"

class UIContext : public Context
{
public:
  static UIContext* instance() { return m_instance; }

  UIContext();
  virtual ~UIContext();

  virtual bool isUiAvailable() const { return true; }

protected:
  virtual void onAddDocument(Document* document);
  virtual void onRemoveDocument(Document* document);
  virtual void onSetActiveDocument(Document* document);

private:
  static UIContext* m_instance;

};

#endif
