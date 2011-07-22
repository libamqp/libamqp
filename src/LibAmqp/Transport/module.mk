
MODULE = LibAmqp/Framing

SRC += $(wildcard  $(MODULE)/*.c)
TEST_SRC += $(wildcard  $(MODULE)/*Test.cpp) $(wildcard  $(MODULE)/*TestSupport.cpp)

SRC += $(wildcard  $(MODULE)/Stream/*.c)
TEST_SRC += $(wildcard  $(MODULE)/Stream/*Test.cpp) $(wildcard  $(MODULE)/Stream/*TestSupport.cpp)
