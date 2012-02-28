
MODULE = Test
TEST_HARNESS_INCLUDES += $(MODULE)
TEST_SRC += $(wildcard $(MODULE)/*.cpp) \
        $(wildcard $(MODULE)/*.c)