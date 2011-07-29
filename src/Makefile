#
# For details on how this Makefile works see the paper "Recursive Make Considered Harmful"
# by Peter Miller (http://aegis.sourceforge.net/auug97.pdf)
#

MODULES := \
	LibAmqp/TestData \
	LibAmqp \
	LibAmqp/Memory \
	LibAmqp/Buffer \
	LibAmqp/Context \
	LibAmqp/Codec \
	LibAmqp/Thread \
	LibAmqp/Transport \
        DecodeDemo \
        SampleServer \
	../3rd-party/UnitTest++/src  \
	Test


CC=gcc
LIBS :=
SRC :=
INCLUDES :=
TEST_SRC :=
TEST_HARNESS_SRC :=
TEST_HARNESS_INCLUDES :=
TEST_RUNNER_ARGS :=

.PHONY:	all trace libs tests check debug targets
all:	trace libs targets tests
ci-build:   scrub all check
targets::

CPPFLAGS = $(patsubst %,-I%, $(INCLUDES)) $(patsubst %,-I%, $(TEST_HARNESS_INCLUDES))
# CPPFLAGS +=  -DDISABLE_MEMORY_POOL
# CPPFLAGS +=  -DTRACE_ALLOCATIONS
# CFLAGS = -g -O3 -Wall -Werror -std=c99 -pedantic
CFLAGS = -g -O0 -Wall -Werror -std=c99 -pedantic -D_POSIX_C_SOURCE=200112L 
CXXFLAGS = -g -Wall
LDFLAGS = -lev

LIBAMQP_LIB := libamqp.a
LIBS += $(LIBAMQP_LIB)

include $(patsubst %,%/module.mk,$(MODULES))

OBJ = $(patsubst %.c,%.o,$(SRC))
TEST_OBJ = $(patsubst %.cpp,%.o,$(filter %.cpp,$(TEST_SRC))) \
	$(patsubst %.c,%.o,$(filter %.c,$(TEST_SRC)))
DEP_SRC += $(SRC) $(TEST_SRC)

libs::	$(LIBAMQP_LIB)
$(LIBAMQP_LIB): $(patsubst %.o,$(LIBAMQP_LIB)(%.o), $(OBJ))
	@ranlib $@

tests::	test-runner

debug:	all
	gdb ./test-runner
	
check::	all
	./test-runner $(TEST_RUNNER_ARGS)
	
test-runner:	$(TEST_OBJ) $(OBJ) $(LIBS)
	g++ -L. $(TEST_OBJ) $(patsubst lib%.a,-l%,$(LIBS)) $(LDFLAGS) -o $@

$(OBJ) $(TEST_OBJ): Makefile

print:
	@echo TEST_HARNESS_INCLUDES=$(TEST_HARNESS_INCLUDES)
	@echo INCLUDES=$(INCLUDES)
	@echo SRC=$(SRC)
	@echo OBJ=$(OBJ)
	@echo TEST_SRC=$(TEST_SRC)
	@echo TEST_OBJ=$(TEST_OBJ)
	@echo DEP_SRC=$(DEP_SRC)
	@echo DEP=$(DEP)
	@echo LIBS=$(LIBS)
	@echo CFLAGS=$(CFLAGS)
	@echo CXXFLAGS=$(CXXFLAGS)

DEP = 	$(patsubst %.cpp,%.d,$(filter %.cpp,$(DEP_SRC))) \
	$(patsubst %.c,%.d,$(filter %.c,$(DEP_SRC)))

include $(DEP)

%.d: %.cpp
	@./depend.sh `dirname $*` $(CPPFLAGS) $*.cpp > $@

%.d: %.c
	@./depend.sh `dirname $*` $(CPPFLAGS) $*.c > $@

CPP_OUT := \
	$(patsubst %.cpp,%.i,$(filter %.cpp,$(DEP_SRC))) \
	$(patsubst %.c,%.i,$(filter %.c,$(DEP_SRC)))
.PHONY: cpp-out
cpp-out:	$(CPP_OUT)

%.i: %.cpp
	$(CC) -E $(CPPFLAGS) $*.cpp -o $@

%.i: %.c
	$(CC) -E $(CPPFLAGS) $*.c -o $@

ASM_OUT := \
	$(patsubst %.cpp,%.s,$(filter %.cpp,$(DEP_SRC))) \
	$(patsubst %.c,%.s,$(filter %.c,$(DEP_SRC)))
.PHONY: asm-out
asm-out:	$(ASM_OUT)

%.s: %.cpp
	$(CC) -S $(CPPFLAGS) $(CXXFLAGS) $*.cpp -o $@

%.s: %.c
	$(CC) -S $(CPPFLAGS) $(CFLAGS) $*.c -o $@

.PHONY:	clean scrub
clean::
	rm -f $(OBJ)
	rm -f $(TEST_OBJ)
	rm -f $(CPP_OUT)
	rm -f $(ASM_OUT)
	rm -f $(LIBAMQP_LIB)
	rm -f test-runner
	rm -f decode-demo
	rm -f sample-server

scrub::
	$(MAKE) -f Makefile.util scrub

