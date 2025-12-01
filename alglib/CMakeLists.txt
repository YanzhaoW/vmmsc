target_sources(
  vmmdcs
  PUBLIC FILE_SET
         HEADERS
         BASE_DIRS
         ${CMAKE_CURRENT_SOURCE_DIR}
         FILES
         alglibinternal.h
         alglibmisc.h
         ap.h
         integration.h
         interpolation.h
         linalg.h
         optimization.h
         solvers.h
         specialfunctions.h
         stdafx.h
  PRIVATE alglibinternal.cpp
          alglibmisc.cpp
          ap.cpp
          integration.cpp
          interpolation.cpp
          linalg.cpp
          optimization.cpp
          solvers.cpp
          specialfunctions.cpp)
