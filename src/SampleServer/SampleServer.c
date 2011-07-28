#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "libamqp.h"

// TODO - libamqp.h should include all the API declarations
#include "Context/Context.h"

extern int setup_listener(int portno);

int main(int argc, char *argv[])
{
    not_implemented(sample_server);
}
