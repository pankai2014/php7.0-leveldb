dnl $Id$
dnl config.m4 for extension leveldb

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(leveldb, for leveldb support,
Make sure that the comment is aligned:
[  --with-leveldb             Include leveldb support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(leveldb, whether to enable leveldb support,
dnl Make sure that the comment is aligned:
dnl [  --enable-leveldb           Enable leveldb support])

if test "$PHP_LEVELDB" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-leveldb -> check with-path
  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="/include/leveldb/db.h"
  if test -r $PHP_LEVELDB/$SEARCH_FOR; then # path given as parameter
    LEVELDB_DIR=$PHP_LEVELDB
  else # search default path list
    AC_MSG_CHECKING([for leveldb files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        LEVELDB_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  
  if test -z "$LEVELDB_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the leveldb distribution])
  fi

  dnl # --with-leveldb -> add include path
  PHP_ADD_INCLUDE($LEVELDB_DIR/include)

  dnl # --with-leveldb -> check for lib and symbol presence
  LIBNAME=leveldb
  LIBSYMBOL=leveldb

  PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $LEVELDB_DIR/$PHP_LIBDIR, LEVELDB_SHARED_LIBADD)

 dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
 dnl [
 dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $LEVELDB_DIR/$PHP_LIBDIR, LEVELDB_SHARED_LIBADD)
 dnl   AC_DEFINE(HAVE_LEVELDBLIB,1,[ ])
 dnl ],[
 dnl   AC_MSG_ERROR([wrong leveldb lib version or lib not found])
 dnl ],[
 dnl   -L$LEVELDB_DIR/$PHP_LIBDIR -lm
 dnl ])
  
 dnl PHP_SUBST(LEVELDB_SHARED_LIBADD)

 dnl PHP_NEW_EXTENSION(leveldb, leveldb.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)

 PHP_REQUIRE_CXX()

 PHP_ADD_LIBRARY(stdc++, 1, EXTRA_LDFLAGS)

 PHP_SUBST(LEVELDB_SHARED_LIBADD)
 PHP_NEW_EXTENSION(leveldb, leveldb.cpp iterator.cpp, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
