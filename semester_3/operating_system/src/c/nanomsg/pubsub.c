/*
 * pubsub.c --
 *
 * https://github.com/dysinger/nanomsg-examples
 *
 * ./pubsub server ipc:///tmp/pubsub.ipc & server=$! && sleep 1
 * ./pubsub client ipc:///tmp/pubsub.ipc client0 & client0=$!
 * ./pubsub client ipc:///tmp/pubsub.ipc client1 & client1=$!
 * ./pubsub client ipc:///tmp/pubsub.ipc client2 & client2=$!
 * sleep 5
 * kill $server $client0 $client1 $client2
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>

#define SERVER "server"
#define CLIENT "client"

char *date ()
{
    time_t raw = time (&raw);
    struct tm *info = localtime (&raw);
    char *text = asctime (info);
    text[strlen(text)-1] = '\0'; // remove '\n'
    return text;
}

int server (const char *url)
{
    int sock = nn_socket (AF_SP, NN_PUB);
    assert (sock >= 0);
    assert (nn_bind (sock, url) >= 0);
    while (1)
    {
        char *d = date();
        int sz_d = strlen(d) + 1; // '\0' too
        printf ("SERVER: PUBLISHING DATE %s\n", d);
        int bytes = nn_send (sock, d, sz_d, 0);
        assert (bytes == sz_d);
        sleep(1);
    }
    return nn_shutdown (sock, 0);
}

int client (const char *url, const char *name)
{
    int sock = nn_socket (AF_SP, NN_SUB);
    assert (sock >= 0);
    // TODO learn more about publishing/subscribe keys
    assert (nn_setsockopt (sock, NN_SUB, NN_SUB_SUBSCRIBE, "", 0) >= 0);
    assert (nn_connect (sock, url) >= 0);
    while (1)
    {
        char *buf = NULL;
        int bytes = nn_recv (sock, &buf, NN_MSG, 0);
        assert (bytes >= 0);
        printf ("CLIENT (%s): RECEIVED %s\n", name, buf);
        nn_freemsg (buf);
    }
    return nn_shutdown (sock, 0);
}

int main (const int argc, const char **argv)
{
    if (argc > 1 && strncmp (SERVER, argv[1], strlen (SERVER)) == 0)
        return server (argv[2]);
    else if (argc > 2 && strncmp (CLIENT, argv[1], strlen (CLIENT)) == 0)
        return client (argv[2], argv[3]);
    else
    {
        fprintf (stderr, "Usage: pubsub %s|%s <URL> <ARG> ...\n",
                 SERVER, CLIENT);
        return 1;
    }
}
