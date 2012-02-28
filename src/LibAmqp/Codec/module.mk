
MODULE = LibAmqp/Codec

SRC += $(wildcard $(MODULE)/*.c)
TEST_SRC += $(wildcard $(MODULE)/*Test.cpp) $(wildcard $(MODULE)/*TestSupport.cpp) $(wildcard  $(MODULE)/*TestFixture.cpp)

SRC += $(wildcard  $(MODULE)/Type/*.c)
TEST_SRC += $(wildcard $(MODULE)/Type/*Test.cpp) $(wildcard $(MODULE)/Type/*TestSupport.cpp) $(wildcard $(MODULE)/Type/*TestFixture.cpp)

SRC += $(wildcard  $(MODULE)/Encode/*.c)
TEST_SRC += $(wildcard $(MODULE)/Encode/*Test.cpp) $(wildcard $(MODULE)/Encode/*TestSupport.cpp) $(wildcard $(MODULE)/Encode/*TestFixture.cpp)

SRC += $(wildcard  $(MODULE)/Decode/*.c)
TEST_SRC += $(wildcard $(MODULE)/Decode/*Test.cpp) $(wildcard $(MODULE)/Decode/*TestSupport.cpp) $(wildcard $(MODULE)/Decode/*TestFixture.cpp)

