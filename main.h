#include "common.h"
#include <pthread.h>
#include "./TCP/TCP.h"
#include "./Tools/Tools.h"
#include "./ConfigManager/ConfigManager.h"
#include "./GameEngine/GameEngine.h"

void *pthread_func(void *args);

typedef struct {
    int clnt_sock;
    struct sockaddr_in clnt_addr;
    int default_delay;
    int competition_delay;
} Thread_Args;