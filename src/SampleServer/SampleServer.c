//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include <ev.h>
#include "libamqp.h"

#include "Misc/IO.h"
#include "Context/Context.h"
#include "Transport/LowLevel/Listener.h"

static int new_connection_handler(amqp_io_event_watcher_t *me, amqp_event_loop_t *loop, int fd, struct sockaddr_storage *client_address, socklen_t address_size, amqp_accept_handler_arguments_t *arguments)
{
    char buffer[128];
    int n;

    amqp_set_socket_to_blocking(fd);

    while ((n = read(fd, buffer, sizeof(buffer) -1)) > 0)
    {
	if (write_all(fd, buffer, n) == -1)
	    break;
    }
    close(fd);
    return 0;
}

void run(int port_number)
{
    struct ev_loop *loop = ev_default_loop(0);
    amqp_context_t *context = amqp_create_context();
    
    amqp_io_event_watcher_t *accept_watcher = amqp_listener_initialize(context, loop, port_number, new_connection_handler, 0);

    ev_run(loop, 0);

    amqp_listener_destroy(context, accept_watcher);
    amqp_context_destroy(context);
}

int main(int argc, char *argv[])
{
    int port_number = 0;
    if (argc > 1)
    {
        port_number = atoi(argv[1]);
    }
    run(port_number > 0 ? port_number : AMQP_DEFAULT_PORT);
    return 0;
}
