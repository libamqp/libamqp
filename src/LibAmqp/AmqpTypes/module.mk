
MODULE = LibAmqp/AmqpTypes

SRC += $(wildcard $(MODULE)/*.c) \
    $(wildcard $(MODULE)/Decode/*.c)

TEST_SRC += $(wildcard $(MODULE)/*Test.cpp) $(wildcard $(MODULE)/*TestFixture.cpp)

