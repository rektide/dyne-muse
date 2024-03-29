/* A Gui in gtk+ for MuSE - Multiple Streaming Engine
 * Copyright (C) 2002-2004 nightolo <night@autistici.org>
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
 */

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <gtk/gtk.h>

#include <jutils.h>
#include <jmixer.h>

#include <gen.h>
#include <utils.h>
#include <filedump.h>
#include <xmlprofile.h>
#include <encoder.h>

#include <config.h>

extern int lameid, oggid;

struct encdata *enc_new(char *text)
{
	GtkWidget *verbox, *tabbola;
	GtkWidget *tmpbar, *tmplabel;
	GtkWidget *tmpbox;
	GtkWidget *scaled;
	GtkWidget *ok;
	/*gchar *text;*/
	bool lame = true;
	struct encdata *enc = NULL;
	GList *listola = NULL; /* recycling */

	/* init area */
	enc = (struct encdata *) g_malloc(sizeof(struct encdata));
	/* init area end */
	
	if(!(strcmp(text, "Lame"))) {
		lame = true;
		lameid = mixer->create_enc(MP3);
		enc->outchan = mixer->get_enc(lameid);

		tmplabel = gtk_label_new(_("Configure Lame Encoder"));
	} else {
		oggid = mixer->create_enc(OGG);
		enc->outchan = mixer->get_enc(oggid);
		tmplabel= gtk_label_new(_("Configure Ogg/Vorbis Encoder"));
	}

	enc->outchan->start();

	verbox = gtk_vbox_new(FALSE, 12);
	gtk_box_pack_start(GTK_BOX(verbox), tmplabel, FALSE, FALSE, 0);
	enc->verbox = verbox;
	
	/*tmplabel = GTK_BIN(w)->child;
	gtk_label_get(GTK_LABEL(tmplabel), &text);*/
	
	/*winenc = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(winenc), 7);
	gtk_window_set_policy(GTK_WINDOW(winenc), TRUE, TRUE, TRUE);
	gtk_signal_connect(GTK_OBJECT(winenc), "destroy",
			(GtkSignalFunc) gtk_widget_destroyed, &winenc);*/
	
	tmpbox = gtk_hbox_new(FALSE, 6);
	gtk_box_pack_start(GTK_BOX(verbox), tmpbox, FALSE, FALSE, 0);

	tmplabel = gtk_label_new(_("Quality: (low)"));
	gtk_box_pack_start(GTK_BOX(tmpbox), tmplabel, FALSE, FALSE, 0);
	
	enc->adj1 = gtk_adjustment_new(0.0, 1.0, 9.0, 1.0, 1.0, 0.0);
	g_signal_connect(G_OBJECT(enc->adj1), "value_changed",
			G_CALLBACK(gcb_enc_set_quality), enc);
	
	scaled = gtk_hscale_new(GTK_ADJUSTMENT(enc->adj1));
	gtk_range_set_update_policy(GTK_RANGE(scaled), GTK_UPDATE_CONTINUOUS);
	gtk_scale_set_draw_value(GTK_SCALE(scaled), FALSE);
	gtk_widget_set_size_request(scaled, 100, 30);
	gtk_box_pack_start(GTK_BOX(tmpbox), scaled, FALSE, FALSE, 0);
	
	tmplabel = gtk_label_new(_("(high)"));
	gtk_box_pack_start(GTK_BOX(tmpbox), tmplabel, FALSE, FALSE, 0);
	
	tmplabel = gtk_label_new(_("    Mode"));
	gtk_box_pack_start(GTK_BOX(tmpbox), tmplabel, FALSE, FALSE, 0);

	enc->mode = gtk_combo_new();
	listola = g_list_append(listola, (void *) _("mono"));
	listola = g_list_append(listola, (void *) _("stereo"));
	gtk_combo_set_popdown_strings(GTK_COMBO(enc->mode), listola);
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(enc->mode)->entry), FALSE);
	g_list_free(listola);
	listola = NULL;
	
	gtk_widget_set_size_request(enc->mode, 70, 22);
	gtk_box_pack_start(GTK_BOX(tmpbox), enc->mode, FALSE, FALSE, 0);
	
	enc->adj_lab = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(verbox), enc->adj_lab, FALSE, FALSE, 0);
	

	gtk_adjustment_set_value(GTK_ADJUSTMENT(enc->adj1), 2.0);
	
	/* hidden info */
	tabbola = gtk_table_new(3, 4, FALSE);
	gtk_table_set_row_spacings(GTK_TABLE(tabbola), 12);
	gtk_table_set_col_spacings(GTK_TABLE(tabbola), 12);
	gtk_box_pack_start(GTK_BOX(verbox), tabbola, FALSE, FALSE, 0);
	enc->tabbola = tabbola;
	
	tmplabel = gtk_label_new(_("bitrate"));
	gtk_table_attach_defaults(GTK_TABLE(tabbola), tmplabel, 0, 1, 0, 1);
	enc->bitrate = gtk_combo_new();
	
	listola = g_list_append(listola, (void *) "16"); 
	listola = g_list_append(listola, (void *) "24"); 
	listola = g_list_append(listola, (void *) "32"); 
	listola = g_list_append(listola, (void *) "48"); 
	listola = g_list_append(listola, (void *) "56"); 
	listola = g_list_append(listola, (void *) "64"); 
	listola = g_list_append(listola, (void *) "96"); 
	listola = g_list_append(listola, (void *) "128");
	gtk_combo_set_popdown_strings(GTK_COMBO(enc->bitrate), listola);
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(enc->bitrate)->entry), FALSE);
	g_list_free(listola);
	listola = NULL;

	gtk_widget_set_size_request(enc->bitrate, 70, 22); 
	gtk_table_attach_defaults(GTK_TABLE(tabbola), enc->bitrate, 1, 2, 0, 1);

	tmplabel = gtk_label_new(_("frequency filtering"));
	gtk_table_attach_defaults(GTK_TABLE(tabbola), tmplabel, 2, 3, 0, 1);
	enc->freqfil = gtk_combo_new();
	
	listola = g_list_append(listola, (void *) _("auto"));
	listola = g_list_append(listola, (void *) _("none"));
	listola = g_list_append(listola, (void *) _("manual"));
	gtk_combo_set_popdown_strings(GTK_COMBO(enc->freqfil), listola);
	g_list_free(listola);
	listola = NULL;
	g_signal_connect(G_OBJECT(GTK_COMBO(enc->freqfil)->entry), 
			"changed", G_CALLBACK(gcb_set_pass), enc);
	gtk_widget_set_size_request(enc->freqfil, 70, 22);
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(enc->freqfil)->entry), 
			FALSE);
	gtk_table_attach_defaults(GTK_TABLE(tabbola), enc->freqfil, 3, 4, 0, 1);

	
	tmplabel=gtk_label_new(_("frequency"));
	gtk_table_attach_defaults(GTK_TABLE(tabbola), tmplabel, 0, 1, 1, 2);
	enc->freq = gtk_combo_new();
	listola = g_list_append(listola, (void *) "auto");
	listola = g_list_append(listola, (void *) "11000");
	listola = g_list_append(listola, (void *) "16000");
	listola = g_list_append(listola, (void *) "22050");
	listola = g_list_append(listola, (void *) "44100");
	gtk_combo_set_popdown_strings(GTK_COMBO(enc->freq), listola);
	g_list_free(listola);
	listola = NULL;
	gtk_widget_set_size_request(enc->freq, 70, 22);
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(enc->freq)->entry), FALSE);
	gtk_table_attach_defaults(GTK_TABLE(tabbola), enc->freq, 1, 2, 1, 2);

	tmplabel = gtk_label_new(_("lowpass Hz"));
	gtk_table_attach_defaults(GTK_TABLE(tabbola), tmplabel, 2, 3, 1, 2);
	enc->lowps = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(enc->lowps), FALSE);
	gtk_entry_set_text(GTK_ENTRY(enc->lowps), _("guessed"));
	gtk_widget_set_size_request(enc->lowps, 70, 22);
	gtk_table_attach_defaults(GTK_TABLE(tabbola), enc->lowps, 3, 4, 1, 2);

	tmplabel = gtk_label_new(_("highpass Hz"));
	gtk_table_attach_defaults(GTK_TABLE(tabbola), tmplabel, 2, 3, 2, 3);
	enc->highps = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(enc->highps), FALSE);
	gtk_entry_set_text(GTK_ENTRY(enc->highps), _("guessed"));
	gtk_widget_set_size_request(enc->highps, 70, 22);
	gtk_table_attach_defaults(GTK_TABLE(tabbola), enc->highps, 3, 4, 2, 3);
	
	/* end of hidden info */

	enc->tasti = gtk_vbox_new(FALSE, 5);
	
	enc->expert = gtk_toggle_button_new_with_label(_("I'm Expert..."));
	g_signal_connect(G_OBJECT(enc->expert), "toggled",
			G_CALLBACK(expert_mode), tabbola);
	gtk_box_pack_start(GTK_BOX(enc->tasti), enc->expert, FALSE, FALSE, 0);
	
	/* profile menu */
	tmpbar = gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(enc->tasti), tmpbar, FALSE, FALSE, 0);

	enc->profroot = gtk_menu_item_new_with_label(_("Profile..."));
	gtk_menu_bar_append(GTK_MENU_BAR(tmpbar), enc->profroot);
	
	if(enc->outchan->tipo == MP3)
		profile_lame_load();
	else
		profile_vorbis_load();

	enc_profmenu(enc);

	
	/* profile menu end */
	
	ok = gtk_button_new_with_label(_("Apply"));
	gtk_button_set_relief(GTK_BUTTON(ok), GTK_RELIEF_HALF);
	g_signal_connect(G_OBJECT(ok), "clicked", 
			G_CALLBACK(gcb_set_enc), enc);
	gtk_box_pack_start(GTK_BOX(enc->tasti), ok, FALSE, FALSE, 0);

	/* set value from core */	
	enc_put(enc);

	gtk_widget_show_all(verbox);
	gtk_widget_hide_on_delete(tabbola);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(enc->expert), FALSE);
	
	return enc;
}

void enc_profmenu(struct encdata *enc)
{
	char *aprof;
	GtkWidget *tmplabel, *tmplabel1, *tmpmenu;
	GList *listrunner;
	struct encprof *tmp;

	tmpmenu = gtk_menu_new();

	if(enc->outchan->tipo == MP3) {
		tmplabel = gtk_menu_item_new_with_label(_("Save..."));
		g_object_set_data(G_OBJECT(tmplabel), "type", (void *) "lame");
		
		tmplabel1 = gtk_menu_item_new_with_label(_("Remove..."));
		g_object_set_data(G_OBJECT(tmplabel1), "type", (void *) "lame");
		
		listrunner = g_list_first(lameprof);
	} else {
		tmplabel = gtk_menu_item_new_with_label(_("Save..."));
		g_object_set_data(G_OBJECT(tmplabel), "type", (void *) "ogg");
		
		tmplabel1 = gtk_menu_item_new_with_label(_("Remove..."));
		g_object_set_data(G_OBJECT(tmplabel1), "type", (void *) "ogg");

		listrunner = g_list_first(vorbisprof);
	}

	g_signal_connect(G_OBJECT(tmplabel), "activate",
			G_CALLBACK(win_profile_save), (void *) enc);
	g_signal_connect(G_OBJECT(tmplabel1), "activate",
			G_CALLBACK(win_profile_remove), (void *) enc);
	gtk_menu_append(GTK_MENU(tmpmenu), tmplabel);
	gtk_menu_append(GTK_MENU(tmpmenu), tmplabel1);
	
	tmplabel = gtk_menu_item_new_with_label(_("Load"));
	gtk_menu_append(GTK_MENU(tmpmenu), tmplabel);
	while(listrunner) {
		tmp = (struct encprof *) listrunner->data;
		aprof = tmp->name;
		tmplabel = gtk_menu_item_new_with_label(aprof);
		g_signal_connect(G_OBJECT(tmplabel), "activate",
			       G_CALLBACK(gcb_enc_put), enc);
		gtk_menu_append(GTK_MENU(tmpmenu), tmplabel);
		//FIXME : missing remove in profile API
		listrunner = g_list_next(listrunner);
	}

	gtk_menu_item_remove_submenu(GTK_MENU_ITEM(enc->profroot));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(enc->profroot), tmpmenu);
	gtk_widget_show_all(tmpmenu);

}

void enc_put(struct encdata *enc)
{
	/* we have got int int struct lametmp, now we need string, 
	 * sprintf will be my friend */
	
	char tmp[20];

	if(!enc) 
		return;

	/* fill GUI */
	if(enc->outchan->bps() == 0)
	  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(enc->bitrate)->entry), "auto");
	else {
	  sprintf(tmp, "%d", enc->outchan->bps());
	  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(enc->bitrate)->entry), tmp);
	}

	if(enc->outchan->freq() == 0)
	  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(enc->freq)->entry), "auto");
	else {
	  sprintf(tmp, "%d", enc->outchan->freq());
	  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(enc->freq)->entry), tmp);
	}

	switch(enc->outchan->channels()) {
	case 1:
	  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(enc->mode)->entry), _("mono"));
	  break;
	case 2:
	  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(enc->mode)->entry), _("stereo"));
	  break;
	}

	switch(enc->outchan->lowpass()) {
	case -1:
	  if(enc->outchan->highpass() == -1)
	    gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(enc->freqfil)->entry),
			    _("none"));
	  break;
	case 0:
	  if(!enc->outchan->highpass())
	    gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(enc->freqfil)->entry),
			    _("auto"));
	  break;
	default:
	  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(enc->freqfil)->entry),
			  _("manual"));
	}
	sprintf(tmp, "%d", enc->outchan->lowpass());
	gtk_entry_set_text(GTK_ENTRY(enc->lowps), tmp);
	sprintf(tmp, "%d", enc->outchan->highpass());
	gtk_entry_set_text(GTK_ENTRY(enc->highps), tmp);
	
	gtk_adjustment_set_value(GTK_ADJUSTMENT(enc->adj1),
				 enc->outchan->_quality);
}

void enc_get(struct encdata *enc)
{
	char *tmp;
	double dtmp;	

	if(!enc)
		return;

	func("enc_get(%p)", enc);
	// set bps (obsolete)
/*
	tmp = gtk_editable_get_chars(GTK_EDITABLE(GTK_COMBO(enc->bitrate)->entry), 0, -1);	
	enc->outchan->bps( atoi(tmp) );
	g_free(tmp);
*/

	tmp = gtk_editable_get_chars(GTK_EDITABLE(GTK_COMBO(enc->mode)->entry), 0, -1);
	/* FIXME: i18n problem */
	if(tmp[0] == 'm') 
		enc->outchan->channels(1);
	else 
		enc->outchan->channels(2);
	g_free(tmp);

	tmp = gtk_editable_get_chars(GTK_EDITABLE(GTK_COMBO(enc->freq)->entry), 0, -1);
	if(tmp[0] == 'a') /* auto */
	  enc->outchan->freq(0);
	else
	  enc->outchan->freq(atoi(tmp));
	g_free(tmp);
	
	/* XXX: quality omitted */
	dtmp = gtk_adjustment_get_value(GTK_ADJUSTMENT(enc->adj1));
	enc->outchan->quality( dtmp );


	tmp = gtk_editable_get_chars(GTK_EDITABLE(enc->lowps), 0, -1);
	enc->outchan->lowpass(atoi(tmp));
	g_free(tmp);
	
	tmp = gtk_editable_get_chars(GTK_EDITABLE(enc->highps), 0, -1);
	enc->outchan->highpass(atoi(tmp));
	g_free(tmp);

	func(_("enc_get finished"));

}

void gcb_set_pass(GtkWidget *w, struct encdata *enc)
{
	char *tmp;
	
	tmp = gtk_editable_get_chars(GTK_EDITABLE(w), 0, -1);
	
	if(!(strcmp(tmp, _("auto")))) {
		gtk_entry_set_editable(GTK_ENTRY(enc->lowps), FALSE);
		gtk_entry_set_editable(GTK_ENTRY(enc->highps), FALSE);
		gtk_entry_set_text(GTK_ENTRY(enc->lowps), _("guessed"));
		gtk_entry_set_text(GTK_ENTRY(enc->highps), _("guessed"));
		return;
	}
	if(!(strcmp(tmp, _("none")))) {
		gtk_entry_set_editable(GTK_ENTRY(enc->lowps), FALSE);
		gtk_entry_set_editable(GTK_ENTRY(enc->highps), FALSE);
		gtk_entry_set_text(GTK_ENTRY(enc->lowps), _("none"));
		gtk_entry_set_text(GTK_ENTRY(enc->highps), _("none"));
		return;
	}	
	gtk_entry_set_editable(GTK_ENTRY(enc->lowps), TRUE);
	gtk_entry_set_editable(GTK_ENTRY(enc->highps), TRUE);

	g_free(tmp);
}

void gcb_set_enc(GtkWidget *w, struct encdata *enc) {

  /* fill muse-core */
  enc_get(enc);
  
  /* debug */
  func("gcb_set_enc: outchan->quality(%.1f)", enc->outchan->_quality);
  func("gcb_set_enc: outchan->bps(%d)", enc->outchan->bps());
  func("gcb_set_enc: outchan->channels(%i)", enc->outchan->channels());
  func("gcb_set_enc: outchan->freq(%d)", enc->outchan->freq());
  func("gcb_set_enc: outchan->highpass(%d)", enc->outchan->highpass());
  func("gcb_set_enc: outchan->lowpass(%d)", enc->outchan->lowpass());
  
  mixer->apply_enc( enc->outchan->id );
}

void gcb_enc_put(GtkWidget *w, struct encdata *enc)
{
	GtkWidget *child;
	gchar *text;

	child = GTK_BIN(w)->child;
	gtk_label_get(GTK_LABEL(child), &text);

	prof2enc(text, enc);

}

void gcb_enc_save(struct encdata *enc)
{
	char *profile;
	struct encprof *tmp;
	
	profile = gtk_editable_get_chars(GTK_EDITABLE(profentry), 0, -1); 
	if(profile[0] == '\0') {
		win_error(_("You should insert a name for this profile"));
		g_free(profile);
		return;
	}
	tmp = enc2prof(profile, enc);
	g_free(profile);

	if(enc->outchan->tipo == MP3) {
		lameprof = g_list_append(lameprof, (void *) tmp);
		profile_lame_write();
	} else {
		vorbisprof = g_list_append(vorbisprof, (void *) tmp);
		profile_vorbis_write();
	}
	
	/* add profile in menu */
	enc_profmenu(enc);
}

void gcb_enc_set_quality(GtkWidget *w, struct encdata *enc)
{
	float val = 0;
	gchar text[256];
	
	val = GTK_ADJUSTMENT(enc->adj1)->value;

	enc->outchan->quality(val);
	snprintf(text,256,"%.1f ( %s )",val,enc->outchan->quality_desc);
	
	gtk_label_set_text(GTK_LABEL(enc->adj_lab), text);
}

void gcb_enc_set_mode(GtkMenuShell *w, struct encdata *enc)
{
	gchar *text = NULL;
	
	if((text = gtk_editable_get_chars(GTK_EDITABLE(
				GTK_COMBO(enc->mode)->entry), 0, -1))) {
		if(text[0] == 'm')
			enc->outchan->channels(1); 
		else
			enc->outchan->channels(2); 

		g_free(text);
	}
	
}

void expert_mode(GtkWidget *w, GtkWidget *table)
{
  if(GTK_TOGGLE_BUTTON(w)->active) {
    gtk_widget_show_all(table);
    //    i_am_expert = true;
 } else {
    gtk_widget_hide_on_delete(table); //bomboclat bug, c1cc10 fix
    //    i_am_expert = false;
  }
}

void prof2enc(gchar *name, struct encdata *enc)
{
	GList *listrunner;
	struct encprof *tmp;

	if(enc->outchan->tipo == MP3)
		listrunner = g_list_first(lameprof);
	else
		listrunner = g_list_first(vorbisprof);

	while(listrunner) {
		tmp = (struct encprof *) listrunner->data;
		if(!strcmp(tmp->name, name)) {
			gtk_entry_set_text(GTK_ENTRY(
				GTK_COMBO(enc->bitrate)->entry), tmp->bitrate);
			gtk_entry_set_text(GTK_ENTRY(
				GTK_COMBO(enc->mode)->entry), tmp->mode);
			gtk_adjustment_set_value(GTK_ADJUSTMENT(enc->adj1),
					tmp->quality);
			gtk_entry_set_text(GTK_ENTRY(
				GTK_COMBO(enc->freq)->entry), tmp->frequency);
			gtk_entry_set_text(GTK_ENTRY(
				GTK_COMBO(enc->freqfil)->entry), tmp->freqfil);
			gtk_entry_set_text(GTK_ENTRY(enc->lowps), 
					tmp->lowpass);
			gtk_entry_set_text(GTK_ENTRY(enc->highps), 
					tmp->highpass);
			break;
		}
		listrunner = g_list_next(listrunner);
	}


}
		
struct encprof *enc2prof(gchar *name, struct encdata *enc)
{
	struct encprof *tmp;

	tmp = (struct encprof *) g_malloc(sizeof(struct encprof));

	tmp->name = g_strdup(name);
	tmp->mode = gtk_editable_get_chars(GTK_EDITABLE(
				GTK_COMBO(enc->mode)->entry), 0, -1);
	tmp->quality = GTK_ADJUSTMENT(enc->adj1)->value;
	tmp->bitrate = gtk_editable_get_chars(GTK_EDITABLE(
				GTK_COMBO(enc->bitrate)->entry), 0, -1);
	tmp->frequency = gtk_editable_get_chars(GTK_EDITABLE(
				GTK_COMBO(enc->freq)->entry), 0, -1);
	tmp->freqfil = gtk_editable_get_chars(GTK_EDITABLE(
				GTK_COMBO(enc->freqfil)->entry), 0, -1);
	tmp->lowpass = gtk_editable_get_chars(GTK_EDITABLE(enc->lowps), 
			0, -1);	
	tmp->highpass = gtk_editable_get_chars(GTK_EDITABLE(enc->highps),
			0, -1);

	return tmp;


}
	
