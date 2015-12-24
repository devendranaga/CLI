
CLI_SERVICE_SRC +=

CLI_CLIENT_SRC += client/cli_show.c
CLI_CLIENT_SRC += client/clicli.c

CLI_LIB_SRC += libs/libev.c

CLI_LIB_NAME = libcliev.a
CLI_CLIENT_NAME = CLIC
CLI_SERVICE_NAME = CLIS

CLI_CC = gcc
CLI_AR = ar
CLI_AR_ARGS = rcv
CLI_CFLAGS = -Wall
CLI_INCL = -I. -Ilibs/ -Iclient/

CLI_CLIENT_OBJ = $(patsubst %.c, %.o, ${CLI_CLIENT_SRC})
CLI_LIB_OBJ = $(patsubst %.c, %.o, ${CLI_LIB_SRC})
CLI_LIBS+= -pthread -lrt -lm

all: $(CLI_LIB_NAME) $(CLI_CLIENT_NAME) $(CLI_SERVICE_NAME)

$(CLI_LIB_NAME): $(CLI_LIB_OBJ)
	${CLI_AR} $(CLI_AR_ARGS) $(CLI_LIB_NAME) $(CLI_LIB_OBJ)

$(CLI_CLIENT_NAME): $(CLI_CLIENT_OBJ)
	${CLI_CC} -g $(CLI_CLIENT_OBJ) -o $(CLI_CLIENT_NAME) $(CLI_LIB_NAME) $(CLI_LIBS)

%.o: %.c
	${CLI_CC} $(CLI_INCL) $(CLI_CFLAGS) -c -o $@ $<

clean:
	rm -rf $(CLI_LIB_NAME) $(CLI_CLIENT_NAME) *.o libs/*.o client/*.o service/*.o

