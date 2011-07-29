SET(AMQP_SOURCES
  EncodingMetaData.c
  EncodingMetaData.h
  Type.c
  Type.h
  TypeEncodeMetaData.c
  TypeEncodeMetaData.h
  TypeMethods.c
  TypeMethods.h
  TypePrint.c
  TypePrint.h
  TypeValidate.h
)

SET(AMQP_TEST_SOURCES
  EncodingMetaDataTest.cpp
  TypePrintTest.cpp
  TypeTest.cpp
  TypeValidateTest.cpp
)

# $Id: CMakeLists.txt,v 1.14 2011/03/16 12:33:34 erk Exp $
