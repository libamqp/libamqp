#
# For details on how this Malkefile works see the paper "Recursive Make Considered Harmful"
# by Peter Miller (http://aegis.sourceforge.net/auug97.pdf)
#

MODULES := \
	LibAmqp \
	LibAmqp/Memory \
	LibAmqp/Buffer \
	LibAmqp/Context \
	LibAmqp/TestData \
	LibAmqp/Codec \
	LibAmqp/Framing \
        DecodeDemo \
	../3rd-party/UnitTest++/src  \
	Test

OLD_MODULES := \
	AmqpClient/Frame \
	AmqpClient/SaslFrame \
	AmqpClient/Network \
	AmqpClient/Stream \
	DecodeDemo

CC=gcc
LIBS :=
SRC :=
INCLUDES :=
TEST_SRC :=
TEST_HARNESS_SRC :=
TEST_HARNESS_INCLUDES :=

.PHONY:	all trace libs tests check debug targets
all:	trace libs targets tests
targets::

CPPFLAGS = $(patsubst %,-I%, $(INCLUDES)) $(patsubst %,-I%, $(TEST_HARNESS_INCLUDES))
CPPFLAGS +=  -DDISABLE_MEMORY_POOL
# CPPFLAGS +=  -DTRACE_ALLOCATIONS
# CFLAGS = -g -O3 -Wall -Werror -std=c99 -pedantic
CFLAGS = -g -O0 -Wall -Werror -std=c99 -pedantic -D_POSIX_C_SOURCE=200112L 
CXXFLAGS = -g -Wall

LIBAMQP_LIB := libamqp.a
LIBS += $(LIBAMQP_LIB)

include $(patsubst %,%/module.mk,$(MODULES))

OBJ = $(patsubst %.c,%.o,$(SRC))
TEST_OBJ = $(patsubst %.cpp,%.o,$(TEST_SRC))
DEP_SRC += $(TEST_SRC)

libs::	$(LIBAMQP_LIB)
$(LIBAMQP_LIB): $(patsubst %.o,$(LIBAMQP_LIB)(%.o), $(OBJ))
	@ranlib $@

tests::	TestRunner

debug:	all
	gdb ./TestRunner
	
check::	all
	./TestRunner
	
TestRunner:	$(TEST_OBJ) $(OBJ) $(LIBS)
	g++ -L. $(TEST_OBJ) $(patsubst lib%.a,-l%,$(LIBS)) -o $@

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

DEP = $(OBJ:.o=.d) $(TEST_OBJ:.o=.d) \
	$(patsubst %.cpp,%.d,$(filter %.cpp,$(DEP_SRC))) \
	$(patsubst %.c,%.d,$(filter %.c,$(DEP_SRC)))

include $(DEP)

%.d: %.cpp
	@./depend.sh `dirname $*` $(CPPFLAGS) $*.cpp > $@

%.d: %.c
	@./depend.sh `dirname $*` $(CPPFLAGS) $*.c > $@

CPP_OUT := \
	$(OBJ:.o=.i) \
	$(TEST_OBJ:.o=.i) \
	$(patsubst %.cpp,%.i,$(filter %.cpp,$(DEP_SRC))) \
	$(patsubst %.c,%.i,$(filter %.c,$(DEP_SRC)))
.PHONY: cpp-out
cpp-out:	$(CPP_OUT)

%.i: %.cpp
	$(CC) -E $(CPPFLAGS) $*.cpp -o $@

%.i: %.c
	$(CC) -E $(CPPFLAGS) $*.c -o $@

ASM_OUT := \
	$(OBJ:.o=.s) \
	$(TEST_OBJ:.o=.s) \
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
	rm -f TestRunner

scrub::
	$(MAKE) -f Makefile.util scrub

