/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Pan - A Newsreader for Gtk+
 * Copyright (C) 2002  Charles Kerr <charles@rebelbase.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

/*********************
**********************  Includes
*********************/

#include <config.h>
#include <iostream>
extern "C" {
  #include <glib/gi18n.h>
  #include <gtk/gtk.h>
}
#include <pan/general/debug.h>
#include <pan/general/foreach.h>
#include <pan/usenet-utils/gnksa.h>
#include "hig.h"
#include "pad.h"
#include "pan-file-entry.h"
#include "profiles-dialog.h"

using namespace pan;

/***
****
***/

namespace
{
  void set_entry (GtkWidget * w, const std::string& str)
  {
    const char * s = str.empty() ? "" : str.c_str();
    gtk_entry_set_text (GTK_ENTRY(w), s);
  }

  void from_entry (GtkWidget * w, std::string& setme)
  {
    StringView v (gtk_entry_get_text (GTK_ENTRY(w)));
    v.trim ();
    setme.assign (v.str, v.len);
  }

  void on_sig_file_toggled (GtkToggleButton * tb, gpointer sensitize)
  {
    gtk_widget_set_sensitive (GTK_WIDGET(sensitize), gtk_toggle_button_get_active(tb));
  }

  GtkWidget* make_servers_combo (const Data& data, const Quark sel)
  {
    const quarks_t servers (data.get_servers ());

    GtkListStore * store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
    int i(0), sel_index (0);
    foreach_const (quarks_t, servers, it)
    {
      const Quark& server (*it);
      if (server == sel)
        sel_index = i;
      ++i;

      int port;
      std::string addr;
      data.get_server_addr (server, addr, port);
  
      GtkTreeIter iter;
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, 0, addr.c_str(), 1, server.c_str(), -1);
    }

    GtkWidget * w = gtk_combo_box_new_with_model (GTK_TREE_MODEL(store));
    GtkCellRenderer * renderer (gtk_cell_renderer_text_new ());
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (w), renderer, true);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (w), renderer, "text", 0, NULL);
    gtk_combo_box_set_active (GTK_COMBO_BOX(w), sel_index);
    return w;
  }
}

ProfileDialog :: ProfileDialog (const Data         & data,
                                const StringView   & profile_name,
                                const Profile      & profile,
                                GtkWindow          * parent)
{
  _root = gtk_dialog_new_with_buttons (_("Posting Profile"), parent,
                                       GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                       GTK_STOCK_APPLY, GTK_RESPONSE_OK,
                                       NULL);
  gtk_dialog_set_default_response (GTK_DIALOG(_root), GTK_RESPONSE_OK); 
  gtk_window_set_role (GTK_WINDOW(_root), "pan-edit-profile-dialog");

  GtkTooltips * ttips = gtk_tooltips_new ();
  g_object_ref_sink_pan (G_OBJECT(ttips));
  g_object_weak_ref (G_OBJECT(_root), (GWeakNotify)g_object_unref, ttips);

  int row (0);
  GtkWidget *t = HIG :: workarea_create ();
  HIG :: workarea_add_section_title (t, &row, _("Profile Information"));
    HIG :: workarea_add_section_spacer (t, row, 1);
    GtkWidget * w = _name_entry = gtk_entry_new ();
    set_entry (w, profile_name);
    HIG :: workarea_add_row (t, &row, _("_Profile Name:"), w);

  HIG :: workarea_add_section_divider (t, &row);
  HIG :: workarea_add_section_title (t, &row, _("Required Information"));
    HIG :: workarea_add_section_spacer (t, row, 2);
    w = _username_entry = gtk_entry_new ();
    set_entry (w, profile.username);
    HIG :: workarea_add_row (t, &row, _("_Full Name:"), w);
    w = _address_entry = gtk_entry_new ();
    set_entry (w, profile.address);
    HIG :: workarea_add_row (t, &row, _("_Email Address:"), w);
    w = _server_combo = make_servers_combo (data, profile.posting_server);
    HIG :: workarea_add_row (t, &row, _("_Post Articles via:"), w);

  HIG :: workarea_add_section_divider (t, &row);
  HIG :: workarea_add_section_title (t, &row, _("Signature"));
    HIG :: workarea_add_section_spacer (t, row, 3);

    w = _signature_file_check = gtk_check_button_new_with_mnemonic (_("_Use a Signature"));
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(w), profile.use_sigfile);
    HIG :: workarea_add_wide_control (t, &row, w);

    w = _signature_file = pan:: file_entry_new (_("Signature File"), GTK_FILE_CHOOSER_ACTION_OPEN);
    g_signal_connect (_signature_file_check, "toggled", G_CALLBACK(on_sig_file_toggled), w);
    file_entry_set (w, profile.signature_file.c_str());
    HIG :: workarea_add_row (t, &row, _("_Signature:"), w);

    GtkListStore * store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_INT);
    GtkTreeIter iter;
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, 0, _("Text File"), 1, Profile::FILE, -1);
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, 0, _("Text"),      1, Profile::TEXT, -1);
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, 0, _("Command"),   1, Profile::COMMAND, -1);
    w = _signature_file_combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL(store));
    g_signal_connect (_signature_file_check, "toggled", G_CALLBACK(on_sig_file_toggled), w);
    GtkCellRenderer * renderer (gtk_cell_renderer_text_new ());
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (w), renderer, TRUE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (w), renderer, "text", 0, NULL);
    int active = 0;
    if (profile.sig_type == profile.TEXT) active = 1;
    if (profile.sig_type == profile.COMMAND) active = 2;
    gtk_combo_box_set_active (GTK_COMBO_BOX(w), active);
    HIG :: workarea_add_row (t, &row, _("Signature _Type:"), w);

  HIG :: workarea_add_section_divider (t, &row);
  HIG :: workarea_add_section_title (t, &row, _("Optional Information"));
    HIG :: workarea_add_section_spacer (t, row, 3);

    w = _msgid_fqdn_entry = gtk_entry_new ();
    set_entry (w, profile.fqdn);
    gtk_tooltips_set_tip (GTK_TOOLTIPS(ttips), w, _("When posting to Usenet, your article's Message-ID contains a domain name.  You can set a custom domain name here, or leave it blank to let Pan use the domain name from your email address."), "");
    HIG :: workarea_add_row (t, &row, _("Message-ID _Domain Name:"), w, NULL);

    w = _attribution_entry = gtk_entry_new ();
    set_entry (w, profile.attribution);
    gtk_tooltips_set_tip (GTK_TOOLTIPS(ttips), w, _("%i for Message-ID\n%a for Author and Address\n%n for Author name\n%d for Date"), "");
    HIG :: workarea_add_row (t, &row, _("_Attribution:"), w, NULL);


    HIG :: workarea_add_section_spacer (t, row, 1);
    w = _extra_headers_tv = gtk_text_view_new ();
    const int columns (60), rows(10);
    PangoFontDescription *pfd (pango_font_description_from_string ("Monospace 10"));
    PangoContext * context = gtk_widget_create_pango_context (w);
    pango_context_set_font_description (context, pfd);
    std::string line (columns, 'A');
    PangoLayout * layout = pango_layout_new (context);
    pango_layout_set_text (layout, line.c_str(), line.size());
    PangoRectangle r;
    pango_layout_get_extents (layout, &r, 0);
    g_object_unref (layout);
    g_object_unref (context);
    pango_font_description_free (pfd);
    gtk_widget_set_size_request (w, PANGO_PIXELS(r.width), PANGO_PIXELS(r.height*rows));
    std::string s;
    foreach_const (Profile::headers_t, profile.headers, it)
      s += it->first + ": " + it->second + "\n";
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW(w)), s.c_str(), s.size());
    GtkWidget * eventbox = gtk_event_box_new ();
    gtk_tooltips_set_tip (GTK_TOOLTIPS(ttips), eventbox, _("Extra headers to be included in your posts, such as\nReply-To: \"Your Name\" <yourname@somewhere.com>\nOrganization: Your Organization"), "");
    GtkWidget * scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(scrolled_window),
                                         GTK_SHADOW_IN);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);
    gtk_container_add (GTK_CONTAINER(scrolled_window), w);
    gtk_container_add (GTK_CONTAINER(eventbox), scrolled_window);
    HIG :: workarea_add_row (t, &row, _("E_xtra Headers:"), eventbox, w);

  on_sig_file_toggled (GTK_TOGGLE_BUTTON(_signature_file_check), _signature_file);
  on_sig_file_toggled (GTK_TOGGLE_BUTTON(_signature_file_check), _signature_file_combo);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(_root)->vbox), t, true, true, 0);
  gtk_widget_show_all (t);

  if (parent != 0) {
    gtk_window_set_transient_for (GTK_WINDOW(_root), parent);
    gtk_window_set_position (GTK_WINDOW(_root), GTK_WIN_POS_CENTER_ON_PARENT);
  }
}

ProfileDialog :: ~ProfileDialog ()
{
}

namespace
{
  void message_dialog_set_text (GtkMessageDialog * dialog,
                                const char * primary,
                                const char * secondary)
  {
#if GTK_CHECK_VERSION(2,6,0)
    gtk_message_dialog_set_markup (dialog, primary);
    gtk_message_dialog_format_secondary_text (dialog, "%s", secondary);
#else
    char * pch = g_strdup_printf ("<b>%s</b>\n\n%s", primary, secondary);
    gtk_message_dialog_set_markup (dialog, pch);
    g_free (pch);
#endif
  }
}

/*static*/ bool
ProfileDialog :: run_until_valid_or_cancel (ProfileDialog& pd)
{
  for (;;)
  {
    const int response (gtk_dialog_run (GTK_DIALOG(pd.root())));

    if (response == GTK_RESPONSE_CANCEL)
      return false;

    std::string name;
    Profile profile;
    pd.get_profile (name, profile);

    if (GNKSA :: check_from (profile.address, true))
    {
      GtkWidget * d = gtk_message_dialog_new (
        GTK_WINDOW(pd.root()),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_CLOSE,
        NULL);
      message_dialog_set_text (GTK_MESSAGE_DIALOG(d),
                               _("Invalid email address."),
                               _("Please use an address of the form joe@somewhere.org"));
      gtk_dialog_run (GTK_DIALOG(d));
      gtk_widget_destroy (d);
      gtk_widget_grab_focus (pd._address_entry);
    }
    else return true;
  }
}

void
ProfileDialog :: get_profile (std::string& profile_name, Profile& profile)
{
  from_entry (_name_entry, profile_name);
  from_entry (_username_entry, profile.username);
  from_entry (_address_entry, profile.address);
  from_entry (_msgid_fqdn_entry, profile.fqdn);
  from_entry (_attribution_entry, profile.attribution);

  profile.use_sigfile = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(_signature_file_check));
  from_entry (file_entry_gtk_entry(_signature_file), profile.signature_file);

  int type;
  GtkTreeIter iter;
  GtkComboBox * combo = GTK_COMBO_BOX (_signature_file_combo);
  gtk_combo_box_get_active_iter (combo, &iter);
  GtkTreeModel * model (gtk_combo_box_get_model (combo));
  gtk_tree_model_get (model, &iter, 1, &type, -1);
  profile.sig_type = type;

  char * pch;
  combo = GTK_COMBO_BOX (_server_combo);
  gtk_combo_box_get_active_iter (combo, &iter);
  model = gtk_combo_box_get_model (combo);
  gtk_tree_model_get (model, &iter, 1, &pch, -1);
  profile.posting_server = pch;
  g_free (pch);

  // extract extra headers from the text view
  profile.headers.clear ();
  GtkTextBuffer * buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW(_extra_headers_tv));
  GtkTextIter start, end;
  gtk_text_buffer_get_bounds (buf, &start, &end);
  char * text = gtk_text_buffer_get_text (buf, &start, &end, false);
  StringView v(text), line;
  while (v.pop_token (line, '\n')) {
    StringView header, value(line);
    if (value.pop_token (header, ':')) {
      header.trim ();
      value.trim ();
      if (!header.empty() && !value.empty())
        profile.headers[header] = value;
    }
  }
  g_free (text);
}

/******
*******
*******
******/

namespace
{
  enum { COL_NAME, N_COLS };

  GtkListStore* new_profile_store (const Profiles& profiles)
  {
    GtkListStore * store = gtk_list_store_new (N_COLS, G_TYPE_STRING);
    typedef std::set<std::string> names_t;
    const names_t profile_names (profiles.get_profile_names ());
    foreach_const (names_t, profile_names, it) {
      GtkTreeIter iter;
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, COL_NAME, it->c_str(), -1);
    }
    return store;
  }

  std::string get_selected_profile (GtkTreeView * view)
  {
    std::string name;
    GtkTreeSelection * selection (gtk_tree_view_get_selection (view));
    GtkTreeModel * model;
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
      char * pch (0);
      gtk_tree_model_get (model, &iter, COL_NAME, &pch, -1);
      if (pch) {
        name = pch;
        g_free (pch);
      }
    }
    return name;
  }

  static void on_add_button (GtkWidget*, gpointer d) { static_cast<ProfilesDialog*>(d)->create_new_profile(); }
  static void on_edit_button (GtkWidget*, gpointer d) { static_cast<ProfilesDialog*>(d)->edit_profile(); }
  static void on_delete_button (GtkWidget*, gpointer d) { static_cast<ProfilesDialog*>(d)->delete_profile(); }
}

void
ProfilesDialog :: rebuild_store ()
{
  _store = new_profile_store (_profiles);
  gtk_tree_view_set_model (GTK_TREE_VIEW(_view), GTK_TREE_MODEL(_store));
  g_object_unref (_store);
}

void
ProfilesDialog :: delete_profile ()
{
  const std::string name (get_selected_profile (GTK_TREE_VIEW (_view)));
  _profiles.delete_profile (name);
  rebuild_store ();
  refresh_buttons ();
}

void
ProfilesDialog :: edit_profile ()
{
  const std::string old_name (get_selected_profile (GTK_TREE_VIEW (_view)));
  if (!old_name.empty())
  {
    Profile profile;
    if (_profiles.get_profile (old_name, profile))
    {
      ProfileDialog d (_data, old_name, profile, GTK_WINDOW(_root));
      const bool do_rebuild = ProfileDialog :: run_until_valid_or_cancel (d);
      if (do_rebuild)
      {
        std::string new_name;
        d.get_profile (new_name, profile);
        _profiles.delete_profile (old_name);
        _profiles.add_profile (new_name, profile);
        rebuild_store ();
        refresh_buttons ();
      }
      gtk_widget_destroy (d.root());
    }
  }
}

void
ProfilesDialog :: create_new_profile ()
{
  Profile profile;
  profile.username = g_get_real_name ();
  /* xgettext: no-c-format */
  profile.attribution = _("On %d, %n wrote:");
  ProfileDialog d (_data, _("New Profile"), profile, GTK_WINDOW(_root));
  const bool do_rebuild = ProfileDialog :: run_until_valid_or_cancel (d);
  if (do_rebuild)
  {
    std::string name;
    d.get_profile (name, profile);
    _profiles.add_profile (name, profile);
    rebuild_store ();
    refresh_buttons ();
  }
  gtk_widget_destroy (d.root());
}

void
ProfilesDialog :: refresh_buttons ()
{
  const std::string name (get_selected_profile (GTK_TREE_VIEW (_view)));
  bool have_sel (!name.empty());
  gtk_widget_set_sensitive (_edit_button, have_sel);
  gtk_widget_set_sensitive (_remove_button, have_sel);
}

namespace
{
  void profiles_tree_view_selection_changed_cb (GtkTreeSelection * selection, gpointer user_data)
  {
    ProfilesDialog *d = (ProfilesDialog *)user_data;
    d->refresh_buttons ();
  }
  void tree_view_row_activated_cb (GtkTreeView        * view,
                                   GtkTreePath        * path,
                                   GtkTreeViewColumn  * col,
                                   gpointer             user_data)
  {
    on_edit_button (NULL, user_data);
  }
}

ProfilesDialog :: ~ProfilesDialog ()
{
}

ProfilesDialog :: ProfilesDialog (const Data& data, Profiles &profiles, GtkWindow *parent):
  _data (data),
  _profiles (profiles)
{
  _root = gtk_dialog_new_with_buttons (_("Posting Profiles"), parent,
                                       GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
                                       NULL);
  gtk_window_set_role (GTK_WINDOW(_root), "pan-profiles-dialog");
  //g_signal_connect (GTK_OBJECT(_root), "response", G_CALLBACK(response_cb), this);

  // workarea
  GtkWidget * hbox = gtk_hbox_new (false, PAD);
  gtk_container_set_border_width (GTK_CONTAINER(hbox), PAD_BIG);
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(_root)->vbox), hbox, true, true, 0);

  // create the list
  GtkWidget * w = _view = gtk_tree_view_new ();
  rebuild_store ();
  GtkCellRenderer * renderer = gtk_cell_renderer_text_new ();
  GtkTreeViewColumn * column = gtk_tree_view_column_new_with_attributes (_("Profiles"), renderer, "text", COL_NAME, NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_NAME);
  gtk_tree_view_append_column (GTK_TREE_VIEW(w), column);
  GtkTreeSelection * selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (w));
  gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
  g_signal_connect (G_OBJECT (selection), "changed",
                    G_CALLBACK (profiles_tree_view_selection_changed_cb), this);
  g_signal_connect (GTK_TREE_VIEW(w), "row-activated",
                    G_CALLBACK (tree_view_row_activated_cb), this);
  w = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(w), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(w), GTK_SHADOW_IN);
  gtk_container_add (GTK_CONTAINER(w), _view);
  gtk_box_pack_start (GTK_BOX (hbox), w, true, true, 0);
  gtk_widget_set_size_request (w, 300, 300);

  // button box
  GtkTooltips * tips = gtk_tooltips_new ();
  g_object_ref_sink_pan (G_OBJECT(tips));
  g_object_weak_ref (G_OBJECT(_root), (GWeakNotify)g_object_unref, tips);
  GtkWidget * bbox = gtk_vbox_new (FALSE, PAD_SMALL);
  gtk_box_pack_start (GTK_BOX (hbox), bbox, false, false, 0);

  // add button
  w = gtk_button_new_from_stock (GTK_STOCK_ADD);
  gtk_box_pack_start (GTK_BOX (bbox), w, false, false, 0);
  gtk_tooltips_set_tip (tips, w, _("Add a New Profile"), NULL);
  gtk_signal_connect (GTK_OBJECT(w), "clicked", GTK_SIGNAL_FUNC(on_add_button), this);

  // edit button
#if GTK_CHECK_VERSION(2,6,0)
  w = gtk_button_new_from_stock (GTK_STOCK_EDIT);
#else
  w = gtk_button_new_from_stock (GTK_STOCK_OPEN);
#endif
  _edit_button = w;
  gtk_box_pack_start (GTK_BOX (bbox), w, false, false, 0);
  gtk_tooltips_set_tip (tips, w, _("Edit the selected profile"), NULL);
  gtk_signal_connect (GTK_OBJECT(w), "clicked", GTK_SIGNAL_FUNC(on_edit_button), this);

  // remove button
  _remove_button = w = gtk_button_new_from_stock (GTK_STOCK_REMOVE);
  gtk_box_pack_start (GTK_BOX (bbox), w, false, false, 0);
  gtk_tooltips_set_tip (tips, w, _("Remove the selected profile"), NULL);
  gtk_signal_connect (GTK_OBJECT(w), "clicked", GTK_SIGNAL_FUNC(on_delete_button), this);

  // set sensitive buttons
  refresh_buttons ();

  gtk_widget_show_all (hbox);

  if (parent != 0) {
    gtk_window_set_transient_for (GTK_WINDOW(_root), parent);
    gtk_window_set_position (GTK_WINDOW(_root), GTK_WIN_POS_CENTER_ON_PARENT);
  }
}