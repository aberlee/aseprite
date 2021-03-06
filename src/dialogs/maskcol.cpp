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

#include "app.h"
#include "app/color.h"
#include "app/color_utils.h"
#include "base/bind.h"
#include "document.h"
#include "document_wrappers.h"
#include "gui/box.h"
#include "gui/button.h"
#include "gui/frame.h"
#include "gui/hook.h"
#include "gui/label.h"
#include "gui/slider.h"
#include "gui/widget.h"
#include "ini_file.h"
#include "modules/editors.h"
#include "modules/gui.h"
#include "raster/image.h"
#include "raster/mask.h"
#include "raster/sprite.h"
#include "undo/undo_history.h"
#include "undoers/set_mask.h"
#include "util/misc.h"
#include "widgets/color_bar.h"
#include "widgets/color_button.h"

static ColorButton* button_color;
static CheckBox* check_preview;
static Slider* slider_tolerance;

static void button_1_command(Widget* widget, const DocumentReader& document);
static void button_2_command(Widget* widget, const DocumentReader& document);

static Mask* gen_mask(const Sprite* sprite);
static void mask_preview(const DocumentReader& document);

void dialogs_mask_color(Document* document)
{
  DocumentReader documentReader(document);
  Sprite* sprite = document->getSprite();
  Box* box1, *box2, *box3, *box4;
  Widget* label_color;
  Button* button_1;
  Button* button_2;
  JWidget label_tolerance;
  Button* button_ok;
  Button* button_cancel;
  Image *image;

  if (!App::instance()->isGui() || !sprite)
    return;

  image = sprite->getCurrentImage();
  if (!image)
    return;

  FramePtr window(new Frame(false, "Mask by Color"));
  box1 = new Box(JI_VERTICAL);
  box2 = new Box(JI_HORIZONTAL);
  box3 = new Box(JI_HORIZONTAL);
  box4 = new Box(JI_HORIZONTAL | JI_HOMOGENEOUS);
  label_color = new Label("Color:");
  button_color = new ColorButton
   (get_config_color("MaskColor", "Color",
		     app_get_colorbar()->getFgColor()),
    sprite->getImgType());
  button_1 = new Button("1");
  button_2 = new Button("2");
  label_tolerance = new Label("Tolerance:");
  slider_tolerance = new Slider(0, 255, get_config_int("MaskColor", "Tolerance", 0));
  check_preview = new CheckBox("&Preview");
  button_ok = new Button("&OK");
  button_cancel = new Button("&Cancel");

  if (get_config_bool("MaskColor", "Preview", true))
    check_preview->setSelected(true);

  button_1->Click.connect(Bind<void>(&button_1_command, button_1, Ref(documentReader)));
  button_2->Click.connect(Bind<void>(&button_2_command, button_2, Ref(documentReader)));
  button_ok->Click.connect(Bind<void>(&Frame::closeWindow, window.get(), button_ok));
  button_cancel->Click.connect(Bind<void>(&Frame::closeWindow, window.get(), button_cancel));

  button_color->Change.connect(Bind<void>(&mask_preview, Ref(documentReader)));
  slider_tolerance->Change.connect(Bind<void>(&mask_preview, Ref(documentReader)));
  check_preview->Click.connect(Bind<void>(&mask_preview, Ref(documentReader)));

  jwidget_magnetic(button_ok, true);
  jwidget_expansive(button_color, true);
  jwidget_expansive(slider_tolerance, true);
  jwidget_expansive(box2, true);

  window->addChild(box1);
  box1->addChild(box2);
  box1->addChild(box3);
  box1->addChild(check_preview);
  box1->addChild(box4);
  box2->addChild(label_color);
  box2->addChild(button_color);
  box2->addChild(button_1);
  box2->addChild(button_2);
  box3->addChild(label_tolerance);
  box3->addChild(slider_tolerance);
  box4->addChild(button_ok);
  box4->addChild(button_cancel);

  // Default position
  window->remap_window();
  window->center_window();

  // Mask first preview
  mask_preview(documentReader);

  // Load window configuration
  load_window_pos(window, "MaskColor");

  // Open the window
  window->open_window_fg();

  if (window->get_killer() == button_ok) {
    DocumentWriter documentWriter(documentReader);
    undo::UndoHistory* undo = document->getUndoHistory();
    Mask* mask;

    /* undo */
    if (undo->isEnabled()) {
      undo->setLabel("Mask by Color");
      undo->setModification(undo::DoesntModifyDocument);
      undo->pushUndoer(new undoers::SetMask(undo->getObjects(), document));
    }

    /* change the mask */
    mask = gen_mask(sprite);
    document->setMask(mask);
    mask_free(mask);

    set_config_color("MaskColor", "Color", button_color->getColor());
    set_config_int("MaskColor", "Tolerance", slider_tolerance->getValue());
    set_config_bool("MaskColor", "Preview", check_preview->isSelected());
  }

  /* update boundaries and editors */
  document->generateMaskBoundaries();
  update_screen_for_document(document);

  /* save window configuration */
  save_window_pos(window, "MaskColor");
}

static void button_1_command(Widget* widget, const DocumentReader& document)
{
  button_color->setColor(app_get_colorbar()->getFgColor());
  mask_preview(document);
}

static void button_2_command(Widget* widget, const DocumentReader& document)
{
  button_color->setColor(app_get_colorbar()->getBgColor());
  mask_preview(document);
}

static Mask *gen_mask(const Sprite* sprite)
{
  int xpos, ypos, color, tolerance;

  const Image* image = sprite->getCurrentImage(&xpos, &ypos, NULL);

  color = color_utils::color_for_image(button_color->getColor(), sprite->getImgType());
  tolerance = slider_tolerance->getValue();

  Mask* mask = mask_new();
  mask_by_color(mask, image, color, tolerance);
  mask_move(mask, xpos, ypos);

  return mask;
}

static void mask_preview(const DocumentReader& document)
{
  if (check_preview->isSelected()) {
    Mask* mask = gen_mask(document->getSprite());
    {
      DocumentWriter documentWriter(document);
      documentWriter->generateMaskBoundaries(mask);
    }
    mask_free(mask);

    update_screen_for_document(document);
  }
}
