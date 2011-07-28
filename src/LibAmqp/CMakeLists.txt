AMQP_MODULE(Buffer)
AMQP_MODULE(Codec)
AMQP_MODULE(Context)
AMQP_MODULE(Memory)

SET(AMQP_SOURCES
  ${Buffer_SOURCES}
  ${Codec_SOURCES}
  ${Context_SOURCES}
  ${Memory_SOURCES}
)

AMQP_MODULE(TestData)

SET(AMQP_TEST_SOURCES
  ${Buffer_TEST_SOURCES}
  ${Codec_TEST_SOURCES}
  ${Context_TEST_SOURCES}
  ${Memory_TEST_SOURCES}
  ${TestData_TEST_SOURCES}
)

# $Id: CMakeLists.txt,v 1.14 2011/03/16 12:33:34 erk Exp $
