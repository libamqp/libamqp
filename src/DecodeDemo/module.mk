
MODULE = DecodeDemo
INCLUDES += $(MODULE)


DEMO_SRC := $(wildcard $(MODULE)/*.c)
DEMO_OBJ := $(DEMO_SRC:.c=.o)

DEP_SRC += $(DEMO_SRC)


targets::	decode-demo

decode-demo:	$(DEMO_OBJ) $(CLIENT_LIB)
	$(CC) $(DEMO_OBJ) -L. $(patsubst lib%.a,-l%,$(CLIENT_LIB)) -o $@


