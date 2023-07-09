#include "pgn.h"

// char* remove_PGN_comments(char* pgn) {
//     char* newpgn = malloc(sizeof(char) * strlen(pgn));
//     ;
//     int write = 0, ptr = 0;
//     for (size_t i = strrchr(pgn, ']') + 1 - pgn; i < strlen(pgn);  ++i) {
        
//         if (pgn[i] == '{') {
//             while (pgn[i] != '}') {
//                 ++i;
//             }
//             ++i;
//         }
//         if (pgn[i] == ';') {
//             while (pgn[i] != '\n') {
//                 ++i;
//             }
//             ++i;
//         }
//         if (write == 0) {
//             if (!isspace(pgn[i])) {
//                 write = 1;
//                 newpgn[ptr] = pgn[i];
//                 ++ptr;
//             }
//         }
//         else {
//             newpgn[ptr] = pgn[i];
//             ++ptr;
//         }
//     } 
//     char* newpgnResize = malloc(sizeof(char) * ptr);
//     for (int i = 0; i < ptr; i++) {
//         newpgnResize[i] = newpgn[i];
//     }
//     free(newpgn);
//     if(newpgnResize[ptr-1]=='\n')
//     newpgnResize[ptr-1]='\0';
//     return newpgnResize;
// }

// void PGN_to_tree(char* pgn) 
// {
    
//     pgn = remove_PGN_comments(pgn);

//     char* move_strings= strtok(pgn," \n");

//     GList* list = NULL;


//     //splits file into words
//     while (1)
//     {
//         char* word = (char*)malloc(sizeof(char)*8);
//         move_strings = strtok(NULL," \n");
//         if(move_strings==NULL)
//         break;

//         strcpy(word, move_strings);
//         //printf("%s\n",word);
        
//         list=g_list_append(list, word);
//     }

//     int len = g_list_length(list);
//     char figures[] = "RNBQKP";
//     char promotion = 0; 
//     int bw = 1; // black/white

//     game_state state = init_state_non_pointer();

//     //init_state(&state);

//     //analysing words
//     for(int j=0;j<len-1;j++)
//     {   
//         char* current_word= list->data;
//         char first_letter = current_word[0];
        
//         if (isdigit(first_letter)) {
//             list=list->next;
//             continue;
//         }
//         //puts(current_word);

//         bw = (bw + 1) % 2;

//         char dragged_piece;
//         int from_row=-1, from_col=-1, to_row=-1, to_col=-1;

//         int wordlen = strlen(current_word);
        
//         if(strchr(figures,first_letter))//if not pawn and not castling
//         {
                    
//             if(strrchr(current_word,'x')!=NULL)//capture
//             {
//                 if(wordlen > 4 && !isdigit(current_word[3])) //disambiguation check
//                 {
//                     if(isdigit(current_word[1]))    // is row or column disambiguated
//                     from_row = current_word[1] - 49;
//                     else
//                     from_col = current_word[1] - 97;
                    

//                     to_row = current_word[4]-49;
//                     to_col = current_word[3]-97;
//                 }
//                 else {
//                     to_row = current_word[3]-49;
//                     to_col = current_word[2]-97;
//                 }
//             }
//             else//non capture
//             {
//                 if(wordlen > 3 && !isdigit(current_word[2]))     //disambiguation check
//                 {
//                     if(isdigit(current_word[1]))      // is row or column disambiguated
//                     from_row = current_word[1] - 49;
//                     else
//                     from_col = current_word[1] - 97;

//                     to_row = current_word[3]-49;
//                     to_col = current_word[2]-97;
//                 }
//                 else {
//                     to_row = current_word[2]-49;
//                     to_col = current_word[1]-97;                   
//                 }
//             }
//             to_row = 7 - to_row;
//             dragged_piece=first_letter;
//             if (bw == 1) {             //if black
//                 dragged_piece = tolower(dragged_piece);
//             }

//             if(toupper(dragged_piece)==figures[0])//rook
//             {
//                 rook_search(dragged_piece, to_row, &from_row, to_col, &from_col);
//             }
                

//             if(toupper(dragged_piece)==figures[1])//knight
//             {
//                 const int a[] = {-2, -2, -1, -1, 1, 1, 2, 2};
//                 const int b[] = {-1, 1, -2, 2, -2, 2, -1, 1};

//                 for (int i = 0; i < 8; i++) {                       //checking all possible knight moves
//                     int row = to_row + a[i], col = to_col + b[i];
//                     if (row < 0 || row >= 8 || col < 0 || col >= 8) continue;       //out of field
//                     //if (current_word[0] == 'N' && current_word[2] == 'd') printf("%d %d %c %c %d %d\n", row, col, dragged_piece, state.field[row][col], from_row, from_col);
//                     if (from_col >= 0 && from_col != col) continue;                 //column or row is known
                    
//                     if (from_row >= 0 && from_row != row) continue;
//                     if(state.field[row][col]==dragged_piece) {
                        
//                         from_row = row;
//                         from_col = col;
//                         break;
//                     }
//                 }
//             }
            
//             if(toupper(dragged_piece)==figures[2])//bishop
//             {
//                 bishop_search(dragged_piece, to_row, &from_row, to_col, &from_col);
//             }

            
//             if(toupper(dragged_piece)==figures[3])//queen
//             {                                                               //no repeating code :)
//                rook_search(dragged_piece, to_row, &from_row, to_col, &from_col);
//                bishop_search(dragged_piece, to_row, &from_row, to_col, &from_col);
//             }
            
//             if(toupper(dragged_piece)==figures[4])//king
//             {
//                 const int a[] = {-1, 0, 1};
//                 for (int i = 0; i < 3; i++) {
//                     for (int k = 0; k < 3; k++) {
//                         int row = to_row + a[i], col = to_col + a[k];
//                         if (row < 0 || row >= 8 || col < 0 || col >= 8) continue;       //out of field
//                         if(state.field[row][col] == dragged_piece)
//                         {
//                             from_row=row;
//                             from_col = col;
//                         }
//                     }
//                 }
//             }
//         }
        
//         else /*if castling or pawn*/ 
//         {
//             if(first_letter=='O')//castling
//             {
//                 dragged_piece='K';
//                 if (bw == 1) {             //if black
//                     dragged_piece = tolower(dragged_piece);
//                 }

//                 from_col=4;
//                 int l = strlen(current_word);
//                 if(l<5)//kingside
//                 { 
//                     to_col=6;
//                 }
//                 else//queenside
//                 {
//                     to_col=2;
//                 }
//                 if(bw == 1)     //black
//                 {
//                     to_row = 0; 
//                     from_row=0;
//                 }
//                 else
//                 {
//                     to_row = 7; 
//                     from_row=7;
//                 }
//             }
//             else//pawn
//             {
//                 dragged_piece ='P';
//                 if (bw == 1) {             //if black
//                     dragged_piece = tolower(dragged_piece);
//                 }

//                 if(strchr(current_word,'=')!=NULL) {                //if promoted
//                     promotion = strchr(current_word,'=')[1];
//                     if (bw == 1) {             //if black
//                         promotion = tolower(promotion);
//                     }
//                 }
//                 if(strchr(current_word,'x')!=NULL)//capture by pawn
//                 {
//                     to_row = current_word[3]-49;
//                     to_row = 7 - to_row;
//                     to_col = current_word[2]-97;
//                     from_col = current_word[0]-97;
//                     if (bw == 1) from_row = to_row - 1; else from_row = to_row + 1;
//                 }
                
//                 else// if not capture
//                 {
//                     to_row = current_word[1]-49;
//                     to_row = 7 - to_row;
//                     to_col = current_word[0]-97;
//                     from_col = to_col;
//                     if (bw == 1) {
//                         if(state.field[to_row - 1][to_col] == dragged_piece)
//                         {
//                             from_row = to_row - 1;
//                         }
//                         else {
//                             from_row = to_row - 2;
//                         }
//                     }
//                     else {
//                         if(state.field[to_row + 1][to_col] == dragged_piece)
//                         {
//                             from_row = to_row + 1;
//                         }
//                         else {
//                             from_row = to_row + 2;
//                         }
//                     }
//                 }
                
//             }
//         }
//         //printf("%d %d %d %d\n", from_col, from_row, to_col, to_row);
//         set_field(&state, from_row, from_col, '-');
//         next_move(&state, dragged_piece, from_row, from_col, to_row, to_col,promotion);
//         //if (current_word[0] == 'R' && current_word[2] == 'f') return;
//         //if (current_word[0] == 'd' && current_word[1] == '4') return;
//         // for (int i = 0; i < 8; i++) {
//         //     puts(state.field[i]);
//         // }
//         list=list->next;
//     }
//     g_list_free(list);
// }

// void bishop_search(int dragged_piece, int to_row, int* from_row_ptr, int to_col, int* from_col_ptr) {
//     int from_col = *(from_col_ptr);
//     int from_row = *(from_row_ptr);
//     int row = to_row, col = to_col;
//     do {
//         row--;
//         col--;
//         if (from_col >= 0 && from_col != col) continue;                 //column or row is known
//         if (from_row >= 0 && from_row != row) continue;
//         if(tree.current->field.field[row][col]==dragged_piece) {
//             *from_row_ptr = row;
//             *from_col_ptr = col;
//             return;
//         }
//         if (tree.current->field.field[row][col] != '-') {
//             break;
//         }
//     } while (row >= 0 && col >= 0);
//     row = to_row, col = to_col;
//     do {
//         row++;
//         col++;
//         if (from_col >= 0 && from_col != col) continue;                 //column or row is known
//         if (from_row >= 0 && from_row != row) continue;
//         if(tree.current->field.field[row][col]==dragged_piece) {
//             *from_row_ptr = row;
//             *from_col_ptr = col;
//             return;
//         }
//         if (tree.current->field.field[row][col] != '-') {
//             break;
//         }
//     } while (row < 8 && col < 8);
//     row = to_row, col = to_col;
//     do {
//         row++;
//         col--;
//         if (from_col >= 0 && from_col != col) continue;                 //column or row is known
//         if (from_row >= 0 && from_row != row) continue;
//         if(tree.current->field.field[row][col]==dragged_piece) {
//             *from_row_ptr = row;
//             *from_col_ptr = col;
//             return;
//         }
//         if (tree.current->field.field[row][col] != '-') {
//             break;
//         }
//     } while (row < 8 && col > 0);
//     row = to_row, col = to_col;
//     do {
//         row--;
//         col++;
//         if (from_col >= 0 && from_col != col) continue;                 //column or row is known
//         if (from_row >= 0 && from_row != row) continue;
//         if(tree.current->field.field[row][col]==dragged_piece) {
//             *from_row_ptr = row;
//             *from_col_ptr = col;
//             return;
//         }
//         if (tree.current->field.field[row][col] != '-') {
//             break;
//         }
//     } while (row > 0 && col < 8);
// }

// void rook_search(int dragged_piece, int to_row, int* from_row_ptr, int to_col, int* from_col_ptr) {
//     int from_col = *from_col_ptr;
//     int from_row = *from_row_ptr;
//     if (from_row < 0) {
//         int row = to_row + 1, col = to_col;
//         if (from_col >= 0) col = from_col;
//         while (row < 8) {
//             if(tree.current->field.field[row][col]==dragged_piece)
//             {
//                 *from_row_ptr = row;
//                 *from_col_ptr = col;
//                 return;
//             }
//             else if (tree.current->field.field[row][col] != '-') {
//                 break;
//             }
//             row++;
//         }
//         row = to_row - 1, col = to_col;
//         if (from_col >= 0) col = from_col;
//         while (row >= 0) {
//             if(tree.current->field.field[row][col]==dragged_piece)
//             {
//                 *from_row_ptr = row;
//                 *from_col_ptr = col;
//                 return;
//             }
//             else if (tree.current->field.field[row][col] != '-') {
//                 break;
//             }
//             row--;
//         } 
//     }
    
//     if (from_col < 0) {
//         int row = to_row, col = to_col + 1;
//         if (from_row >= 0) row = from_row;
//         while (col < 8) {
//             if(tree.current->field.field[row][col]==dragged_piece)
//             {
//                 *from_row_ptr = row;
//                 *from_col_ptr = col;
//                 return;
//             }
//             else if (tree.current->field.field[row][col] != '-' ) {
//                 break;
//             }
//             col++;
//         } 
//         row = to_row, col = to_col - 1;
//         if (from_row >= 0) row = from_row;
//         while (col >= 0) {
//             if(tree.current->field.field[row][col]==dragged_piece)
//             {
//                 *from_row_ptr = row;
//                 *from_col_ptr = col;
//                 return;
//             }
//             else if (tree.current->field.field[row][col] != '-') {
//                 break;
//             }
//             col--;
//         } 
//     }
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
// 	//show_notation(&tree);
//     gtk_widget_destroy(widget); 
// 	//show_notations(&tree);
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