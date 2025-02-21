# Locate MySQL client library
#
# This module defines
# MYSQL_LIBRARY, the name of the library to link against
# MYSQL_FOUND, if false, do not try to link against MySQL
# MYSQL_INCLUDE_DIR, where to find mysql.h and other headers
#

find_path(MYSQL_INCLUDE_DIR
  NAMES mysql.h
  PATHS
    /usr/include/mysql
    /usr/local/include/mysql
    /usr/local/opt/mysql-client/include/mysql
    /opt/homebrew/opt/mysql-client/include/mysql
    /usr/local/opt/mysql/include/mysql
    /opt/homebrew/opt/mysql/include/mysql
    /usr/local/Cellar/mysql-client/*/include/mysql
    /usr/local/Cellar/mysql/*/include/mysql
)

find_library(MYSQL_LIBRARY
  NAMES mysqlclient
  PATHS
    /usr/lib
    /usr/lib/mysql
    /usr/local/lib
    /usr/local/lib/mysql
    /usr/local/opt/mysql-client/lib
    /opt/homebrew/opt/mysql-client/lib
    /usr/local/opt/mysql/lib
    /opt/homebrew/opt/mysql/lib
    /usr/local/Cellar/mysql-client/*/lib
    /usr/local/Cellar/mysql/*/lib
)

if(MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
  set(MYSQL_FOUND TRUE)
  set(MYSQL_LIBRARIES ${MYSQL_LIBRARY})
else()
  set(MYSQL_FOUND FALSE)
endif()

mark_as_advanced(
  MYSQL_INCLUDE_DIR
  MYSQL_LIBRARY
) 