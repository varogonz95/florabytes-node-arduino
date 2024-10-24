#define PGOTCHI_USE_LOCALHOST 

#ifdef PGOTCHI_USE_LOCALHOST
    #define PGOTCHI_PROTOCOL "https"
    #define PGOTCHI_HOST "192.168.100.93"
    #define PGOTCHI_PORT "7087"
#else
    #define PGOTCHI_PROTOCOL "https"
    #define PGOTCHI_HOST "pgotchi-dev-east-service.azurewebsites.net"
    #define PGOTCHI_PORT "443"
#endif
#define PGOTCHI_API_URL PGOTCHI_PROTOCOL "://" PGOTCHI_HOST ":" PGOTCHI_PORT "/"
