#include "engine_manager.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include "interact.h"

int main()
{
    GSubprocess *engine = NULL;
    engine_params params;
    params.param_names = NULL;
    params.param_values = NULL;
    // wait for engine name
    while (1)
    {
        while (1)
        {
            int code;
            size_t nbytes;
            read(STDIN_FILENO, &code, sizeof code);
            
            // fprintf(stderr, "Got message code %d. ", code);

            read(STDIN_FILENO, &nbytes, sizeof nbytes);
            
            // fprintf(stderr, "Got data length %zu.\n", nbytes);
           
            if (code == LOAD_ENGINE){
                read(STDIN_FILENO, engine_name, nbytes);
                
                // fprintf(stderr, "%s\n", engine_name);
                
                break;
            } else {
                char buff[1024];
                read(STDIN_FILENO, buff, nbytes);
            }
        }

        char config_path[64] = "config/";
        strcat(config_path, engine_name);
        strcat(config_path, ".conf");
        
        // fprintf(stderr, "%s\n", config_path);
        
        FILE *config = fopen(config_path, "r");
        if (!config) {
            perror("Cannot open file");
            exit(EXIT_FAILURE);
        }
        clear_params(&params);
        fgets(params.exec_path, sizeof(params.exec_path), config);
        params.exec_path[strlen(params.exec_path) - 1] = 0; // trim newline
        fscanf(config, "%d", &(params.nparams)); fgetc(config);
        
        // fprintf(stderr, "engine location %s\nnparams %d\n", params.exec_path, params.nparams);
        
        params.param_names = calloc(params.nparams, sizeof *(params.param_names));
        params.param_values = calloc(params.nparams, sizeof *(params.param_values));
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
        if (run_engine(engine, &params)){
            
            // fputs("Engine started\n", stderr);
            
            tell_gui(DONE, NULL, 0);
            main_loop();
        }
        else
            fprintf(stderr, "init failed");
        fflush(stdout);
        g_subprocess_send_signal(engine, SIGTERM);
        g_subprocess_wait(engine, NULL, NULL);
        if (!g_subprocess_get_if_exited(engine))
            g_subprocess_force_exit(engine);
        g_output_stream_close(to_engine, NULL, NULL);
        g_input_stream_close(from_engine, NULL, NULL);
    }
}

int run_engine(GSubprocess* engine, engine_params* params)
{
    engine = g_subprocess_new(
        G_SUBPROCESS_FLAGS_STDIN_PIPE | G_SUBPROCESS_FLAGS_STDOUT_PIPE,
        NULL,
        params->exec_path,
        NULL
    );

    to_engine = g_subprocess_get_stdin_pipe(engine);
    from_engine = g_subprocess_get_stdout_pipe(engine);
    return init_engine(params);
}

int init_engine(engine_params* params)
{
    char buff[2048];
	gsize nread;
	nread = g_input_stream_read(from_engine, buff, sizeof buff, NULL, NULL);
	buff[nread] = 0;

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
    char buff[2048];
    while (1) {
    	gsize nread=g_input_stream_read(from_engine, buff, sizeof buff, NULL, NULL);
    	buff[nread] = 0;
        // parse input into lines
        char *start = buff, *end = strstr(buff, "\n");
        while (end) {
            // parse each line
            *(end++) = 0;
            char *delim = strstr(start, " ");
            *(delim++) = 0;
            // transfer data to gui
            if (strcmp(start, "info") == 0)
                tell_gui(INFO, delim, strlen(delim));
            else if (strcmp(start, "bestmove") == 0)
                tell_gui(BESTMOVE, delim, strlen(delim));
            
            start = end;
            end = strstr(start, "\n");
        }
    }
}

void main_loop()
{
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, engine_to_manager, NULL);
    while (1) {
        int code=0;
        size_t size=0;
        read(STDIN_FILENO, &code, sizeof code);
        read(STDIN_FILENO, &size, sizeof size);
        char buff[2048];
        size_t nread;
        nread = read(STDIN_FILENO, buff, size);
        if (nread == -1){
            perror("Cannot read from stdin:");
            pthread_cancel(thread_id);
            exit(EXIT_FAILURE);
        } else {
            buff[nread] = 0;
        }
        switch (code)
        {
        case QUIT:
            stop_engine();
            pthread_cancel(thread_id);
            exit(EXIT_SUCCESS);
            break;
        case GO:
            if (engine_state == ENGINE_IDLE)
                start_stop();
            break;
        case STOP:
            stop_engine();
            break;
        default:
            break;
        }
        //tell_engine(buff);
    }
    pthread_join(thread_id, NULL);
}

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

void clear_params(struct _engine_params* params)
{
    if (params->param_names) free(params->param_names);
    if (params->param_values) free(params->param_values);
    params->nparams=0;
    params->exec_path[0] = 0;
}

void tell_gui(int code, const void* data, size_t size)
{
    write(STDOUT_FILENO, &code, sizeof code);
    write(STDOUT_FILENO, &size, sizeof size);
    if (data)
        write(STDOUT_FILENO, data, size);
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