#include "engine_manager.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

int main()
{
    GSubprocess *engine = NULL;
    engine_params params;
    params.param_names = NULL;
    params.param_values = NULL;
    // make 2 threads
    //while (1) {
        // read engine params from corresponding config file
        char config_path[64] = "config/";
        strcat(config_path, engine_name);
        strcat(config_path, ".conf");
        FILE *config = fopen(config_path, "r");
        if (!config) {
            perror("Cannot open file:");
            exit(EXIT_FAILURE);
        }
        fgets(params.exec_path, sizeof(params.exec_path), config);
        params.exec_path[strlen(params.exec_path) - 1] = 0;
        fscanf(config, "%d", &(params.nparams)); fgetc(config);
        //printf("engine location %s\nnparams %d\n", params.exec_path, params.nparams);
        if (params.param_names) free(params.param_names);
        if (params.param_values) free(params.param_values);
        params.param_names = malloc(params.nparams * sizeof *(params.param_names));
        params.param_values = malloc(params.nparams * sizeof *(params.param_values));
        for (int i=0; !feof(config) && i < params.nparams; i++){
            char buff[64];
            fgets(buff, sizeof buff, config);
            char * delim=strstr(buff,"=");
            if (delim){
                *delim = 0;
                delim++;
                strcpy(params.param_names[i], buff);
                strcpy(params.param_values[i], delim);
            }
        }
        fclose(config);
        //puts(params.exec_path);
        engine = g_subprocess_new(
    		G_SUBPROCESS_FLAGS_STDIN_PIPE | G_SUBPROCESS_FLAGS_STDOUT_PIPE,
    		NULL,
    		params.exec_path,
    		NULL
    	);

        to_engine = g_subprocess_get_stdin_pipe(engine);
    	from_engine = g_subprocess_get_stdout_pipe(engine);
        if (init_engine(&params)){
            puts("ready");
            fflush(stdout);
            main_loop();
        }
        else
            puts("init failed");
        fflush(stdout);
        g_subprocess_send_signal(engine, SIGTERM);
        g_subprocess_wait(engine, NULL, NULL);
        if (!g_subprocess_get_if_exited(engine))
            g_subprocess_force_exit(engine);
        g_output_stream_close(to_engine, NULL, NULL);
        g_input_stream_close(from_engine, NULL, NULL);
    //}
}

void tell_engine(const char* command)
{
    g_output_stream_write(
        to_engine,
        command,
        strlen(command),
        NULL,
        NULL
    );
    g_output_stream_flush(to_engine, NULL, NULL);
}

int init_engine(engine_params* params)
{
    char buff[2048];
	gsize nread;
	//puts("header\n");
	nread = g_input_stream_read(from_engine, buff, sizeof buff, NULL, NULL);
	buff[nread] = 0;
	// printf("read %lu bytes:\n%s\n", nread, buff);

	tell_engine(UCI);
	nread = g_input_stream_read(from_engine, buff, sizeof buff, NULL, NULL);
	buff[nread] = 0;
	// printf("read %lu bytes:\n%s\n", nread, buff);

    for (size_t i = 0; i < params->nparams; i++) {
        char command[64] = "setoption name ";
        strcat(command, params->param_names[i]);
        strcat(command, " value ");
        strcat(command, params->param_values[i]);
        strcat(command, "\n");
        // puts(command);
        tell_engine(command);
    }

	tell_engine(ISREADY);
	nread = g_input_stream_read(from_engine, buff, sizeof buff, NULL, NULL);
	buff[nread] = 0;
    // puts(buff);
    return !!strstr(buff, "readyok");
}

void start_stop()
{
    switch (engine_state) {
        case ENGINE_IDLE: {
            char command[128] = "go\n";
            tell_engine(command);
            engine_state = ENGINE_WORKING;
            break;
        }
        case ENGINE_WORKING:
            stop_engine();
            break;
        default: break;
    }
}

void stop_engine()
{
    tell_engine("stop\n");
    engine_state = ENGINE_IDLE;
}

void *engine_to_manager(void *data)
{
    while (1) {
        char buff[2048];
    	gsize nread;
        //puts("Reading from engine...");
    	nread = g_input_stream_read(from_engine, buff, sizeof buff, NULL, NULL);
    	buff[nread] = 0;
        //buffermessage
        //fputs(buff, stdout);
        fflush(stdout);
    	//printf("read %lu bytes from engine:\n%s\n", nread, buff);
    }
}

void main_loop()
{
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, engine_to_manager, NULL);
    while (1) {
        char buff[2048];
        size_t nread;
        nread = read(STDIN_FILENO, buff, sizeof buff);
        if (nread == -1){
            perror("Cannot read from stdin:");
            pthread_cancel(thread_id);
            exit(EXIT_FAILURE);
        } else {
            buff[nread] = 0;
        }
        if (strcmp(buff, "quit\n") == 0){
            puts("Shutting down engine...");
            stop_engine();
            pthread_cancel(thread_id);
            break;
        }
        tell_engine(buff);
    }
    //pthread_join(thread_id, NULL);
}

// gboolean update_variants(GtkBox *engine_response)
// {
//     GList *children1 = gtk_container_get_children(GTK_CONTAINER(engine_response));
//     GtkActionBar *engine_controls = g_list_nth_data(children1, 0);
//     GtkActionBar *evaluation_status = g_list_nth_data(children1, 1);
//     GtkLabel *variants = g_list_nth_data(children1, 2);
//     GList *children2 = gtk_container_get_children(GTK_CONTAINER(engine_controls));
//     GtkLabel *position = g_list_nth_data(children2, 0);
//     GtkButton *engine_toggler = g_list_nth_data(children2, 1);
//
//     //char buff[64*1024];
//     //char *res = NULL;
//     // do {
//     //     char *res = fgets(buff, 521, from_engine);
//     //
//     // } while(!res);
//     return TRUE;
// }

gboolean parse_engine_response(gint from_engine, GIOCondition condition, gpointer object)
{
    if (condition == G_IO_IN){
        char buff[4096];
        int nread = 0;
        do {
            nread = read(from_engine, buff, sizeof(buff));
            buff[nread] = 0;

        } while(nread == sizeof(buff));
    }
    return TRUE;
}
