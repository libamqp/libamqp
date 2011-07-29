SET(AMQP_SOURCES
  Decode.c
  Decode.h
)

SET(AMQP_TEST_SOURCES
  BasicDecodeTest.cpp
  DecodeBadDataTest.cpp
  DecodeBooleanTest.cpp
  DecodeCompoundTest.cpp
  DecodeIntegerTest.cpp
  DecodeNullTest.cpp
  DecodeNumberTest.cpp
  DecodeStringTest.cpp
  DecodeSymbolTest.cpp
  DecodeTestSupport.h
)

# $Id: CMakeLists.txt,v 1.14 2011/03/16 12:33:34 erk Exp $
