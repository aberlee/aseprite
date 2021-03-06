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

#include <allegro/unicode.h>
#include <stdarg.h>

#include "base/bind.h"
#include "gui/box.h"
#include "gui/button.h"
#include "gui/hook.h"
#include "gui/list.h"
#include "gui/system.h"
#include "gui/theme.h"
#include "gui/widget.h"
#include "modules/gui.h"
#include "widgets/groupbut.h"

static JWidget find_selected(JWidget widget);
static int select_button(JWidget widget, int index);

static bool radio_change_hook(JWidget vbox);

JWidget group_button_new(int w, int h, int first_selected, ...)
{
  Box* vbox = new Box(JI_VERTICAL | JI_HOMOGENEOUS);
  Box* hbox = NULL;
  RadioButton* radio;
  int x, y, icon;
  va_list ap;
  int c = 0;
  char buf[256];

  va_start(ap, first_selected);

  jwidget_noborders(vbox);

  for (y=0; y<h; y++) {
    if (w > 1) {
      hbox = new Box(JI_HORIZONTAL | JI_HOMOGENEOUS);
      jwidget_noborders(hbox);
    }

    for (x=0; x<w; x++) {
      icon = va_arg(ap, int);

      radio = radio_button_new(vbox->id+0x1000,
			       x ==   0 && y ==   0 ? 2: 0,
			       x == w-1 && y ==   0 ? 2: 0,
			       x ==   0 && y == h-1 ? 2: 0,
			       x == w-1 && y == h-1 ? 2: 0);

      radio->user_data[1] = (void*)c;

      usprintf(buf, "radio%d", c);
      radio->setName(buf);

      if (icon >= 0)
	set_gfxicon_to_button(radio, icon, icon+1, icon, JI_CENTER | JI_MIDDLE);

      radio->Click.connect(Bind<bool>(&radio_change_hook, vbox));

      if (c == first_selected)
	radio->setSelected(true);

      if (hbox)
	hbox->addChild(radio);
      else
	vbox->addChild(radio);

      c++;
    }

    if (hbox)
      vbox->addChild(hbox);
  }

  va_end(ap);

  return vbox;
}

int group_button_get_selected(JWidget group)
{
  JWidget sel = find_selected(group);

  if (sel)
    return (long)sel->user_data[1];
  else
    return -1;
}

void group_button_select(JWidget group, int index)
{
  JWidget sel = find_selected(group);

  if (!sel || (long)sel->user_data[1] != index) {
    sel->setSelected(false);
    select_button(group, index);
  }
}

static JWidget find_selected(JWidget widget)
{
  JWidget sel;
  JLink link;

  if (widget->isSelected())
    return widget;
  else {
    JI_LIST_FOR_EACH(widget->children, link)
      if ((sel = find_selected(reinterpret_cast<JWidget>(link->data))))
	return sel;

    return NULL;
  }
}

static int select_button(JWidget widget, int index)
{
  JLink link;

  if (widget->type == JI_RADIO) {
    if ((long)widget->user_data[1] == index) {
      widget->setSelected(true);
      return true;
    }
  }
  else {
    JI_LIST_FOR_EACH(widget->children, link)
      if (select_button(reinterpret_cast<JWidget>(link->data), index))
	return true;
  }

  return false;
}

static bool radio_change_hook(JWidget vbox)
{
  jwidget_emit_signal(vbox, SIGNAL_GROUP_BUTTON_CHANGE);
  return true;
}
