#include "engine_manager.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/poll.h>

char engine_name[128] = "stockfish";
int to_engine, from_engine;

void engine_manager_main(int from_gui, int to_gui)
{
    while (1) {
        // read engine params from corresponding config file
        engine_params params;
        char config_path[64] = "config/";
        strcat(config_path, engine_name);
        strcat(config_path, ".conf");
        FILE *config = fopen(config_path, "r");
        fgets(params.exec_path, sizeof(params.exec_path), config);
        params.exec_path[strlen(params.exec_path)-1] = '/';
        fgets(params.exec_file, sizeof(params.exec_file), config);
        params.exec_file[strlen(params.exec_file)-1] = 0;
        strcat(params.exec_path, params.exec_file);
        puts(params.exec_path);
        int nparams;
        fscanf(config, "%d", &nparams);
        printf("naparms %d\n", nparams);
        fgetc(config); //skip \n
        params.param_names = malloc(nparams * sizeof *(params.param_names));
        params.param_values = malloc(nparams * sizeof *(params.param_values));
        int i=0;
        while (!feof(config) && i < nparams){
            char buff[64];
            fgets(buff, sizeof buff, config);
            puts(buff);
            printf("%d %d\n", i, buff[0]);
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
            printf("%s = %s\n", params.param_names[i], params.param_values[i]);
            i++;
        }
        fclose(config);

        printf("execl(\"%s\", \"%s\", NULL)\n", params.exec_path, params.exec_file);

        // setup pipes for interacting with engine
        int to_engine_fd[2], from_engine_fd[2];
        pipe(from_engine_fd);
    	pipe(to_engine_fd);

        // forking engine process
        pid_t pid = -1;
        for (int i = 0; pid == -1 && i < 10; i++) pid = fork();
        switch (pid) {
            case 0: // engine process
                close(to_engine_fd[1]); // close from engine manager write end
                close(from_engine_fd[0]); // close to engine manager read end

                // redirecting engine outputs to pipes
                while (dup2(to_engine_fd[0], STDIN_FILENO) == -1 && (errno == EINTR));
                close(to_engine_fd[0]);
                while ((dup2(from_engine_fd[1], STDOUT_FILENO) == -1) && (errno == EINTR));
                close(from_engine_fd[1]);
                puts(params.exec_path);
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
                sleep(10);
                interact(to_engine, from_engine, to_gui, from_gui);
                close(to_engine);
                close(from_engine);
                kill(pid, SIGKILL);
        }
    }
}

static void interact(int to_engine, int from_engine, int to_gui, int from_gui)
{
    pthread_t thread_id;
    init_engine(to_engine, from_engine);
    pthread_create(&thread_id, NULL, engine_to_gui, NULL);
    while (engine_state == ENGINE_WORKING || engine_state == ENGINE_IDLE) {
        char buff[256];
        read(from_gui, buff, sizeof(buff));
        puts(buff);
        sleep(10);
    }
    pthread_join(thread_id, NULL);
}

static void *engine_to_gui(void* data)
{
    while (engine_state == ENGINE_WORKING || engine_state == ENGINE_IDLE) {
        char buff[256];
        read(from_engine, buff, sizeof(buff));
        puts(buff);
        sleep(10);
    }
}

void tell_engine(const char* command)
{
    puts(command);
    write(to_engine, command, strlen(command));
}

void init_engine()
{
    struct pollfd from_engine_fd;
    from_engine_fd.fd = from_engine;
    from_engine_fd.events = POLLIN | POLLPRI;

    puts("Init engine...");
    if (engine_state == ENGINE_WORKING)
        stop_engine();
    skip_output();
    char buff[4096];
    buff[sizeof(buff)-1] = 0;
    int nread;
    tell_engine("uci\n");
    poll(&from_engine_fd, 1, 10);
    while (from_engine_fd.revents & (POLLIN | POLLPRI)) {
        puts("reading from engine");
        nread = read(from_engine, buff, sizeof(buff)-1);
        //buff[nread]= 0;
        printf("%d bytes read\n", nread);
        puts(buff);
        poll(&from_engine_fd, 1, 10);
    };
    buff[5] = 0;
    if (!strstr(buff, "uciok"))
        puts("Cannot load engine in UCI mode, it may not be accessible!");
    puts("isready");
    int isready=0, tries=0;
    do {
        tell_engine("isready\n");
        poll(&from_engine_fd, 1, 10);
        while (from_engine_fd.revents & (POLLIN | POLLPRI)) {
            puts("reading from engine");
            nread = read(from_engine, buff, sizeof(buff));
            //buff[nread]= 0;
            printf("%d bytes read\n", nread);
            puts(buff);
            poll(&from_engine_fd, 1, 10);
        }

    } while(!strstr(buff, "readyok") && tries < 10);
    puts("Init engine done!");
}

void skip_output()
{
    char buff[1024];
    int nread;
    struct pollfd from_engine_fd;
    from_engine_fd.fd = from_engine;
    from_engine_fd.events = POLLIN | POLLPRI;
    poll(&from_engine_fd, 1, 5);
    while ((from_engine_fd.revents & (POLLIN | POLLPRI))) {
        puts("Skipping");
        nread = read(from_engine, buff, sizeof(buff)-1);
        printf("read %d bytes\n", nread);
        buff[nread] = 0;
        puts(buff);
        poll(&from_engine_fd, 1, 5);
    }
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
        default:
            break;
    }
}

void stop_engine()
{
    tell_engine("stop");
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
