# FindPQXX.cmake

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
    /usr/local/include
    /usr/include
    /usr/local/opt/libpqxx/include
)

find_library(PQXX_LIBRARY
  NAMES pqxx libpqxx
  PATHS
    /usr/local/lib
    /usr/lib
    /usr/local/opt/libpqxx/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PQXX
    REQUIRED_VARS PQXX_LIBRARY PQXX_INCLUDE_DIR
)

if(PQXX_FOUND AND NOT TARGET PQXX::PQXX)
    add_library(PQXX::PQXX UNKNOWN IMPORTED)
    set_target_properties(PQXX::PQXX PROPERTIES
        IMPORTED_LOCATION "${PQXX_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${PQXX_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(
  PQXX_INCLUDE_DIR
  PQXX_LIBRARY
) 