#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
#include <unistd.h>
#include <stdlib.h>
#include "gui.h"
#include "globals.h"

int start_engine_manager(GSubprocess*);

int read_settings(struct _settings*);

int main(int argc, char** argv)
{
	read_settings(&settings);
	GSubprocess *engine_manager = NULL;
	gtk_init(&argc, &argv);
	if (!start_engine_manager(engine_manager))
		fputs("Error while starting engine!\n", stderr);
	init_elements(settings.textures);
	gtk_main();
	g_output_stream_write(to_engine_manager,"quit\n",(sizeof "quit\n")-1, NULL, NULL);
    return EXIT_SUCCESS;
}

int start_engine_manager(GSubprocess *engine_manager)
{
	engine_manager = g_subprocess_new(
		G_SUBPROCESS_FLAGS_STDIN_PIPE | G_SUBPROCESS_FLAGS_STDOUT_PIPE,
		NULL,
		"./engine_manager",
		NULL
	);
	if (!engine_manager) {
		fprintf(stderr, "Cannot create subprocess for engine manager! Engine would be unavailable.\n");
		return FALSE;
	}
	to_engine_manager = g_subprocess_get_stdin_pipe(engine_manager);
	GInputStream *from_engine_manager = g_subprocess_get_stdout_pipe(engine_manager);
	if (!to_engine_manager || !from_engine_manager) {
		fprintf(stderr, "Cannot create pipes for engine manager. Engine would be unavailable.\n");
		return FALSE;
	}
	from_engine_manager_source = NULL;
	if (g_pollable_input_stream_can_poll(G_POLLABLE_INPUT_STREAM(from_engine_manager))){
		from_engine_manager_source = g_pollable_input_stream_create_source(
			G_POLLABLE_INPUT_STREAM(from_engine_manager),
			NULL
		);
		g_source_attach(from_engine_manager_source, NULL); // to default context
	} else {
		fprintf(stderr, "Cannot create pollable stream from engine manager!\n");
		exit(EXIT_FAILURE);
	}
	char buff[2048] = "";
	gsize nread;
	nread = g_input_stream_read(from_engine_manager, buff, sizeof buff, NULL, NULL);
	buff[nread] = 0;
	return !!strstr(buff, "ready");
}

int read_settings(struct _settings* settings)
{
	FILE *settings_file = fopen("config/settings.conf", "r");
	const char* const settings_names[32] = {"engine", "textures"};
	int nsettings = 2;

	if (settings_file) {
		while (!feof(settings_file)) {
			char buff[128];
			fgets(buff, sizeof buff, settings_file);
			buff[strlen(buff)-1]=0; // cut \n
			char *delim=strstr(buff, "=");
            if (delim){
                *delim = 0;
                delim++;
				for (int i = 0; i < nsettings; i++){
					if (strcmp(buff, settings_names[i]) == 0){
						switch (i) {
							case 0:
								strcpy(settings->engine, delim);
								break;
							case 1:
								strcpy(settings->textures, delim);
								break;
						}
						break;
					}
				}
            }
		}
		fclose(settings_file);
	} else {
		perror("Cannot open settings file:");
		return FALSE;
	}
	return TRUE;
}
