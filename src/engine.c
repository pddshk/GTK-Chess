#include "engine.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

void tell_engine(const char* command)
{
    write(to_engine, command, strlen(command));
}

void init_engine()
{
    puts("Init engine...");
    if (engine_status)
        stop_engine();
    //skip_output();
    char buff[4096];
    buff[sizeof(buff)-1] = 0;
    int nread;
    //read(from_engine, buff, sizeof(buff));
    tell_engine("uci\n");
    do {
        puts("reading from engine");
        nread = read(from_engine, buff, sizeof(buff));
        buff[nread]= 0;
        printf("%d bytes read\n", nread);
        puts(buff);
    } while(nread == sizeof(buff)-1);
    //skip_output();
    puts("isready");
    tell_engine("isready\n");
    do {
        puts("reading from engine");
        nread = read(from_engine, buff, sizeof(buff));
        buff[nread]= 0;
        printf("%d bytes read\n", nread);
        puts(buff);
    } while(nread == sizeof(buff)-1);
    puts("Init engine done!");
}

void skip_output()
{
    if (engine_status)
        printf("Warning! Skipping engine output when it is calculating");
    char buff[1024];
    int nread;
    puts("Skipping");
    if ((fcntl(from_engine, F_GETFL) & O_WRONLY) == 0)
        do {
            nread = read(from_engine, buff, sizeof(buff)-1);
            printf("read %d bytes\n", nread);
            buff[nread] = 0;
            puts(buff);
        } while(nread  == sizeof(buff)-1);
}

void start_stop(GtkBox* engine_response)
{
    if (!engine_status) {
        char *str = "go\n";
        g_timeout_add(500, G_SOURCE_FUNC(update_variants), engine_response);
        tell_engine(str);
        engine_status = 1;
    } else {
        stop_engine();
    }
}

void stop_engine()
{
    tell_engine("stop");
    engine_status = 0;
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
