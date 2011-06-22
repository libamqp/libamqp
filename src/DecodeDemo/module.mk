#
# For details on how this Malkefile works see the paper "Recursive Make Considered Harmful"
# by Peter Miller (http://aegis.sourceforge.net/auug97.pdf)
#

MODULE = DecodeDemo
INCLUDES += $(MODULE)


DEMO_SRC := $(wildcard $(MODULE)/*.c)
DEMO_OBJ := $(DEMO_SRC:.c=.o)

DEP_SRC += $(DEMO_SRC)


targets::	decode-demo

decode-demo:	$(DEMO_OBJ) $(LIBAMQP_LIB)
	$(CC) $(DEMO_OBJ) -L. $(patsubst lib%.a,-l%,$(LIBAMQP_LIB)) -o $@


