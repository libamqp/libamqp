
MODULE = ../3rd-party/UnitTest++/src

TEST_HARNESS_INCLUDES += $(MODULE)
TEST_HARNESS_LIB := libUnitTest++.a
TEST_HARNESS_SRC := \
	$(wildcard $(MODULE)/*.cpp) \
	$(wildcard $(MODULE)/Posix/*.cpp)

DEP_SRC += $(TEST_HARNESS_SRC)
LIBS += $(TEST_HARNESS_LIB)

libs::	$(TEST_HARNESS_LIB)

TEST_OBJ = $(patsubst %.cpp,%.o,$(TEST_SRC))
DEP := $(TEST_OBJ:.o=.d)

$(TEST_HARNESS_LIB): $(patsubst %.cpp,$(TEST_HARNESS_LIB)(%.o),$(TEST_HARNESS_SRC))
	ranlib $@

clean::
	rm -f $(TEST_HARNESS_LIB) $(MODULE)/*.o
