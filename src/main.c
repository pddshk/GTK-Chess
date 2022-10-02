#include <gtk/gtk.h>
#include <gio/gio.h>
#include <librsvg/rsvg.h>
#include <unistd.h>
#include <stdlib.h>
#include "gui.h"
//#include "engine_manager.h"
#define UCI "uci\n"
#define ISREADY "isready\n"

#include "globals.h"

int main(int argc, char** argv)
{
	// GSubprocess *engine = g_subprocess_new(
	// 	G_SUBPROCESS_FLAGS_STDIN_PIPE | G_SUBPROCESS_FLAGS_STDOUT_PIPE,
	// 	NULL,
	// 	"engines/stockfish_15",
	// 	NULL
	// );
	// GOutputStream *to_engine = g_subprocess_get_stdin_pipe(engine);
	// GInputStream *from_engine = g_subprocess_get_stdout_pipe(engine);
	// if (to_engine) puts("to engine is not null");
	// if (from_engine) puts("from_engine is not null");
	// char buff[2048];
	// gsize nread;
	// puts("header\n");
	// nread = g_input_stream_read(from_engine, buff, sizeof buff, NULL, NULL);
	// buff[nread] = 0;
	// printf("read %lu bytes:\n%s\n", nread, buff);
	//
	// puts(UCI);
	// g_output_stream_write(to_engine, UCI, (sizeof UCI) - 1, NULL, NULL);
	// g_output_stream_flush(to_engine, NULL, NULL);
	// nread = g_input_stream_read(from_engine, buff, sizeof buff, NULL, NULL);
	// buff[nread] = 0;
	// printf("read %lu bytes:\n%s\n", nread, buff);
	//
	// puts(ISREADY);
	// g_output_stream_write(to_engine, ISREADY, (sizeof ISREADY) - 1, NULL, NULL);
	// g_output_stream_flush(to_engine, NULL, NULL);
	// nread = g_input_stream_read(from_engine, buff, sizeof buff, NULL, NULL);
	// buff[nread] = 0;
	// printf("read %lu bytes:\n%s\n", nread, buff);

	gtk_init(&argc, &argv);
	init_elements();
	//g_unix_fd_add(from_engine_manager, G_IO_IN, GUnixFDSourceFunc function, gpointer user_data);
	gtk_main();
    return EXIT_SUCCESS;
}
