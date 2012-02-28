AMQP_MODULE(Decode)
AMQP_MODULE(Encode)
AMQP_MODULE(Type)

SET(AMQP_SOURCES
  Codec.h
  Convert.c
  Convert.h
  ${Decode_SOURCES}
  ${Encode_SOURCES}
  ${Type_SOURCES}
)

SET(AMQP_TEST_SOURCES
  CodecTestFixture.cpp
  CodecTestFixture.h
)

# $Id: CMakeLists.txt,v 1.14 2011/03/16 12:33:34 erk Exp $
