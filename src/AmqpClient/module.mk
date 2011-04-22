
MODULE = AmqpClient
INCLUDES += $(MODULE)

CLIENT_LIB := libamqp.a
LIBS += $(CLIENT_LIB)

libs::	$(CLIENT_LIB)

$(CLIENT_LIB): $(patsubst %.o,$(CLIENT_LIB)(%.o), $(OBJ))
	ranlib $@

clean::
	rm -f $(CLIENT_LIB)
