//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include <ev.h>
#include "libamqp.h"

// TODO - libamqp.h should include all the API declarations
#include "Context/Context.h"
#include "Transport/Transport.h"
#include "Transport/Socket.h"

void run(int port_number)
{
    amqp_context_t *context = amqp_create_context();

    struct ev_loop *loop = ev_default_loop(0);

    amqp_transport_initialize(context, loop);
    amqp_setup_listener(context, port_number);

    ev_run(loop, 0);

    // Might never be reached
    amqp_transport_cleanup(context);
    amqp_destroy_context(context);
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
