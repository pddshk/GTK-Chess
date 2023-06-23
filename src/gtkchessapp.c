#include "gtkchessapp.h"
#include "board.h"
#include "state_tree.h"

enum _EngineState{
	ENGINE_NONE,
    ENGINE_OFF,
    ENGINE_IDLE,
    ENGINE_WORKING,
    ENGINE_ERROR
} engine_state;

void gtkchess_app_startup(
	__attribute_maybe_unused__ GApplication *app,
	__attribute_maybe_unused__ gpointer data)
{
    engine_manager = NULL;
	
	if (!start_engine_manager(engine_manager)){
		fputs("Error while starting engine!\n", stderr);
		engine_state = ENGINE_NONE;
	} else {
		engine_state = ENGINE_IDLE;
	}
	init_tree(&tree, NULL);
}

void gtkchess_app_activate(
	GApplication *app,
	__attribute_maybe_unused__ gpointer data)
{
    GList *windows=gtk_application_get_windows(GTK_APPLICATION(app));
    if (!windows){
        builder = builder_init();
        mainwindow = GTK_WINDOW(gtk_builder_get_object(builder, "MainWindow"));
        gtk_application_add_window(GTK_APPLICATION(app), mainwindow);
        gtk_widget_show_all(GTK_WIDGET(mainwindow));
    } else {
        gtk_window_present(mainwindow);
    }
}

void gtkchess_app_shutdown(
	__attribute_maybe_unused__ GApplication *self,
	__attribute_maybe_unused__ gpointer data)
{
	g_source_remove(g_source_get_id(from_engine_manager_source));
    tell_engine_manager(QUIT, NULL, 0);
    if (G_IS_SUBPROCESS(engine_manager) &&
			!g_subprocess_get_if_exited(engine_manager)) {
		fprintf(stderr, "Warning: Force exiting engine manager");
        g_subprocess_force_exit(engine_manager);
	}
	destroy_tree(&tree);
}

void gtkchess_app_open(
	__attribute_maybe_unused__ GApplication* app,
	__attribute_maybe_unused__ gpointer data)
{
}

int start_engine_manager(GSubprocess *engine_manager)
{
	GError *err = NULL;
	engine_manager = g_subprocess_new(
		G_SUBPROCESS_FLAGS_STDIN_PIPE | G_SUBPROCESS_FLAGS_STDOUT_PIPE,
		&err,
		"./engine_manager",
		NULL
	);
	if (err) {
		fprintf(stderr, "Cannot create subprocess for engine manager! Engine would be unavailable.\n %s", err->message);
		g_error_free(err);
		return FALSE;
	}
	to_engine_manager = g_subprocess_get_stdin_pipe(engine_manager);
	GInputStream *from_engine_manager = g_subprocess_get_stdout_pipe(engine_manager);
	if (!to_engine_manager || !from_engine_manager) {
		fprintf(stderr, "Cannot create pipes for engine manager. Engine would be unavailable. Try relaunching application\n");
		return FALSE;
	}
	from_engine_manager_source = NULL;
	if (g_pollable_input_stream_can_poll(G_POLLABLE_INPUT_STREAM(from_engine_manager))){
		from_engine_manager_source = g_pollable_input_stream_create_source(
			G_POLLABLE_INPUT_STREAM(from_engine_manager),
			NULL
		);
		g_source_attach(
			from_engine_manager_source,
			NULL // to default context
		);
	} else {
		fprintf(stderr, "Cannot create pollable stream from engine manager!\n");
		return FALSE;
	}
	tell_engine_manager(LOAD_ENGINE, "stockfish", strlen("stockfish"));
	int code=-1;
	size_t size=0;
	g_input_stream_read(from_engine_manager, &code, sizeof code, NULL, &err);
	if (err){
		fprintf(stderr, "Cannot read from engine manager: %s", err->message);
		g_error_free(err);
	}
	g_input_stream_read(from_engine_manager, &size, sizeof size, NULL, NULL);
	// printf("returned code: %d\n", code);
	return code == SUCCESS;
}

GtkBuilder *builder_init(void)
{
	GtkCssProvider* provider = gtk_css_provider_new();
	GdkDisplay* display = gdk_display_get_default();
	GdkScreen* screen = gdk_display_get_default_screen (display);
	gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
	gtk_css_provider_load_from_resource(provider, "/org/gtk/gtkchess/selected.css");

	//cppcheck-suppress shadowVariable
    GtkBuilder *builder=gtk_builder_new_from_resource("/org/gtk/gtkchess/window.glade");
    GObject* window=gtk_builder_get_object(builder, "MainWindow");
	gtk_window_set_default_size(GTK_WINDOW(window), 1600, 900);
	vbox = GTK_BOX(gtk_builder_get_object(builder, "Notation"));
    GtkWidget *Board = GTK_WIDGET(gtk_builder_get_object(builder, "Board"));

    GdkPixbuf *empty_icon = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 0, 8, 1, 1);
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

	g_source_set_callback(
		from_engine_manager_source,
		G_SOURCE_FUNC(parse_engine_response),
		builder,
		NULL
	);
	variations[0] = GTK_LABEL(gtk_builder_get_object(builder, "Variation0"));
	variations[1] = GTK_LABEL(gtk_builder_get_object(builder, "Variation1"));
	variations[2] = GTK_LABEL(gtk_builder_get_object(builder, "Variation2"));
	variations[3] = GTK_LABEL(gtk_builder_get_object(builder, "Variation3"));
	init_textures();
	// load_textures("classic");
    return builder;
}

void flip_board(__attribute_maybe_unused__ GtkButton* button, gpointer Board)
{
	flipped = !flipped;
	gtk_widget_queue_draw(GTK_WIDGET(Board));
}

void new_game(__attribute_maybe_unused__ GtkButton* button, gpointer Board)
{
	destroy_tree(&tree);
	init_tree(&tree, NULL);
	tree.root->state.flipped = flipped;
	gtk_widget_queue_draw(GTK_WIDGET(Board));
	show_state(tree.root,0);
}

gboolean parse_engine_response(GObject* stream, __attribute_maybe_unused__ gpointer data)
{
	int code=0;
	size_t nbytes=0;
	gssize nread;
	char buff[2048] = "";
	GError *error = NULL;
	g_pollable_input_stream_read_nonblocking(
		G_POLLABLE_INPUT_STREAM(stream),
		&code,
		sizeof code,
		NULL,
		&error
	);
	if (error) {
		puts(error->message);
		g_error_free(error);
		return TRUE;
	}
	g_pollable_input_stream_read_nonblocking(
		G_POLLABLE_INPUT_STREAM(stream),
		&nbytes,
		sizeof nbytes,
		NULL,
		&error
	);
	if (error) {
		puts(error->message);
		g_error_free(error);
		return TRUE;
	}
	nread = g_pollable_input_stream_read_nonblocking(
		G_POLLABLE_INPUT_STREAM(stream),
		buff,
		nbytes,
		NULL,
		&error
	);
	if (error) {
		puts(error->message);
		g_error_free(error);
		return TRUE;
	}
	if (nread == G_IO_ERROR_WOULD_BLOCK) return TRUE;
	//handle message by code
	switch (code)
	{
	case INFO:
		printf("info %s\n", buff);
		break;
	case BESTMOVE:
		printf("bestmove %s\n", buff);
		break;
	case FAILURE:
		printf("Something went wrong\n");
		break;
	default:
		printf("got code %d\n", code);
		break;
	}
	return TRUE;
}

void toggle_engine(GtkButton* self, __attribute_maybe_unused__ gpointer data)
{
	switch (engine_state) {
		case ENGINE_IDLE:
			//puts("Starting engine!");
			tell_engine_manager(GO, NULL, 0);
			//g_output_stream_write(to_engine_manager, "go\n", (sizeof "go\n") - 1, NULL, NULL);
			engine_state = ENGINE_WORKING;
			gtk_button_set_label(self, "Stop");
			break;
		case ENGINE_WORKING:
			//puts("Stopping engine!");
			tell_engine_manager(STOP, NULL, 0);
			//g_output_stream_write(to_engine_manager, "stop\n", (sizeof "stop\n") - 1, NULL, NULL);
			engine_state = ENGINE_IDLE;
			gtk_button_set_label(self, "Go");
			break;
		case ENGINE_NONE:
			puts("No engine to start!");
			break;
		case ENGINE_OFF:
			//puts("Engine is off!");
			break;
		case ENGINE_ERROR:
			puts("Engine is broken!");
			break;
		default: break;
	}
}

void tell_engine_manager(int type, const void* data, size_t size)
{
    g_output_stream_write(to_engine_manager, &type, sizeof type, NULL, NULL);
	g_output_stream_write(to_engine_manager, &size, sizeof size, NULL, NULL);
    if (size)
        g_output_stream_write(to_engine_manager, data, size, NULL, NULL);
    g_output_stream_flush(to_engine_manager, NULL, NULL);
}

void add_variation(
	__attribute_maybe_unused__ GtkButton* self,
	__attribute_maybe_unused__ gpointer data)
{
	// printf("nvariations = %d\n",nvariations);
	// fflush(stdout);
	if (nvariations > 3) return;
	nvariations++;
	char text[2]="1";
	// sprintf(text,"%d",nvariations);
	gtk_label_set_text(variations[nvariations-1], text);
}

void rm_variation(
	__attribute_maybe_unused__ GtkButton* self,
	__attribute_maybe_unused__ gpointer data)
{
	printf("nvariations = %d\n",nvariations);
	fflush(stdout);
	if (nvariations < 2) return;
	gtk_label_set_text(variations[nvariations-1], "");
	nvariations--;
}

gchar* get_sign(int number,char symbol)
{
	number++;
	//printf("%d/n", number);
	gchar* st = (gchar*)malloc(sizeof(gchar)*(number*40 + 1));
	for(int i = 0; i< number * 40;i++) {
		st[i] = symbol;
	}
	st[number * 40] = '\0';
	return st;

}

// Bullshit
// void get_FEN(__attribute_maybe_unused__ GtkButton* button, gpointer data)
// {
// 	GtkWidget* widget = GTK_WIDGET(data);
// 	GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG (widget));
// 	GList *children = gtk_container_get_children(GTK_CONTAINER(content_area));
// 	GtkWidget *grid = children->data;
// 	GList *gchildren = gtk_container_get_children(GTK_CONTAINER(grid));
	
//     GtkEntry* entry = GTK_ENTRY(gchildren->next->data);
//     FEN_to_state(gtk_entry_get_text(entry));
// 	game_state state = tree.current->field;
// 	destroy_tree(&tree);
// 	init_tree(state);
// 	show_state(tree.root, 0);
//     gtk_widget_destroy (widget); // This will close the dialog
// 	//gtk_widget_queue_draw(GTK_WIDGET(gtk_builder_get_object(builder, "Board")));
// }

// void paste_FEN(
// 	__attribute_maybe_unused__ GtkButton* main_window_button,
// 	__attribute_maybe_unused__ gpointer data
// ) {
//     GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "MainWindow"));
//     GtkWidget *dialog;
//     GtkWidget *content_area;
//     GtkWidget *grid;
//     GtkWidget *label;
//     //GtkWidget *button;
//     static GtkEntry *textbox;

//     dialog = gtk_dialog_new_with_buttons ("Get Text",
//                                           GTK_WINDOW(window),
//                                           GTK_DIALOG_MODAL,
// 										  0,
//                                           NULL);
//     content_area = gtk_dialog_get_content_area(GTK_DIALOG (dialog));
//     grid = gtk_grid_new();
//     gtk_container_add (GTK_CONTAINER (content_area), grid);

//     label = gtk_label_new("Paste FEN: ");
//     gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
//     textbox = GTK_ENTRY(gtk_entry_new());
//     gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(textbox), 1, 0, 1, 1);
// 	GtkWidget *okbutton = gtk_button_new_with_label("OK");
// 	gtk_grid_attach(GTK_GRID(grid), okbutton, 0, 2, 30, 20);
//     gtk_widget_show_all(dialog);

//     g_signal_connect (okbutton, "clicked", G_CALLBACK (get_FEN), dialog);
// }


//Bullshit as well
// void get_PGN(__attribute_maybe_unused__ GtkButton* button, gpointer data)
// {
// 	//PGN_to_tree(gtk_entry_get_text(entry));
// 	//destroy_tree(tree);GtkWidget* widget = GTK_WIDGET(data);
// 	GtkWidget* widget = GTK_WIDGET(data);
// 	GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG (widget));
// 	GList *children = gtk_container_get_children(GTK_CONTAINER(content_area));
// 	GtkWidget *grid = children->data;
// 	GList *gchildren = gtk_container_get_children(GTK_CONTAINER(grid));
// 	//destroy_tree(tree);
	
//     GtkEntry* entry = GTK_ENTRY(gchildren->next->data);
// 	PGN_to_tree((char*)gtk_entry_get_text(entry));
// 	//show_state(tree->root, 0);
//     gtk_widget_destroy(widget); 
// 	//show_state(tree->root, 0);
//     //gtk_widget_destroy (widget); // This will close the dialog
// 	//gtk_widget_queue_draw(GTK_WIDGET(gtk_builder_get_object(builder, "Board")));
// }

// void paste_PGN(__attribute_maybe_unused__ GtkButton* main_window_button, __attribute_maybe_unused__ gpointer data) 
// {
// 	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "MainWindow"));
//     GtkWidget *dialog;
//     GtkWidget *content_area;
//     GtkWidget *grid;
//     GtkWidget *label;
//     //GtkWidget *button;
//     static GtkEntry *textbox;

//     dialog = gtk_dialog_new_with_buttons ("Get Text",
//                                           GTK_WINDOW(window),
//                                           GTK_DIALOG_MODAL,
// 										  0,
//                                           NULL);
//     content_area = gtk_dialog_get_content_area(GTK_DIALOG (dialog));
//     grid = gtk_grid_new();
//     gtk_container_add (GTK_CONTAINER (content_area), grid);

//     label = gtk_label_new("Paste PGN: ");
//     gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
//     textbox = GTK_ENTRY(gtk_entry_new());
//     gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(textbox), 1, 0, 1, 1);
// 	GtkWidget *okbutton = gtk_button_new_with_label("OK");
// 	gtk_grid_attach(GTK_GRID(grid), okbutton, 0, 2, 30, 20);
//     gtk_widget_show_all(dialog);

//     g_signal_connect (okbutton, "clicked", G_CALLBACK (get_PGN), dialog);
// }

void select_state(__attribute_maybe_unused__ GtkButton* button, gpointer node) {
	tree.current = (tnode*)node;
	gtk_widget_queue_draw(GTK_WIDGET(gtk_builder_get_object(builder, "Board")));
	show_state(tree.root, 0);
	
}

void show_state(tnode* node, int level) 
{
	if (node==NULL)
	return;
	switch(level)
	{
		case 0:
		{
			//erasing tree graphics
			GList* l = gtk_container_get_children(GTK_CONTAINER(vbox));
			for(; l;l=l->next)
			{
				gtk_container_remove(GTK_CONTAINER(vbox),l->data);
			}
			g_list_free(l);

			//visualising zero move
			GtkBox* subtreehbox =  GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
			gtk_container_add(GTK_CONTAINER(vbox),GTK_WIDGET(subtreehbox));
			const char label[11] = "Game Start";
			GtkButton *button = GTK_BUTTON(gtk_button_new_with_label(label));
			if (node == tree.current) {
				GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(button));
				gtk_style_context_add_class(context,"selected");
			}
			g_signal_connect(button, "clicked", G_CALLBACK(select_state), (gpointer)node);
			gtk_widget_set_size_request(GTK_WIDGET(button), 240, 50);
			gtk_container_add(GTK_CONTAINER(subtreehbox), GTK_WIDGET(button));

			//visualising tree
			if(g_list_length(node->children)!=0)
			{
				GList* elem = node->children;
				for(; elem; elem = elem->next) 
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
			

			GtkBox* subtreehbox =  GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
			gtk_container_add(GTK_CONTAINER(vbox),GTK_WIDGET(subtreehbox));
			char* label = malloc(sizeof(char)* 10);
			get_label(node,label);

			GtkTextBuffer* tb = gtk_text_buffer_new (NULL);
			gchar *text;
			if(label[0]=='1'&&label[2]!='.')
			{
				text=(gchar*)malloc(sizeof(gchar)*(62));
				text[0] = '|';
				text[1]='\n';
				text[2] = '|';
				for(int i = 3; i< 62;i++) {
					text[i] = '-';
				}
				text[61] = '\0';
			}
			else
			text =  get_sign(1,' ');


			gtk_text_buffer_set_text (tb,text,strlen(text));
			GtkTextView *textArea = GTK_TEXT_VIEW(gtk_text_view_new_with_buffer(tb));
			gtk_container_add(GTK_CONTAINER(subtreehbox), GTK_WIDGET(textArea));
			free(text); 

			GtkButton *button = GTK_BUTTON(gtk_button_new_with_label(label));
			free(label);
			if (node == tree.current) {
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
				GList* elem = node->children;
				tnode* first_item = elem->data;
				(*first_item).indent = node->indent; 
				elem = elem->next;
				for(; elem; elem = elem->next) 
				{
					tnode* item = elem->data;
					(*item).indent = node->indent; 
					(*item).indent++;
					(*item).vbox = subtreebox;
					GtkBox* child_subtreehbox =  GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
					//gtk_widget_set_size_request(GTK_WIDGET(subtreehbox), 300, 100);
					(*item).hbox = child_subtreehbox;
					
					gtk_container_add(GTK_CONTAINER(subtreebox), GTK_WIDGET(child_subtreehbox));
					//reordering
					GValue targetIndex = G_VALUE_INIT;
					g_value_init (&targetIndex, G_TYPE_INT);
					gtk_container_child_get_property(GTK_CONTAINER(subtreebox),GTK_WIDGET(child_subtreehbox),"position",&targetIndex);
					gtk_box_reorder_child (GTK_BOX(subtreebox),GTK_WIDGET(child_subtreehbox),g_value_get_int(&targetIndex) + 1);
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
			//tnode* parent = (tnode*)node->parent;
			GtkBox* subtreebox = node->vbox;
			GtkBox *hbox=node->hbox;
			char* label=malloc(sizeof(char)* 10);
			get_label(node,label);
			if((*node).hbox_status == 0)
			{
	 			GtkTextBuffer* tb = gtk_text_buffer_new (NULL);
				gchar *text =  get_sign(node->indent,' '); 
				gtk_text_buffer_set_text (tb,text,strlen(text));
				GtkTextView *textArea = GTK_TEXT_VIEW(gtk_text_view_new_with_buffer(tb));
				gtk_container_add(GTK_CONTAINER(hbox), GTK_WIDGET(textArea));
				free(text); 
			}

			//button creation
			GtkButton *button = GTK_BUTTON(gtk_button_new_with_label(label));
			gtk_widget_set_size_request(GTK_WIDGET(button), 120, 50);
			gtk_container_add(GTK_CONTAINER(hbox), GTK_WIDGET(button));
			
			if (node == tree.current) 
			{
				GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(button));
				gtk_style_context_add_class(context,"selected");
			}
			g_signal_connect(button, "clicked", G_CALLBACK(select_state), (gpointer)node);
			
			free(label);
			//going through children
			if(g_list_length(node->children)!=0)
			{
				GList* elem = node->children;
				tnode* first_item = elem->data;
				int t_level = level;
				level++;
				(*first_item).vbox = subtreebox;
				(*first_item).indent = (node->indent) + 1; 
				(*first_item).hbox_status=1;
				(*first_item).hbox=hbox;
				elem = elem->next;
				for(; elem; elem = elem->next) 
				{
					tnode* item = elem->data;
					(*item).vbox = subtreebox;
					(*item).indent = (node->indent) + 1; 
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
