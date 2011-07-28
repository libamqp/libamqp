#
# For details on how this Makefile works see the paper "Recursive Make Considered Harmful"
# by Peter Miller (http://aegis.sourceforge.net/auug97.pdf)
#

MODULE = SampleServer

SAMPLE_SERVER_SRC := $(wildcard $(MODULE)/*.c)
SAMPLE_SERVER_OBJ := $(SAMPLE_SERVER_SRC:.c=.o)

DEP_SRC += $(SAMPLE_SERVER_SRC)

targets::	sample-server

sample-server:	$(SAMPLE_SERVER_OBJ) $(LIBAMQP_LIB)
	$(CC) $(SAMPLE_SERVER_OBJ) -L. $(patsubst lib%.a,-l%,$(LIBAMQP_LIB)) $(LDFLAGS) -o $@


