#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
#include <unistd.h>
#include <stdlib.h>
#include "gui.h"
#include "engine_manager.h"

#include "globals.h"

int main(int argc, char** argv)
{
	int from_engine_manager_fd[2], to_engine_manager_fd[2];
	pipe(from_engine_manager_fd);
	pipe(to_engine_manager_fd);
	pid_t pid = -1;
	for (int i = 0; pid == -1 && i < 10; i++) pid = fork();
	switch (pid) {
		case 0: // engine manager process
			close(to_engine_manager_fd[1]); // close to engine manager write end
			close(from_engine_manager_fd[0]); // close from engine manager read end
			engine_manager_main(to_engine_manager_fd[0], from_engine_manager_fd[1]);
			return EXIT_SUCCESS;
		case -1:
			perror("Critical: cannot spawn process for engine manager. Try relaunching application\n");
		default:
			close(from_engine_manager_fd[1]); // close to engine read end
			close(to_engine_manager_fd[0]); // close from engine write end
			int from_engine_manager = from_engine_manager_fd[0],
				to_engine_manager = to_engine_manager_fd[1];
			gtk_init(&argc, &argv);
			init_elements();
			//g_unix_fd_add(from_engine_manager, G_IO_IN, GUnixFDSourceFunc function, gpointer user_data);
			gtk_main();
			break;
	}
    return EXIT_SUCCESS;
}
