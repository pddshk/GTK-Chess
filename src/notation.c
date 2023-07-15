#include "notation.h"
#include "state_tree.h"

extern state_tree tree;
extern GtkBuilder *builder;
extern GtkBox* vbox;

// allocates
gchar* get_sign(int number, char symbol)
{
	number++;
	gchar* st = malloc(sizeof(gchar) * (number*40 + 1));
	for(int i = 0; i < number * 40; i++) {
		st[i] = symbol;
	}
	st[number * 40] = '\0';
	return st;
}

void show_notation(const state_tree* tree)
{
	show_state(tree->root, 0);
}

void select_state(__attribute_maybe_unused__ GtkButton* button, tnode* node) {
	tree.current = node;
	gtk_widget_queue_draw(GTK_WIDGET(gtk_builder_get_object(builder, "Board")));
	show_notation(&tree);
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
			g_signal_connect(button, "clicked", G_CALLBACK(select_state), node);
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
				text[62] = '\0';
			}
			else
			text = get_sign(1,' ');


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
			g_signal_connect(button, "clicked", G_CALLBACK(select_state), node);

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
			g_signal_connect(button, "clicked", G_CALLBACK(select_state), node);
			
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
