#
#   Copyright 2011 StormMQ Limited
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
# For details on how this Malkefile works see the paper "Recursive Make Considered Harmful"
# by Peter Miller (http://aegis.sourceforge.net/auug97.pdf)
#

MODULES := \
	AmqpClient/Buffer \
	AmqpClient/Codec \
	AmqpClient/Context \
	AmqpClient/ErrorHandling \
	AmqpClient/Frame \
	AmqpClient/SaslFrame \
	AmqpClient/Network \
	AmqpClient/Memory \
	AmqpClient/Stream \
	AmqpClient/Type \
	AmqpClient/types/binary \
	AmqpClient/types/map \
	AmqpClient/types/string \
	AmqpClient \
	CustomCppUnitLite/Deque \
	CustomCppUnitLite/TestHarness \
	DecodeDemo

CC=gcc
LIBS :=
SRC :=
INCLUDES :=
TEST_SRC :=

.PHONY:	all trace libs tests check debug targets
all:	trace libs tests targets 

CPPFLAGS = -I. $(patsubst %,-I%, $(INCLUDES))
CFLAGS = -g -O3 -Wall -Werror -std=c99 -pedantic
CXXFLAGS = -g -Wall

OBJ = $(patsubst %.c,%.o,$(SRC))
TEST_OBJ = $(patsubst %.cpp,%.o,$(TEST_SRC))

include $(patsubst %,%/module.mk,$(MODULES))

targets::
tests::	test
test:	$(TEST_OBJ) $(OBJ) $(LIBS)
	g++ -L. $(TEST_OBJ) $(patsubst lib%.a,-l%,$(LIBS)) -o $@

debug:	all
	gdb ./test
	
check::	all
	{ trap "" 6; ./test; }
	
print:	
	@echo INCLUDES=$(INCLUDES)
	@echo SRC=$(SRC)
	@echo OBJ=$(OBJ)
	@echo TEST_SRC=$(TEST_SRC)
	@echo TEST_OBJ=$(TEST_OBJ)
	@echo DEP_SRC=$(DEP)
	@echo DEP=$(DEP)
	@echo LIBS=$(LIBS)
	@echo CFLAGS=$(CFLAGS)

DEP = $(OBJ:.o=.d) \
	$(TEST_OBJ:.o=.d) \
	$(patsubst %.cpp,%.d,$(filter %.cpp,$(DEP_SRC))) \
	$(patsubst %.c,%.d,$(filter %.c,$(DEP_SRC))) 

include $(DEP)

%.d: %.cpp
	./depend.sh `dirname $*` $(CPPFLAGS) $*.cpp > $@

%.d: %.c
	./depend.sh `dirname $*` $(CPPFLAGS) $*.c > $@

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

.PHONY:	clean veryclean
clean::
	rm -f $(OBJ) 
	rm -f $(TEST_OBJ) 
	rm -f $(CPP_OUT)
	rm -f $(ASM_OUT)

veryclean::     clean
	find . -name \*.[odsi] -print | xargs rm -f

