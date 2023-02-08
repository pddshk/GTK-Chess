#include "gui.h"
#include <glib-unix.h>
#include <string.h>
//#include "engine.h"



enum _EngineState{
    ENGINE_OFF,
    ENGINE_IDLE,
    ENGINE_WORKING,
    ENGINE_ERROR
} engine_state;


gchar* get_sign(int number)
{
	number--;
	//printf("%d/n", number);
	gchar* st = (gchar*)malloc(sizeof(gchar)*(number*40 + 1));
	for(int i = 0; i< number * 40;i++) {
		st[i] = ' ';
	}
	st[number * 40] = '\0';
	return st;

}

void init_elements(char* textures)
{
	engine_state = ENGINE_IDLE; 
	init_state(&state);
	
	//
	tree = init_tree(&state);
	//
	init_textures();
	load_textures(textures);
	/////////////
	void* provider = gtk_css_provider_new();
	void* display = gdk_display_get_default();
	void* screen = gdk_display_get_default_screen (display);
	gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
	gtk_css_provider_load_from_path(GTK_CSS_PROVIDER(provider),"./src/selected.css",NULL);
	/////////////
	builder=gtk_builder_new_from_file("src/window.glade");
	GObject* window=gtk_builder_get_object(builder, "MainWindow");
	gtk_window_set_default_size(GTK_WINDOW(window), 1600, 900);
    GtkWidget *Board = GTK_WIDGET(gtk_builder_get_object(builder, "Board"));//220
	GdkPixbuf *empty_icon = gdk_pixbuf_new (GDK_COLORSPACE_RGB, 0, 8, 1, 1);

	//GtkBox *vbox=GTK_WIDGET(gtk_builder_get_object(builder, "Notation"));
	GtkTargetEntry *board_entry = gtk_target_entry_new(
		"GtkDrawingArea",
		GTK_TARGET_SAME_WIDGET,
		0
	);
	gtk_drag_dest_set (
		GTK_WIDGET(Board),
		GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_DROP,
		board_entry,
		1,
		GDK_ACTION_MOVE
	);
	gtk_drag_source_set (
		GTK_WIDGET(Board),
		GDK_BUTTON1_MASK,
		board_entry,
		1,
		GDK_ACTION_MOVE
	);
	g_signal_connect(Board, "drag-begin", G_CALLBACK(drag_begin), empty_icon);

	GtkWidget **dialogs = g_new(GtkWidget*, 4);
	// mate_dialog
	dialogs[0] = gtk_message_dialog_new(
		GTK_WINDOW(window),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_INFO,
		GTK_BUTTONS_OK,
		"Mate"
	);
	g_signal_connect(dialogs[0], "response", G_CALLBACK(gtk_widget_hide), NULL);
	// stalemate_dialog
	dialogs[1] = gtk_message_dialog_new(
		GTK_WINDOW(window),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_INFO,
		GTK_BUTTONS_OK,
		"Stalemate"
	);
	g_signal_connect(dialogs[1], "response", G_CALLBACK(gtk_widget_hide), NULL);
	// insufficient_material_dialog
	dialogs[2] = gtk_message_dialog_new(
		GTK_WINDOW(window),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_INFO,
		GTK_BUTTONS_OK,
		"Insufficient material"
	);
    g_signal_connect(dialogs[2], "response", G_CALLBACK(gtk_widget_hide), NULL);
	// threefold_repetition_dialog
	dialogs[3] = gtk_message_dialog_new(
		GTK_WINDOW(window),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_INFO,
		GTK_BUTTONS_OK,
		"Threefold repetition"
	);
	g_signal_connect(dialogs[3], "response", G_CALLBACK(gtk_widget_hide), NULL);

	g_signal_connect(Board, "drag-drop", G_CALLBACK(drag_drop), dialogs);
	gtk_builder_connect_signals(builder, NULL);
	//GObject *EngineToggler=gtk_builder_get_object(builder, "EngineToggler");

	g_source_set_callback (
		from_engine_manager_source,
		G_SOURCE_FUNC(parse_engine_response),
		builder,
		NULL
	);

	gtk_widget_show(GTK_WIDGET(window));
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}

void flip_board(GtkButton* button, gpointer Board)
{
	state.flipped = !state.flipped;
	gtk_widget_queue_draw(GTK_WIDGET(Board));
}

void new_game(GtkButton* button, gpointer Board)
{
	int flipped = state.flipped;
	init_state(&state);
	//
	destroy_tree(tree);
	tree = init_tree(&state);
	//
	state.flipped = flipped;
	gtk_widget_queue_draw(GTK_WIDGET(Board));
	show_state(tree->root,0);
}

void get_FEN (GtkWidget *widget, gint response_id, gpointer data)
{
    GtkEntry* entry = data;
    FEN_to_state(gtk_entry_get_text(entry));
	tree = init_tree(&state);
	show_state(tree->root, 0);
    gtk_widget_destroy (widget); // This will close the dialog
	gtk_widget_queue_draw(GTK_WIDGET(gtk_builder_get_object(builder, "Board")));
}

void paste_FEN(GtkButton* main_window_button, gpointer data) {
    GtkWidget *window = gtk_builder_get_object(builder, "MainWindow");
    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *button;
    static GtkEntry *textbox;

    dialog = gtk_dialog_new_with_buttons ("Get Text",
                                          window,
                                          GTK_DIALOG_MODAL,
                                          GTK_STOCK_OK,
                                          GTK_RESPONSE_OK,
                                          NULL);
    content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    grid = gtk_grid_new();
    gtk_container_add (GTK_CONTAINER (content_area), grid);

    label = gtk_label_new("Paste FEN: ");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    textbox = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), textbox, 1, 0, 1, 1);

    gtk_widget_show_all (dialog);
    g_signal_connect (GTK_DIALOG (dialog), "response", G_CALLBACK (get_FEN), textbox);
}

void select_state(GtkButton* button, gpointer node) {
	state = *(((tnode*)node)->field);
	(*tree).current = (tnode*)node;
	gtk_widget_queue_draw(GTK_WIDGET(gtk_builder_get_object(builder, "Board")));
	show_state(tree->root, 0);
}

gboolean parse_engine_response(GObject* stream, gpointer data)
{
	gssize nread;
	char buff[2048] = "";
	GError *error = NULL;
	nread = g_pollable_input_stream_read_nonblocking(
		G_POLLABLE_INPUT_STREAM(stream),
		buff,
		sizeof buff,
		NULL,
		&error
	);
	if (error) {
		puts(error->message);
		g_error_free(error);
		return TRUE;
	}
	if (nread == G_IO_ERROR_WOULD_BLOCK) return TRUE;
	puts(buff);
	return TRUE;
}

void toggle_engine(GtkButton* self, gpointer data)
{
	switch (engine_state) {
		case ENGINE_IDLE:
			//puts("Starting engine!");
			g_output_stream_write(to_engine_manager, "go\n", (sizeof "go\n") - 1, NULL, NULL);
			engine_state = ENGINE_WORKING;
			gtk_button_set_label(self, "Stop");
			break;
		case ENGINE_WORKING:
			//puts("Stopping engine!");
			g_output_stream_write(to_engine_manager, "stop\n", (sizeof "stop\n") - 1, NULL, NULL);
			engine_state = ENGINE_IDLE;
			gtk_button_set_label(self, "Go");
			break;
		case ENGINE_OFF:
			//puts("Engine is off!");
			break;
		case ENGINE_ERROR:
			puts("Engine is broken!");
			break;
	}
}

void show_state(tnode* node, int level) 
{
	if (node==NULL)
	return;
	GtkBox *vbox=GTK_BOX(gtk_builder_get_object(builder, "Notation"));
	switch(level){
		case 0:
		{
			//printf("0\n");
			GList* l = gtk_container_get_children(GTK_CONTAINER(vbox));
			for(; l!=NULL;l=l->next)
			{
				gtk_container_remove(GTK_CONTAINER(vbox),l->data);
			}
			g_list_free(l);
			if(g_list_length(node->children)!=0)
			{
				GList* elem = node->children;
				for(; elem!=NULL; elem = elem->next) 
				{
					tnode* item = elem->data;
					show_state(item,1);
				}
				g_list_free(elem);
			}
			break;
		}
		case 1:
		{
			//printf("1\n");

			GtkBox* subtreehbox =  GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
			gtk_container_add(GTK_CONTAINER(vbox),GTK_WIDGET(subtreehbox));
			char* label = get_label(node);
			GtkButton *button = GTK_BUTTON(gtk_button_new_with_label(label));
			free(label);
			if (node == tree->current) {
				GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(button));
				gtk_style_context_add_class(context,"selected");
			}
			g_signal_connect(button, "clicked", G_CALLBACK(select_state), (gpointer)node);

			gtk_widget_set_size_request(GTK_WIDGET(button), 240, 50);
			
			gtk_container_add(GTK_CONTAINER(subtreehbox), GTK_WIDGET(button));
			//if there is more than 1 child we create a vbox for them
			GtkBox* subtreebox = NULL;
			if (g_list_length(node->children) > 1) {
				subtreebox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
				gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(subtreebox));
			}
			//first child gets level 1 and is shown last, second - level 2, third - level 3...
			if(g_list_length(node->children)!=0)
			{
				tnode* first_item;
				int j=0;
				GList* elem = node->children;
				for(; elem!=NULL; elem = elem->next) 
				{
					tnode* item = elem->data;
					(*item).indent = node->indent; 
					if(j==0)
					{
						j=1;
						first_item = item;
						continue;
					}
					(*item).indent++;
					(*item).vbox = subtreebox;
					GtkBox* subtreehbox =  GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
					//gtk_widget_set_size_request(GTK_WIDGET(subtreehbox), 300, 100);
					(*item).hbox = subtreehbox;
					
					gtk_container_add(GTK_CONTAINER(subtreebox), GTK_WIDGET(subtreehbox));
					//reordering
					GValue targetIndex = G_VALUE_INIT;
					g_value_init (&targetIndex, G_TYPE_INT);
					gtk_container_child_get_property(GTK_CONTAINER(subtreebox),GTK_WIDGET(subtreehbox),"position",&targetIndex);
					gtk_box_reorder_child (GTK_BOX(subtreebox),GTK_WIDGET(subtreehbox),g_value_get_int(&targetIndex) + 1);
					//
					show_state(item,2);
				}
				g_list_free(elem);
				show_state(first_item,1);
			}
			gtk_widget_show_all(GTK_WIDGET(vbox));
			break;
		}
		default:
		{
			tnode* parent = (tnode*)node->parent;
			GtkBox* subtreebox = node->vbox;
			GtkBox *hbox=node->hbox;
			char* label=get_label(node);
			if((*node).hbox_status == 0)
			{
	 			GtkTextBuffer* tb = gtk_text_buffer_new (NULL);
				gchar *text =  get_sign(node->indent); 
				gtk_text_buffer_set_text (tb,text,strlen(text));
				GtkEntry *textArea = (gtk_text_view_new_with_buffer(tb));
				gtk_container_add(GTK_CONTAINER(hbox), GTK_WIDGET(textArea));
				free(text); 
			}

			//button creation
			GtkButton *button = GTK_BUTTON(gtk_button_new_with_label(label));
			gtk_widget_set_size_request(GTK_WIDGET(button), 120, 50);
			gtk_container_add(GTK_CONTAINER(hbox), GTK_WIDGET(button));
			if (node == tree->current) 
			{
				GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(button));
				gtk_style_context_add_class(context,"selected");
			}
			g_signal_connect(button, "clicked", G_CALLBACK(select_state), (gpointer)node);
			
			free(label);
			//going through children
			if(g_list_length(node->children)!=0)
			{
				tnode* first_item;
				int t_level = level ;
				level++;
				int j=0;
				GList* elem = node->children ;
				for(; elem!=NULL; elem = elem->next) 
				{
					tnode* item = elem->data;
					(*item).vbox = subtreebox;
					(*item).indent = (node->indent) + 1; 
					if(j==0)
					{
						j=1;
						(*item).hbox_status=1;
						(*item).hbox=hbox;
						first_item=item;
						continue;
					}
					(*item).hbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
					gtk_container_add(GTK_CONTAINER(subtreebox), GTK_WIDGET(item->hbox));
					//reordering
					GValue targetIndex = G_VALUE_INIT;
					g_value_init (&targetIndex, G_TYPE_INT);
					gtk_container_child_get_property(GTK_CONTAINER(subtreebox),GTK_WIDGET(node->hbox),"position",&targetIndex);
					//puts("afsd");
					gtk_box_reorder_child (subtreebox,GTK_WIDGET(item->hbox),g_value_get_int(&targetIndex) + 1);
					//
					show_state(item,level);
					
				}
				g_list_free(elem);
				show_state(first_item,t_level);
			}
			//printf("first level default\n");
			gtk_widget_show_all(GTK_WIDGET(vbox));
			gtk_widget_show_all(GTK_WIDGET(subtreebox));
			break;
		}
	}
}