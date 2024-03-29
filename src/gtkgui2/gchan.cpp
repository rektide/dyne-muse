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

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <jutils.h>
#include <gen.h>
#include <utils.h>
#include <docking.h>
#include <gchan_events.h>
#include <gchan.h>
#include <listpack.h>

#include <jmixer.h>

#include <xpm2/play.xpm>
#include <xpm2/stop.xpm>
#include <xpm2/pause.xpm>
#include <xpm2/rewind.xpm>
#include <xpm2/forward.xpm>
#include <xpm2/dock.xpm>

#include <config.h>

static GtkWidget *winh;

bool createch(void)
{
	struct gchan *object;
	GtkWidget *fixed;
	GtkWidget *frami1;
	GtkWidget *vbox, *htast, *hbot;
	GtkObject *adj, *adj1;
	GtkWidget *playmenu, *playmenuopt;
	GtkWidget *tmpwid, *tmpwid1, *dock;
	GtkWidget *progress; 
	GtkWidget *volume; 
	GtkWidget *scrollwin;
	GtkWidget *table, *rmit/*, *file, *http*/;
	GtkWidget *piddi;
	GtkTreeSelection *select;
	GtkListStore *store;
	GtkWidget *tree;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTargetEntry target_entry[3];

	gchar numchan[16];
	unsigned int pos=0;
	unsigned int idx=1;	
	dock=rmit=NULL;


	while(( object = (struct gchan *) list_get_data(listachan, idx, 0) ))
		idx++;
	
	if(idx > MAX_CHANNELS)
		return FALSE;
	
	func(_("GTK_GUI::createch(%u)"), idx);
	object = (struct gchan *) g_malloc (sizeof(struct gchan));
	
	object->idx = idx;	
	object->channel = 0; /* nothing */
	
	func(_("GTK_GUI::createch : chan[%u] is at %p"), idx, object);

	mixer->create_channel(idx-1);

	snprintf(numchan, 16, _("Channel[%u]"), object->idx);
	frami1 = gtk_frame_new(numchan);
	object->frami = frami1;
	gtk_frame_set_shadow_type(GTK_FRAME(frami1), GTK_SHADOW_ETCHED_OUT);
	
	vbox = gtk_vbox_new(FALSE, 0); 
	gtk_container_add(GTK_CONTAINER(frami1), vbox);
	
	/* at the moment pos is 0 function */
	list_add(&listachan, (void *) object, pos, idx, object->frami);
	

	htast = gtk_hbox_new(TRUE, 0); /* same dimension */
	fixed = gtk_fixed_new();
	gtk_fixed_put(GTK_FIXED(fixed), htast, 0, 0);
	gtk_box_pack_start(GTK_BOX(vbox), fixed, FALSE, FALSE, 0);

	/* addiamo il coso del tempo */
	tmpwid = gtk_entry_new_with_max_length(9);
	object->ptime = tmpwid;
	gtk_widget_set_size_request(tmpwid, 55, 22);
	gtk_entry_set_text(GTK_ENTRY(tmpwid), "00:00:00");
	gtk_entry_set_editable(GTK_ENTRY(tmpwid), FALSE);
	gtk_box_pack_start(GTK_BOX(htast), tmpwid, FALSE, FALSE, 0);
	
	/* enjoy ourselves with buttons */
	
	tmpwid = createpixmap(window, tmpwid, play_xpm, 
			_("Play Channel"), TRUE);
	object->play = tmpwid;
	g_signal_connect(G_OBJECT(tmpwid), "pressed",
			G_CALLBACK(gcb_play_channel), object);
	gtk_box_pack_start(GTK_BOX(htast), tmpwid, FALSE, TRUE, 0);
	
	tmpwid = createpixmap(window, tmpwid, stop_xpm, 
			_("Stop Channel"), FALSE);
	g_signal_connect(G_OBJECT(tmpwid), "pressed",
			G_CALLBACK(gcb_stop_channel), object);
	gtk_box_pack_start(GTK_BOX(htast), tmpwid, FALSE, TRUE, 0);
	
	tmpwid = createpixmap(window, tmpwid, pause_xpm, 
			_("Pause Channel"), TRUE);
	object->pause = tmpwid;
	g_signal_connect(G_OBJECT(tmpwid), "clicked",
			G_CALLBACK(gcb_pause_channel), object);
	gtk_box_pack_start(GTK_BOX(htast), tmpwid, FALSE, TRUE, 0);

	tmpwid = createpixmap(window, tmpwid, rewind_xpm, 
			_("Rewind Channel"), FALSE);
	g_signal_connect(G_OBJECT(tmpwid), "clicked",
			G_CALLBACK(gcb_begin_channel), object);
	gtk_box_pack_start(GTK_BOX(htast), tmpwid, FALSE, TRUE, 0);
	
	tmpwid = createpixmap(window, tmpwid, forward_xpm, 
			_("Forward Channel"), FALSE);
	g_signal_connect(G_OBJECT(tmpwid), "clicked",
			G_CALLBACK(gcb_end_channel), object);
	gtk_box_pack_start(GTK_BOX(htast), tmpwid, FALSE, TRUE, 0);
	
	/* second part */
	adj = gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	object->adjprog = adj;
	progress = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	gtk_range_set_update_policy(GTK_RANGE(progress),GTK_UPDATE_DELAYED);
	gtk_scale_set_digits(GTK_SCALE(progress), 6);
	object->progress = progress;

	gtk_scale_set_draw_value(GTK_SCALE(progress), FALSE);
	/*
	g_signal_connect(G_OBJECT(progress), "button_press_event",
			   G_CALLBACK(gcb_event_pause_channel), object);
	g_signal_connect(G_OBJECT(progress), "button_release_event",
			G_CALLBACK(gcb_event_set_position), object);
	*/

	/* here there is a problem: the position sticks to the mouse
	   maybe there is a way to say to unfocus the widget after moving it? */
	g_signal_connect(G_OBJECT(progress), "button_release_event",
			G_CALLBACK(gcb_event_set_position), object);
	gtk_box_pack_start(GTK_BOX(vbox), progress, FALSE, FALSE, 0);

	hbot = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(vbox), hbot);
	
	/* volume adjustment  */
	adj1 = gtk_adjustment_new(0.0, 0.0, 1.0, 0.01, 0.01, 0.0);
	object->adjvol = adj1;
	object->volid = g_signal_connect(G_OBJECT(adj1), "value_changed",
			G_CALLBACK(gcb_set_volume), object);

#ifdef WITH_SPEED
	/* speed adjustment */
	object->adjspeed = gtk_adjustment_new(1.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_signal_connect(G_OBJECT(object->adjspeed), "value_changed",
			G_CALLBACK(gcb_set_speed), object);
#endif
	
	volume = gtk_vscale_new(GTK_ADJUSTMENT(adj1));
	gtk_scale_set_draw_value(GTK_SCALE(volume), FALSE);

	tmpwid = gtk_table_new(2, 1, FALSE);
	gtk_box_pack_start(GTK_BOX(hbot), tmpwid, FALSE, FALSE, 0);
	tmpwid1 = gtk_vbox_new(FALSE, 0);
	object->vol_lab = gtk_label_new(_("VOL"));
	gtk_box_pack_start(GTK_BOX(tmpwid1), object->vol_lab, FALSE, FALSE, 0);
	
	gtk_box_pack_start(GTK_BOX(tmpwid1), volume, TRUE, TRUE, 0);
	object->vol_lab=gtk_label_new(_("100"));
	gtk_box_pack_start(GTK_BOX(tmpwid1), object->vol_lab, FALSE, FALSE, 0);
	gtk_table_attach_defaults(GTK_TABLE(tmpwid), tmpwid1, 0, 1, 1, 2);

#ifdef WITH_SPEED
	/* speed widget */
	//	tmpwid1=gtk_vbox_new(FALSE, 0);
	object->speed=gtk_tx_dial_new(GTK_ADJUSTMENT(object->adjspeed));
	set_tip(object->speed, _("Speed"));
	gtk_box_pack_start(GTK_BOX(tmpwid1), object->speed, FALSE, FALSE, 0);
	object->speed_lab=gtk_label_new(_("100"));
	gtk_box_pack_start(GTK_BOX(tmpwid1), object->speed_lab, FALSE, FALSE, 0);
	gtk_table_attach_defaults(GTK_TABLE(tmpwid), tmpwid1, 0, 1, 0, 1);
#endif

	scrollwin = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(scrollwin, 90, 140);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollwin), 
					GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

	
	store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);
	tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	g_object_unref(store);
	object->tree = tree;
	//gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(tree), TRUE);
	
	/* DND stuff */
	target_entry[0].target = DRAG_TAR_NAME_0;
	target_entry[0].flags = 0;
	target_entry[0].info = DRAG_TAR_INFO_0;

	target_entry[1].target = DRAG_TAR_NAME_1;
	target_entry[1].flags = 0;
	target_entry[1].info = DRAG_TAR_INFO_1;

	target_entry[2].target = DRAG_TAR_NAME_2;
	target_entry[2].flags = 0;
	target_entry[2].info = DRAG_TAR_INFO_2;

	gtk_drag_dest_set(
		tree, 
		(GtkDestDefaults)(GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_HIGHLIGHT |
		GTK_DEST_DEFAULT_DROP),
		target_entry,sizeof(target_entry) / sizeof(GtkTargetEntry),
		GDK_ACTION_MOVE);
	g_signal_connect(G_OBJECT(tree), "drag_motion",
			G_CALLBACK(DND_data_motion), object);
	gtk_drag_source_set(
		tree,
		(GdkModifierType) (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK),
		target_entry, sizeof(target_entry) / sizeof(GtkTargetEntry),
		GDK_ACTION_MOVE);
	
	g_signal_connect(G_OBJECT(tree), "drag_begin",
			G_CALLBACK(DND_begin), object);
	g_signal_connect(G_OBJECT(tree), "drag_end",
			G_CALLBACK(DND_end), object);
	g_signal_connect(G_OBJECT(tree), "drag_data_get",
			G_CALLBACK(DND_data_get), object);
	g_signal_connect(G_OBJECT(tree), "drag_data_received",
			G_CALLBACK(DND_data_received), object);
	g_signal_connect(G_OBJECT(tree), "drag_data_delete",
			G_CALLBACK(DND_data_delete), object);
	
	/* end*/

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_MULTIPLE);
	/*g_signal_connect(G_OBJECT(select), "changed",
			 G_CALLBACK(gcb_set_channel), object);
*/
	g_signal_connect(G_OBJECT(tree), "button_press_event",
			G_CALLBACK(gcb_event_view_popup), object);
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Nick", 
			renderer, "text", TITLE, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE);			
	gtk_container_add(GTK_CONTAINER(scrollwin), tree);
	gtk_box_pack_start(GTK_BOX(hbot), scrollwin, TRUE, TRUE, 0);
	

	/* playmode menu */
	playmenu = gtk_menu_new();
	tmpwid = gtk_menu_item_new_with_label(_("single play"));
	gtk_menu_append(GTK_MENU(playmenu), tmpwid);
	
	tmpwid=gtk_menu_item_new_with_label(_("loop"));
	gtk_menu_append(GTK_MENU(playmenu), tmpwid);
	
	tmpwid=gtk_menu_item_new_with_label(_("continuous"));
	gtk_menu_append(GTK_MENU(playmenu), tmpwid);

	playmenuopt = gtk_option_menu_new();
	gtk_option_menu_set_menu(GTK_OPTION_MENU(playmenuopt), playmenu);
	g_signal_connect(G_OBJECT(playmenuopt), "changed",
			G_CALLBACK(gcb_set_playmode), object);
	
	rmit = gtk_button_new();
	tmpwid = gtk_image_new_from_stock(GTK_STOCK_CANCEL, 
			GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_container_add(GTK_CONTAINER(rmit), tmpwid);
	object->rmit = rmit;

	g_signal_connect(G_OBJECT(object->rmit),"clicked",
			G_CALLBACK(gcb_deletech), object);

	dock = createpixmap(window, dock, dock_xpm, 
			_("Undock Channel"), FALSE);
	object->dock = dock;
	g_signal_connect(G_OBJECT(object->dock), "clicked",
			G_CALLBACK(dockchan), object);

	piddi = gtk_hbox_new(FALSE, 0);
	object->hbox = piddi;
	gtk_box_pack_start(GTK_BOX(piddi), rmit, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(piddi), dock, FALSE, FALSE, 0);

	table = gtk_table_new(1, 4, TRUE);
	object->table = table;
	gtk_table_attach_defaults(GTK_TABLE(table), piddi, 0, 1, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(table), playmenuopt, 3, 5, 0, 1);

	gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, 0);

	object->playmode = 0;

	/* pack_chan_insert and pos update */
	pos = pack_chan_insert(object->frami);


	return TRUE;
}


void spawnfilew(GtkWidget *button, struct gchan *o)
{
	GtkWidget *filew;

/*#if GTK_CHECK_VERSION(2,4,0)
	notice("use file chooser");
#else*/
	func(_("GTK_GUI::spawnfilew : filebrowser for chan[%u]"), o->idx);

	filew = gtk_file_selection_new(_("Select a .mp3 file or a playlist (.pl)"));
	if(pathfile)
		gtk_file_selection_set_filename(GTK_FILE_SELECTION(filew), pathfile);
	g_object_set_data(G_OBJECT(GTK_FILE_SELECTION(filew)->ok_button),
			"chan", (void *) &o->idx);
	g_signal_connect(G_OBJECT(GTK_FILE_SELECTION(filew)->ok_button),
			"clicked", G_CALLBACK(gcb_add_file), filew);
	
	gtk_file_selection_set_select_multiple(GTK_FILE_SELECTION(filew), TRUE);
	
	g_signal_connect(G_OBJECT(GTK_WIDGET(filew)), "destroy",
			G_CALLBACK(gtk_widget_destroy), NULL);
	g_signal_connect_swapped(G_OBJECT(
				GTK_FILE_SELECTION(filew)->cancel_button),
			"clicked", G_CALLBACK(gtk_widget_destroy), 
			G_OBJECT(filew));
	
	gtk_widget_show(filew);
/*#endif*/
}

void httpwin(GtkWidget *w, struct gchan *o)
{
	GtkWidget *dialog;
	GtkWidget *tmphbox, *tmpvbox;
	GtkWidget *tmpwid;
	gint result;

	dialog = gtk_dialog_new_with_buttons(_("Listen to a network stream"),
			NULL,
			GTK_DIALOG_MODAL,
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			NULL);
	
	tmphbox = gtk_hbox_new(FALSE, 6);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), 
			tmphbox, FALSE, FALSE, 6);
	
	tmpwid = gtk_image_new_from_stock(GTK_STOCK_DIALOG_QUESTION,
			GTK_ICON_SIZE_DIALOG);
	gtk_box_pack_start(GTK_BOX(tmphbox), tmpwid, FALSE, FALSE, 6);
	
	tmpvbox = gtk_vbox_new(FALSE, 6);
	gtk_box_pack_start(GTK_BOX(tmphbox), tmpvbox, FALSE, FALSE, 6);
	
	tmpwid = gtk_label_new(_("Insert http source:"));
	gtk_box_pack_start(GTK_BOX(tmpvbox), tmpwid, FALSE, FALSE, 0);
	
	tmpwid = gtk_entry_new();
	o->httpentry = tmpwid;
	g_signal_connect(G_OBJECT(tmpwid), "activate",
					G_CALLBACK(httpinsert), o);
	gtk_box_pack_start(GTK_BOX(tmpvbox), tmpwid, FALSE, FALSE, 6);
	
	gtk_widget_show_all(dialog);

	result = gtk_dialog_run(GTK_DIALOG(dialog));

	switch(result) {
		case GTK_RESPONSE_ACCEPT:
			httpinsert(w, o);
			break;
		default:
			break;
	}
	
	gtk_widget_destroy(dialog);
	
}

void httpinsert(GtkWidget *w, struct gchan *o)
{
	gchar *s = NULL;

	s = gtk_editable_get_chars(GTK_EDITABLE(o->httpentry), 0, -1);
	if(s[0] == 0) {
		g_free(s);
		win_warning(_("You have to insert an url!!"));
		winh = NULL;
		return;
	}
	act(_("httpinsert() url is %s"), s);
	if(!(mixer->add_to_playlist(o->idx-1, s))) {
		error(_("GTK_GUI:: httpinsert problem adding url"));
		win_error(_("Problem adding url"));
	}
	g_free(s);
	winh = NULL;
}

void gcb_deletech(GtkWidget *w, struct gchan *c)
{
	mixer->delete_channel(c->idx-1);
	switch(c->pos) {
		case 2: gtk_widget_hide(pack1.hscale);
				break;
		case 4: gtk_widget_hide(pack2.hscale);
				break;
		case 6: gtk_widget_hide(pack3.hscale);
	}

	list_remove(&listachan, c->idx);
	pack_refresh(listachan, NULL, true);
	
}
/* this is the only function which calls list_often_private function */
struct gchan *gcb_findch(unsigned int idx,unsigned int pos)
{
	struct listitem *tmp;
	
	if(pos == NOPOS)
		tmp = list_find_byidx(listachan, idx);
	else
		tmp = list_find_bypos(listachan, pos);

	if(!tmp)
		return NULL;
	else
		return (struct gchan *) tmp->data;
}


void gcb_set_playmode(GtkWidget *w, struct gchan *o) 
{
	gint mode;

	if((mode = gtk_option_menu_get_history(GTK_OPTION_MENU(w))) != -1) 
		mixer->set_playmode(o->idx-1, mode);
}


void gcb_set_channel(GtkWidget *w, struct gchan *o) {
	GtkTreeModel *model;
	GtkTreePath *path;
	GtkTreeIter iter;
	GtkTreeSelection *select = (GtkTreeSelection*)w;
	gint row=0;

	if((gtk_tree_selection_get_selected(select, &model, &iter))) {
	  path = gtk_tree_model_get_path(model, &iter);
	  row = gtk_tree_path_get_indices(path)[0];
	  func("GTK2: gcb_set_channel(%i,%i)",o->idx-1,row+1);
	  mixer->set_channel(o->idx-1,row+1);

	  gtk_tree_path_free(path);
	}
}

void gcb_play_channel(GtkWidget *w, struct gchan *o)
{
	guint res;
	
	GtkTreePath *path;
	GtkTreeIter iter;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	gint index=0;
	GList *pathlist=NULL;
	
	if( mixer->chan[o->idx-1]->on) {
	  gtk_toggle_button_set_active
	    (GTK_TOGGLE_BUTTON(o->play), FALSE);
	  return;
	}
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(o->tree));
	if( !(pathlist = gtk_tree_selection_get_selected_rows(select, &model)) )
		return;
	index = gtk_tree_path_get_indices((GtkTreePath *)pathlist->data)[0];
	
	func(_("selected index = %d"), index);
	gtk_tree_path_free((GtkTreePath *)pathlist->data);
	mixer->set_channel(o->idx-1, index+1);
	res = mixer->play_channel(o->idx-1);

	switch(res) {
	case 0:
	  o->channel=0;
	  break;
	case 1:  /* channel is seekable */
	  gtk_entry_set_text(GTK_ENTRY(o->ptime), "00:00:00");
	  gtk_adjustment_set_value(GTK_ADJUSTMENT(o->adjprog), 0.0);
	  o->channel=1;
	  break;
	case 2: /* unseekable */
	  gtk_entry_set_text(GTK_ENTRY(o->ptime), "-stream-");
	  gtk_adjustment_set_value(GTK_ADJUSTMENT(o->adjprog), 0.0);
	  o->channel=2;
	  break;
	}
	
	if(res==0) {
	  func(_("KO for playing :("));
	  gtk_toggle_button_set_mode
	    (GTK_TOGGLE_BUTTON(o->play), FALSE);
	} else {
	  func(_("ok for playing"));
	  gtk_toggle_button_set_mode
	    (GTK_TOGGLE_BUTTON(o->play), TRUE);
	}
		

}

void gcb_stop_channel(GtkWidget *w, struct gchan *o)
{
	mixer->stop_channel(o->idx-1);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(o->pause), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(o->play), FALSE);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(o->adjprog), 0.0);
	switch(o->channel) {
	case 1:
	  gtk_entry_set_text(GTK_ENTRY(o->ptime), "00:00:00");
	  break;
	case 2:
	  gtk_entry_set_text(GTK_ENTRY(o->ptime), "-stream-");
	  break;
	}
	
}

void gcb_pause_channel(GtkWidget *w, struct gchan *o)
{
	mixer->pause_channel(o->idx-1); 
}

void gcb_begin_channel(GtkWidget *w, struct gchan *o)
{
	mixer->set_position(o->idx-1, 0.0);
}

void gcb_end_channel(GtkWidget *w, struct gchan *o)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(o->pause), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(o->play), FALSE);
	mixer->set_position(o->idx-1, 1.0); 

}

void gcb_set_volume(GtkAdjustment *adj, struct gchan *o)
{
	char tmp[4];

	mixer->set_volume(o->idx-1, (1.0-adj->value));
	snprintf(tmp, sizeof(tmp), "%d", (int) ((1.0-adj->value)*100));
	gtk_label_set_text(GTK_LABEL(o->vol_lab), (gchar *) tmp);
}

#ifdef WITH_SPEED
void gcb_set_speed(GtkAdjustment *adj, struct gchan *o)
{
	char tmp[4];

	/* mixer->set_speed(o->idx-1, adj->value); */
	snprintf(tmp, sizeof(tmp), "%d", (int) (adj->value*100));
	gtk_label_set_text(GTK_LABEL(o->speed_lab), (gchar *) tmp);
}
#endif


void gcb_rem_from_playlist(GtkWidget *w, struct gchan *o)
{
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(o->tree));
	GtkTreeSelection *select;
	GtkTreeRowReference *reference;
	GtkTreePath *path;
	GList *pathlist, *reflist;
	gint row;
	

	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(o->tree));
	pathlist = gtk_tree_selection_get_selected_rows(select, NULL);

	pathlist = g_list_first(pathlist);
	reflist = NULL;
	
	while(pathlist) {
		reference = gtk_tree_row_reference_new(model, 
				(GtkTreePath *)pathlist->data);
		reflist = g_list_append(reflist, (void *) reference);
		
		pathlist = g_list_next(pathlist);
	}
	
	reflist = g_list_first(reflist);
	
	while(reflist) {
		path = gtk_tree_row_reference_get_path(
				(GtkTreeRowReference *) reflist->data);
		row = gtk_tree_path_get_indices(path)[0];
		mixer->rem_from_playlist(o->idx-1, row+1);
		if(gtk_tree_model_get_iter(model, &iter, path)) {
		  //			notice("removed %d", row);
			gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
		}
		
		gtk_tree_path_free(path);

		reflist = g_list_next(reflist);
	}

}

void gcb_add_file(GtkWidget *w, GtkFileSelection *fw)
{
	unsigned int idx = 0;
	struct gchan *o;
	gchar **cist; 
	//const gchar *cist;
	gint i;
	GtkTreeSelection *select;
	bool res=false;

	cist=NULL;
	
	idx= *(unsigned int *) g_object_get_data(G_OBJECT(GTK_FILE_SELECTION(fw)->ok_button), "chan");
	func(_("GTK_GUI::gcb_add_file : idx %u"), idx);
	cist = gtk_file_selection_get_selections(GTK_FILE_SELECTION(fw));
	//cist = gtk_file_selection_get_filename(GTK_FILE_SELECTION(fw));

	o = (struct gchan *) list_get_data(listachan, idx, 0);
	/*if(!cist[0]) {
		win_warning(_("You have to insert a filename!!"));
		return;
	}*/
	
	/* muse core'll add file into GtkTreeModel */
	for(i = 0; cist[i] != NULL; i++) {
		res = mixer->add_to_playlist(idx-1, cist[i]);
		//res = mixer->add_to_playlist(idx-1, cist);
		if(!res) { 
			func(_("gcb_add_file:: mixer->add_to_playlist(%u, %s) failed"),
					idx-1, cist[i]);
			win_error(_("Problem adding file :\\"));
		} else {
			//func(_("Added %s"), cist[i]);
			/* saves last directory visited */
			if(pathfile) 
				g_free(pathfile);
			pathfile = g_strdup(cist[i]);
			//pathfile = g_strdup(cist);
			int i = strlen(pathfile);
			while(pathfile[i] != '/') i--;
				pathfile[i+1]= '\0';
		}
	}
	g_strfreev(cist);
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(o->tree));
	gtk_tree_selection_unselect_all(select);
	gtk_widget_destroy((GtkWidget *)fw);

}


