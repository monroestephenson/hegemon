# Locate MySQL
#
# This module defines
# MySQL_LIBRARY, the name of the library to link against
# MySQL_FOUND, if false, do not try to link to MySQL
# MySQL_INCLUDE_DIRS, where to find mysql.h
# MySQL_LIBRARIES, the libraries needed to use MySQL

find_path(MySQL_INCLUDE_DIRS
  NAMES mysql.h
  PATHS
    /usr/local/include
    /usr/include
    /usr/local/mysql/include
    /usr/local/opt/mysql/include
    /opt/homebrew/opt/mysql/include
    /opt/homebrew/opt/mysql-client/include
  PATH_SUFFIXES
    mysql
)

find_library(MySQL_LIBRARY
  NAMES mysqlclient mysql libmysql
  PATHS
    /usr/local/lib
    /usr/lib
    /usr/local/mysql/lib
    /usr/local/opt/mysql/lib
    /opt/homebrew/opt/mysql/lib
    /opt/homebrew/opt/mysql-client/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MySQL
    REQUIRED_VARS MySQL_LIBRARY MySQL_INCLUDE_DIRS
)

if(MySQL_FOUND AND NOT TARGET MySQL::MySQL)
    add_library(MySQL::MySQL UNKNOWN IMPORTED)
    set_target_properties(MySQL::MySQL PROPERTIES
        IMPORTED_LOCATION "${MySQL_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${MySQL_INCLUDE_DIRS}"
    )
endif()

mark_as_advanced(
  MySQL_INCLUDE_DIRS
  MySQL_LIBRARY
) 