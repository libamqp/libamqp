
MODULE = LibAmqp/Buffer

SRC += $(wildcard $(MODULE)/*.c)
TEST_SRC += $(wildcard $(MODULE)/*Test.cpp) $(wildcard $(MODULE)/*TestSupport.cpp)

