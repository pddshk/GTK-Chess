#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
#include <unistd.h>
#include <stdlib.h>
#include "gui.h"
#include "globals.h"
#include "interact.h"

int start_engine_manager(GSubprocess*, struct _settings*);

int read_settings(struct _settings*);

int main(int argc, char** argv)
{
	// puts("Main, read settings");
	
	read_settings(&settings);
	
	// puts("Main, read");
	
	GSubprocess *engine_manager = NULL;
	gtk_init(&argc, &argv);
	
	// puts("Main, starting engine manager");
	// puts(settings.engine);
	
	if (!start_engine_manager(engine_manager, &settings)){
		fputs("Error while starting engine!\n", stderr);
		return EXIT_FAILURE;
	}
	// puts("Main, engine manager started");
	// puts("Main, init elements");
	
	init_elements(settings.textures);
	
	//puts("Main, starting engine");
	
	gtk_main();
	tell_engine_manager(QUIT, NULL, 0);
	return EXIT_SUCCESS;
}

int start_engine_manager(GSubprocess *engine_manager, struct _settings* settings)
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
		fprintf(stderr, "Cannot create pipes for engine manager. Engine would be unavailable. Try relaunching application\n");
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
		return FALSE;
	}
	tell_engine_manager(LOAD_ENGINE, settings->engine, strlen(settings->engine));
	int code=0;
	size_t size=0;
	g_input_stream_read(from_engine_manager, &code, sizeof code, NULL, NULL);
	g_input_stream_read(from_engine_manager, &size, sizeof size, NULL, NULL);
	return code == DONE;
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
