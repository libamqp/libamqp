SET(AMQP_SOURCES
  Memory.c
  Memory.h
  Pool.c
  Pool.h
  PoolInternal.h
)

SET(AMQP_TEST_SOURCES
  MemoryTest.cpp
  MemoryTestSupport.cpp
  MemoryTestSupport.h
  PoolTest.cpp
  PoolTestSupport.cpp
  PoolTestSupport.h
)

# $Id: CMakeLists.txt,v 1.14 2011/03/16 12:33:34 erk Exp $
