#
# For details on how this Makefile works see the paper "Recursive Make Considered Harmful"
# by Peter Miller (http://aegis.sourceforge.net/auug97.pdf)
#

MODULE = LibAmqp/Transport

# SRC += $(filter-out $(wildcard $(MODULE)/*Stubb.c),$(wildcard $(MODULE)/*.c)) \
#    $(wildcard $(MODULE)/LowLevel/*.c)

SRC += $(wildcard $(MODULE)/*.c) \
    $(wildcard $(MODULE)/Connection/*.c) \
    $(wildcard $(MODULE)/Frame/*.c) \
    $(wildcard $(MODULE)/LowLevel/*.c)

TEST_SRC += $(wildcard  \
    $(MODULE)/*Test.cpp) \
    $(wildcard $(MODULE)/*TestSupport.cpp) \
    $(wildcard $(MODULE)/Frame/*.cpp) \
    $(wildcard $(MODULE)/LowLevel/*.cpp) \
    $(wildcard $(MODULE)/Connection/*.cpp) \
    $(wildcard $(MODULE)/DummyBroker/*.c)
