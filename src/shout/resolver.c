/*
** resolver.c
**
** name resolver library
**
*/

#define _GNU_SOURCE 1
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif

#include "thread.h"
#include "resolver.h"
#include "sock.h"

#include <config.h>

/* internal function */

static int _isip(const char *what);

/* internal data */

#ifndef NO_THREAD
static mutex_t _resolver_mutex;
#endif
static int _initialized = 0;

#ifdef HAVE_INET_PTON
static int _isip(const char *what)
{
    union {
        struct in_addr v4addr;
        struct in6_addr v6addr;
    } addr_u;

    if (inet_pton(AF_INET, what, &addr_u.v4addr) <= 0)
        return inet_pton(AF_INET6, what, &addr_u.v6addr) > 0 ? 1 : 0;

    return 1;
}

#else
static int _isip(const char *what)
{
    struct in_addr inp;

    return inet_aton(what, &inp);
}
#endif


#if defined (HAVE_GETNAMEINFO) && defined (HAVE_GETADDRINFO)
char *resolver_getname(const char *ip, char *buff, int len)
{
    struct addrinfo *head = NULL, hints;
    char *ret = NULL;

    if (!_isip(ip)) {
        strncpy(buff, ip, len);
        buff [len-1] = '\0';
        return buff;
    }

    memset (&hints, 0, sizeof (hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    if (getaddrinfo (ip, NULL, &hints, &head))
        return NULL;

    if (head)
    {
        if (getnameinfo(head->ai_addr, head->ai_addrlen, buff, len, NULL, 
                    0, NI_NAMEREQD) == 0)
            ret = buff;

        freeaddrinfo (head);
    }

    return ret;
}


char *resolver_getip(const char *name, char *buff, int len)
{
    struct addrinfo *head, hints;
    char *ret = NULL;

    if (_isip(name)) {
        strncpy(buff, name, len);
        buff [len-1] = '\0';
        return buff;
    }

    memset (&hints, 0, sizeof (hints));
    hints . ai_family = AF_UNSPEC;
    hints . ai_socktype = SOCK_STREAM;
    if (getaddrinfo (name, NULL, &hints, &head))
        return NULL;

    if (head)
    {
        if (getnameinfo(head->ai_addr, head->ai_addrlen, buff, len, NULL, 
                    0, NI_NUMERICHOST) == 0)
            ret = buff;
        freeaddrinfo (head);
    }

    return ret;
}

#else

char *resolver_getname(const char *ip, char *buff, int len)
{
    struct hostent *host;
    char *ret = NULL;
    struct in_addr addr;

    if (! _isip(ip))
    {
        strncpy(buff, ip, len);
        buff [len-1] = '\0';
        return buff;
    }

    thread_mutex_lock(&_resolver_mutex);
    if (inet_aton (ip, &addr)) {
        if ((host=gethostbyaddr (&addr, sizeof (struct in_addr), AF_INET)))
        {
            ret = strncpy (buff, host->h_name, len);
            buff [len-1] = '\0';
        }
    }

    thread_mutex_unlock(&_resolver_mutex);
    return ret;
}

char *resolver_getip(const char *name, char *buff, int len)
{
    struct hostent *host;
    char *ret = NULL;

    if (_isip(name))
    {
        strncpy(buff, name, len);
        buff [len-1] = '\0';
        return buff;
    }
    thread_mutex_lock(&_resolver_mutex);
    host = gethostbyname(name);
    if (host)
    {
        char * temp = inet_ntoa(*(struct in_addr *)host->h_addr);
        ret = strncpy(buff, temp, len);
        buff [len-1] = '\0';
    }
    thread_mutex_unlock(&_resolver_mutex);

    return ret;
}
#endif


void resolver_initialize()
{
    /* initialize the lib if we havne't done so already */

    if (!_initialized)
    {
        _initialized = 1;
        thread_mutex_create (&_resolver_mutex);

        /* keep dns connects (TCP) open */
#ifdef HAVE_SETHOSTENT
        sethostent(1);
#endif
    }
}

void resolver_shutdown(void)
{
    if (_initialized)
    {
        thread_mutex_destroy(&_resolver_mutex);
        _initialized = 0;
#ifdef HAVE_ENDHOSTENT
        endhostent();
#endif
    }
}

