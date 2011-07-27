#
# For details on how this Makefile works see the paper "Recursive Make Considered Harmful"
# by Peter Miller (http://aegis.sourceforge.net/auug97.pdf)
#

MODULE = LibAmqp
INCLUDES += $(MODULE)

SRC += $(wildcard $(MODULE)/*.c)



