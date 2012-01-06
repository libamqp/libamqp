SET(AMQP_SOURCES
  Encode.c
  Encode.h
)

SET(AMQP_TEST_SOURCES
  EncodeBooleanTypeTest.cpp
  EncodeCompoundTest.cpp
  EncodeFixedWidthIntTest.cpp
  EncodeFixedWidthLongTest.cpp
  EncodeFixedWidthTest.cpp
  EncodeFixedWidthUIntTest.cpp
  EncodeFixedWidthULongTest.cpp
  EncodeFramingTypesTest.cpp
  EncodeNullTypeTest.cpp
  EncodeVariableWidthTest.cpp
  EncodeTestSupport.h
)

# $Id: CMakeLists.txt,v 1.14 2011/03/16 12:33:34 erk Exp $
