
CLI_SERVICE_SRC += service/clisrv.c
CLI_SERVICE_SRC += service/auth.c
CLI_SERVICE_SRC += util.c

CLI_CLIENT_SRC += client/cli_show.c
CLI_CLIENT_SRC += client/cli_help.c
CLI_CLIENT_SRC += client/clicli.c
CLI_CLIENT_SRC += util.c

CLI_LIBEV_SRC += libs/eventlib/libev.c
CLI_LIBNET_SRC += libs/netlib/libnet.c
CLI_LIBUTIL_SRC += libs/utils/utils.c

CLI_TEST_LIBEV += tests/server_tests.c

LIBEVCLI_LIB_NAME = libcliev.a
LIBNETCLI_LIB_NAME = libclinet.a
LIBUTIL_LIB_NAME = libcliutil.a

CLI_CLIENT_NAME = cli_client
CLI_SERVICE_NAME = cli_service
CLI_LIBEV_TESTER_NAME = cli_libev_test

CLI_CC = gcc
CLI_AR = ar
CLI_AR_ARGS = rcv
CLI_CFLAGS = -Wall -Werror -Wextra -Wshadow -Wno-unused-parameter -fprofile-arcs -ftest-coverage -coverage
CLI_INCL = -I. -Ilibs/eventlib/ -Ilibs/netlib/ -Iclient/ -Ilibs/utils/

CLI_SERVICE_OBJ = $(patsubst %.c, %.o, ${CLI_SERVICE_SRC})
CLI_CLIENT_OBJ = $(patsubst %.c, %.o, ${CLI_CLIENT_SRC})
CLI_LIBEV_OBJ = $(patsubst %.c, %.o, ${CLI_LIBEV_SRC})
CLI_LIBNET_OBJ = $(patsubst %.c, %.o, ${CLI_LIBNET_SRC})
CLI_LIBUTIL_OBJ = $(patsubst %.c, %.o, ${CLI_LIBUTIL_SRC})
CLI_LIBEV_TESTER_OBJ = $(patsubst %.c, %.o, ${CLI_TEST_LIBEV})
CLI_LIBS+= -pthread -lrt -lm -pg -lgcov -coverage

all: $(LIBEVCLI_LIB_NAME) $(LIBNETCLI_LIB_NAME) $(LIBUTIL_LIB_NAME) $(CLI_CLIENT_NAME) $(CLI_SERVICE_NAME) $(CLI_LIBEV_TESTER_NAME)

$(LIBEVCLI_LIB_NAME): $(CLI_LIBEV_OBJ)
	${CLI_AR} $(CLI_AR_ARGS) $(LIBEVCLI_LIB_NAME) $(CLI_LIBEV_OBJ)

$(LIBNETCLI_LIB_NAME): $(CLI_LIBNET_OBJ)
	${CLI_AR} $(CLI_AR_ARGS) $(LIBNETCLI_LIB_NAME) $(CLI_LIBNET_OBJ)

$(LIBUTIL_LIB_NAME): $(CLI_LIBUTIL_OBJ)
	${CLI_AR} $(CLI_AR_ARGS) $(LIBUTIL_LIB_NAME) $(CLI_LIBUTIL_OBJ)

$(CLI_CLIENT_NAME): $(CLI_CLIENT_OBJ)
	${CLI_CC} -g $(CLI_CLIENT_OBJ) -o $(CLI_CLIENT_NAME) $(LIBEVCLI_LIB_NAME) $(CLI_LIBS)

$(CLI_SERVICE_NAME): $(CLI_SERVICE_OBJ)
	${CLI_CC} -g $(CLI_SERVICE_OBJ) -o $(CLI_SERVICE_NAME) $(LIBEVCLI_LIB_NAME) $(LIBNETCLI_LIB_NAME) $(CLI_LIBS)

$(CLI_LIBEV_TESTER_NAME): $(CLI_LIBEV_TESTER_OBJ)
	${CLI_CC} -g $(CLI_LIBEV_TESTER_OBJ) -o $(CLI_LIBEV_TESTER_NAME) $(LIBEVCLI_LIB_NAME) $(LIBNETCLI_LIB_NAME) $(LIBUTIL_LIB_NAME) $(CLI_LIBS)

%.o: %.c
	${CLI_CC} $(CLI_INCL) $(CLI_CFLAGS) -c -o $@ $<

clean:
	rm -rf $(LIBEVCLI_LIB_NAME) $(LIBNETCLI_LIB_NAME) $(CLI_CLIENT_NAME) $(CLI_SERVICE_NAME) $(CLI_LIBEV_TESTER_NAME) *.o libs/eventlib/*.o  libs/netlib/*.o libs/utils/*.o client/*.o service/*.o

