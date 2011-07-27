#
# For details on how this Makefile works see the paper "Recursive Make Considered Harmful"
# by Peter Miller (http://aegis.sourceforge.net/auug97.pdf)
#

MAKEVAR = LibAmqp/Parent/Module

SRC += $(filter-out $(wildcard $(MAKEVAR)/*Stubb.c),$(wildcard $(MAKEVAR)/*.c))
TEST_SRC += $(wildcard  $(MAKEVAR)/*Test.cpp) $(wildcard $(MAKEVAR)/*TestSupport.cpp $(MAKEVAR)/*Stubb.c)
