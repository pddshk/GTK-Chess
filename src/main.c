#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
#include <unistd.h>
#include <stdlib.h>
#include "gui.h"
#include "globals.h"

// void *when_read(GObject*, GAsyncResult*, gpointer);

int main(int argc, char** argv)
{
	GSubprocess *engine_manager = g_subprocess_new(
		G_SUBPROCESS_FLAGS_STDIN_PIPE | G_SUBPROCESS_FLAGS_STDOUT_PIPE,
		NULL,
		"./engine_manager",
		//"./engines/stockfish_15",
		NULL
	);
	to_engine_manager = g_subprocess_get_stdin_pipe(engine_manager);
	from_engine_manager = g_subprocess_get_stdout_pipe(engine_manager);
	if (to_engine_manager) puts("to engine is not null");
	if (from_engine_manager) puts("from engine is not null");
	if (g_pollable_input_stream_can_poll(G_POLLABLE_INPUT_STREAM(from_engine_manager)))
		puts("from engine is pollable");
	char buff[2048] = "";
	gsize nread;
	g_output_stream_write(to_engine_manager, "uci\n", (sizeof "uci"), NULL, NULL);
	// g_input_stream_read_async(
	// 	from_engine_manager,
	// 	buff,
	// 	sizeof buff,
	// 	G_PRIORITY_DEFAULT,
	// 	NULL,
	// 	NULL,
	// 	NULL
	// );
	nread = g_input_stream_read(from_engine_manager, buff, sizeof buff, NULL, NULL);
	buff[nread] = 0;
	printf("From engine manager: %s", buff);
	gtk_init(&argc, &argv);
	init_elements();
	//g_unix_fd_add(from_engine_manager, G_IO_IN, GUnixFDSourceFunc function, gpointer user_data);
	gtk_main();
	g_output_stream_write(to_engine_manager,"quit\n",(sizeof "quit\n")-1, NULL, NULL);
    return EXIT_SUCCESS;
}

// void *when_read(GObject* source, GAsyncResult* res, gpointer buff){
//
// }
