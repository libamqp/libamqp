#
# For details on how this Makefile works see the paper "Recursive Make Considered Harmful"
# by Peter Miller (http://aegis.sourceforge.net/auug97.pdf)
#

MODULES := \
	LibAmqp/TestData \
	LibAmqp \
	LibAmqp/Misc \
	LibAmqp/AmqpTypes \
	LibAmqp/Buffer \
	LibAmqp/Memory \
	LibAmqp/Context \
	LibAmqp/Codec \
	LibAmqp/Thread \
	LibAmqp/Transport \
	LibAmqp/Messaging \
	LibAmqp/Plugin \
	LibAmqp/Plugin/SaslAnonymous \
	LibAmqp/Plugin/SaslPlain \
        DecodeDemo \
        SampleServer \
	../3rd-party/UnitTest++/src  \
	../3rd-party/PaulHsieh/src \
	Test

CC=gcc
LIBS :=
SRC :=
INCLUDES :=
TEST_SRC :=
TEST_HARNESS_SRC :=
TEST_HARNESS_INCLUDES :=
TEST_RUNNER_ARGS := --exclude SlowRunning WorkInProgress
#TEST_RUNNER_ARGS := BasicDecode CodecDecode IntegerDecode NumericDecode VariableTypeDecoder CompoundTypeDecode BadDataDecoder  CodecEncode CompoundEncoding VariableTypesEncoding
#TEST_RUNNER_ARGS := WorkInProgress
TEST_RUNNER_ARGS := Session
TEST_RUNNER_ARGS :=  Frame Connection
JENKINS_BUILD_OPTIONS :=

SAMPLE_SRC :=

.PHONY:	all trace libs tests check check-all debug targets quick coverge
all:	trace libs samples targets tests
ci-build:   scrub all check
targets::

BUILD_OPTIONS += -DLIBAMQP_DISABLE_BUFFER_INLINE
# BUILD_OPTIONS += -DLIBAMQP_DISABLE_MEMORY_POOL
# BUILD_OPTIONS += -DLIBAMQP_DISABLE_FAST_MUTEX
# BUILD_OPTIONS += -DLIBAMQP_DISABLE_TEST_HOOKS
BUILD_OPTIONS += -DLIBAMQP_TRACE_CONNECT_STATE

# Danger Will Robinson!!!
# Only define this if you need to debug repeated frees of the same memory block.
# A side effect of it's use is that freed blocks are leaked so that the location
# of the a prior free can be remembered.
# BUILD_OPTIONS += -DLIBAMQP_DEBUG_DOUBLE_FREE

# Define this if you need to see what pool allocation are not being freed.
BUILD_OPTIONS += -DLIBAMQP_DEBUG_ALLOCATIONS

CPPFLAGS = $(patsubst %,-I%, $(INCLUDES)) $(patsubst %,-I%, $(TEST_HARNESS_INCLUDES)) $(BUILD_OPTIONS) $(JENKINS_BUILD_OPTIONS)
# CFLAGS += -g -O3 -Wall -Werror -std=c99 -pedantic -D_POSIX_C_SOURCE=200112L -fasm
CFLAGS = -g -O0 -Wall -Werror -std=c99 -pedantic -D_POSIX_C_SOURCE=200112L -D_DARWIN_C_SOURCE -fasm $(COVERAGE_CFLAGS)
CXXFLAGS = -g -Wall
LDFLAGS = -lev -lpthread $(COVERAGE_LDFLAGS)

LIBAMQP_LIB := libamqp.a
LIBS += $(LIBAMQP_LIB)

include $(patsubst %,%/module.mk,$(MODULES))

OBJ = $(patsubst %.c,%.o,$(SRC))
SAMPLE_OBJ = $(patsubst %.c,%.o,$(SAMPLE_SRC))
TEST_OBJ = $(patsubst %.cpp,%.o,$(filter %.cpp,$(TEST_SRC))) \
	$(patsubst %.c,%.o,$(filter %.c,$(TEST_SRC)))
DEP_SRC += $(SRC) $(TEST_SRC) $(SAMPLE_SRC)

libs::	$(LIBAMQP_LIB)
$(LIBAMQP_LIB): $(patsubst %.o,$(LIBAMQP_LIB)(%.o), $(OBJ))
	@ranlib $@

tests::	test-runner

debug:	all
	@echo "set args $(TEST_RUNNER_ARGS)" >> gdb-cmd-args
	gdb -x gdb-cmd-args ./test-runner
	@rm -f gdb-cmd-args
	
check::	all
	./test-runner $(TEST_RUNNER_ARGS)

check-all:	all
	./test-runner 

quick:
	time make AR=./ar.serialize --jobs 3 check

test-runner:	$(TEST_OBJ) $(OBJ) $(LIBS)
	g++ -L. $(TEST_OBJ) $(patsubst lib%.a,-l%,$(LIBS)) $(LDFLAGS) -o $@

coverage:
	find . \( -name \*.[aoi] -or -name \*.orig \) -print | xargs rm -f
	find . \( -name \*.gcov -o -name \*.gcda -o -name \*.gcno \) -print | xargs rm -f
	make COVERAGE_CFLAGS+="-fprofile-arcs -ftest-coverage" COVERAGE_LDFLAGS+="-lgcov" test-runner 
	./test-runner
	./run-gcov.sh

#$(OBJ) $(TEST_OBJ): Makefile

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

samples: $(SAMPLE_OBJ)

.PHONY:	generate
generate:
	cd ../scripts/; ./generate

