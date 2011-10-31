#
# For details on how this Makefile works see the paper "Recursive Make Considered Harmful"
# by Peter Miller (http://aegis.sourceforge.net/auug97.pdf)
#

MODULE = LibAmqp/Messaging

SRC += $(wildcard $(MODULE)/*.c)

TEST_SRC += $(wildcard  $(MODULE)/*Test.cpp) \
    $(wildcard $(MODULE)/*TestSupport.cpp \
    $(MODULE)/*Stubb.c)

