#include "engine_controls.h"
#include "interact.h"
#include "typedecl.h"

extern enum _EngineState engine_state;
extern GOutputStream *to_engine_manager;
extern int nvariations;
extern GtkLabel *variations[4];
extern GSource *from_engine_manager_source;

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
	if (nvariations > 3) return;
	nvariations++;
	char text[2]="1";
	sprintf(text,"%d",nvariations);
	gtk_label_set_text(variations[nvariations-1], text);
}

void rm_variation(
	__attribute_maybe_unused__ GtkButton* self,
	__attribute_maybe_unused__ gpointer data)
{
	fflush(stdout);
	if (nvariations < 2) return;
	gtk_label_set_text(variations[nvariations-1], "");
	nvariations--;
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
