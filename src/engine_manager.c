#include "engine_manager.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

char engine_name[128] = "stockfish";
int to_engine, from_engine;

void engine_manager_main(int from_gui, int to_gui)
{
    // make 2 threads 
    while (1) {
        // read engine params from corresponding config file
        engine_params params;
        char config_path[64] = "config/";
        strcat(config_path, engine_name);
        strcat(config_path, ".conf");
        FILE *config = fopen(config_path, "r");
        fgets(params.exec_path, sizeof(params.exec_path), config);
        fgets(params.exec_file, sizeof(params.exec_file), config);
        int nparams;
        fscanf(config, "%d", &nparams);
        params.param_names = malloc(nparams * sizeof *(params.param_names));
        params.param_values = malloc(nparams * sizeof *(params.param_values));
        int i=0;
        while (!feof(config)){
            char buff[64];
            fgets(buff, 64, config);
            char * delim=strstr(buff,"=");
            if (!delim){
                fputs("Config parse error, no = sign found in parameter specification\n", stderr);
                // replace with retry
                // send message to gui about failure
                // wait for new config
                // continue outer loop
                exit(EXIT_FAILURE);
            }
            *delim = 0;
            delim++;
            strcpy(params.param_names[i], buff);
            strcpy(params.param_values[i], delim);
            i++;
        }
        fclose(config);

        // setup pipes for interacting with engine
        int to_engine_fd[2], from_engine_fd[2];
        pipe(from_engine_fd);
    	pipe(to_engine_fd);

        // forking engine process
        pid_t pid = -1;
        for (int i = 0; pid == -1 && i < 100; i++) pid = fork();
        switch (pid) {
            case 0: // engine process
                close(to_engine_fd[1]); // close from engine manager write end
                close(from_engine_fd[0]); // close to engine manager read end

                // redirecting engine outputs to pipes
                while (dup2(to_engine_fd[0], STDIN_FILENO) == -1 && (errno == EINTR));
                close(to_engine_fd[0]);
                while ((dup2(from_engine_fd[1], STDOUT_FILENO) == -1) && (errno == EINTR));
                close(from_engine_fd[1]);

                execl(params.exec_path, params.exec_file, NULL);
                perror("Cannot start engine");
                exit(EXIT_FAILURE);
            case -1:
                perror("Critical: cannot spawn process for engine, relaunch application");
                exit(EXIT_FAILURE);
            default:
                close(to_engine_fd[0]); // close from engine write end
                close(from_engine_fd[1]); // close to engine read end
                to_engine = to_engine_fd[1];
                from_engine = from_engine_fd[0];
                //main_loop(to_engine, from_engine, to_gui, from_gui);
                tell_engine(to_engine, "quit\n");
                kill(pid, SIGKILL);
                close(to_engine);
                close(from_engine);
        }
    }
}

void tell_engine(int to_engine, const char* command)
{
    //puts(command);
    write(to_engine, command, strlen(command));
}

// void init_engine()
// {
//     puts("Init engine...");
//     if (engine_status)
//         stop_engine();
//     skip_output();
//     char buff[4096];
//     buff[sizeof(buff)-1] = 0;
//     int nread;
//     //read(from_engine, buff, sizeof(buff));
//     tell_engine("uci\n");
//     do {
//         puts("reading from engine");
//         nread = read(from_engine, buff, sizeof(buff)-1);
//         //buff[nread]= 0;
//         printf("%d bytes read\n", nread);
//         puts(buff);
//     } while(nread == sizeof(buff)-1);
//     skip_output();
//     puts("isready");
//     tell_engine("isready\n");
//     do {
//         puts("reading from engine");
//         nread = read(from_engine, buff, sizeof(buff));
//         //buff[nread]= 0;
//         printf("%d bytes read\n", nread);
//         puts(buff);
//     } while(nread == sizeof(buff)-1);
//     puts("Init engine done!");
// }

// void skip_output()
// {
//     if (engine_status)
//         printf("Warning! Skipping engine output when it is calculating");
//     char buff[1024];
//     int nread;
//     puts("Skipping");
//     do {
//         nread = read(from_engine, buff, sizeof(buff)-1);
//         printf("read %d bytes\n", nread);
//         buff[nread] = 0;
//         puts(buff);
//     } while(nread == sizeof(buff)-1);
// }

void start_stop()
{
    switch (engine_state) {
        case ENGINE_IDLE: {
            char command[128] = "go\n";
            tell_engine(to_engine, command);
            engine_state = ENGINE_WORKING;
            break;
        }
        case ENGINE_WORKING:
            stop_engine();
            break;
    }
}

void stop_engine()
{
    tell_engine(to_engine, "stop");
    engine_state = ENGINE_IDLE;
}

gboolean update_variants(GtkBox *engine_response)
{
    GList *children1 = gtk_container_get_children(GTK_CONTAINER(engine_response));
    GtkActionBar *engine_controls = g_list_nth_data(children1, 0);
    GtkActionBar *evaluation_status = g_list_nth_data(children1, 1);
    GtkLabel *variants = g_list_nth_data(children1, 2);
    GList *children2 = gtk_container_get_children(GTK_CONTAINER(engine_controls));
    GtkLabel *position = g_list_nth_data(children2, 0);
    GtkButton *engine_toggler = g_list_nth_data(children2, 1);

    //char buff[64*1024];
    //char *res = NULL;
    // do {
    //     char *res = fgets(buff, 521, from_engine);
    //
    // } while(!res);
    return TRUE;
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
