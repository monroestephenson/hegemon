# Find libpqxx
#
# This module defines
# PQXX_LIBRARY, the name of the library to link against
# PQXX_FOUND, if false, do not try to link against libpqxx
# PQXX_INCLUDE_DIR, where to find pqxx headers
#

find_path(PQXX_INCLUDE_DIR
  NAMES pqxx/pqxx
  PATHS
    /usr/include
    /usr/local/include
    /opt/homebrew/include
)

find_library(PQXX_LIBRARY
  NAMES pqxx
  PATHS
    /usr/lib
    /usr/local/lib
    /opt/homebrew/lib
)

if(PQXX_INCLUDE_DIR AND PQXX_LIBRARY)
  set(PQXX_FOUND TRUE)
  set(PQXX_LIBRARIES ${PQXX_LIBRARY})
else()
  set(PQXX_FOUND FALSE)
endif()

mark_as_advanced(
  PQXX_INCLUDE_DIR
  PQXX_LIBRARY
) 