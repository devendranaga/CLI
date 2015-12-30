
CLI_SERVICE_SRC += service/clisrv.c

CLI_CLIENT_SRC += client/cli_show.c
CLI_CLIENT_SRC += client/clicli.c

CLI_LIB_SRC += libs/eventlib/libev.c

CLI_LIB_NAME = libcliev.a
CLI_CLIENT_NAME = cli_client
CLI_SERVICE_NAME = cli_service

CLI_CC = gcc
CLI_AR = ar
CLI_AR_ARGS = rcv
CLI_CFLAGS = -Wall
CLI_INCL = -I. -Ilibs/eventlib/ -Iclient/

CLI_SERVICE_OBJ = $(patsubst %.c, %.o, ${CLI_SERVICE_SRC})
CLI_CLIENT_OBJ = $(patsubst %.c, %.o, ${CLI_CLIENT_SRC})
CLI_LIB_OBJ = $(patsubst %.c, %.o, ${CLI_LIB_SRC})
CLI_LIBS+= -pthread -lrt -lm

all: $(CLI_LIB_NAME) $(CLI_CLIENT_NAME) $(CLI_SERVICE_NAME)

$(CLI_LIB_NAME): $(CLI_LIB_OBJ)
	${CLI_AR} $(CLI_AR_ARGS) $(CLI_LIB_NAME) $(CLI_LIB_OBJ)

$(CLI_CLIENT_NAME): $(CLI_CLIENT_OBJ)
	${CLI_CC} -g $(CLI_CLIENT_OBJ) -o $(CLI_CLIENT_NAME) $(CLI_LIB_NAME) $(CLI_LIBS)

$(CLI_SERVICE_NAME): $(CLI_SERVICE_OBJ)
	${CLI_CC} -g $(CLI_SERVICE_OBJ) -o $(CLI_SERVICE_NAME) $(CLI_LIB_NAME) $(CLI_LIBS)

%.o: %.c
	${CLI_CC} $(CLI_INCL) $(CLI_CFLAGS) -c -o $@ $<

clean:
	rm -rf $(CLI_LIB_NAME) $(CLI_CLIENT_NAME) $(CLI_SERVICE_NAME) *.o libs/*.o client/*.o service/*.o

