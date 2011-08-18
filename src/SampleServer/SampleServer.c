//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include <ev.h>
#include "libamqp.h"

// TODO - libamqp.h should include all the API declarations
#include "Context/Context.h"
#include "Transport/Transport.h"
#include "Transport/Listener.h"


static int write_all(int fd, const char *buffer, size_t n)
{
    int count = n;
    int written;
    while ((written = write(fd, buffer, count)) >= 0 || errno == EINTR)
    {
        buffer += written;
        count -= written;
	if (count == 0)
	{
	    return n;
	}
    }
    return written;
}

static int new_connection(amqp_io_event_watcher_t *me, amqp_event_loop_t *loop, int fd, struct sockaddr_storage *client_address, socklen_t adress_size)
{
    char buffer[128];
    int n;

    set_socket_to_blocking(fd);

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
    static amqp_event_fn_list_t handlers = { new_connection };
    struct ev_loop *loop = ev_default_loop(0);
    amqp_context_t *context = amqp_create_context();
    amqp_io_event_watcher_t *accept_watcher = amqp_listener_initialize(context, loop, port_number);

    accept_watcher->fns = &handlers;

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
