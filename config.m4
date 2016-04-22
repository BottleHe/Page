dnl $Id$
dnl config.m4 for extension page

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(page, for page support,
	Make sure that the comment is aligned:
	[  --with-page             Include page support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(page, whether to enable page support,
	Make sure that the comment is aligned:
	[  --enable-page           Enable page support])

if test "$PHP_PAGE" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-page -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/page.h"  # you most likely want to change this
  dnl if test -r $PHP_PAGE/$SEARCH_FOR; then # path given as parameter
  dnl   PAGE_DIR=$PHP_PAGE
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for page files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       PAGE_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$PAGE_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the page distribution])
  dnl fi

  dnl # --with-page -> add include path
  dnl PHP_ADD_INCLUDE($PAGE_DIR/include)

  dnl # --with-page -> check for lib and symbol presence
  dnl LIBNAME=page # you may want to change this
  dnl LIBSYMBOL=page # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PAGE_DIR/lib, PAGE_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_PAGELIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong page lib version or lib not found])
  dnl ],[
  dnl   -L$PAGE_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(PAGE_SHARED_LIBADD)

  PHP_NEW_EXTENSION(page, page.c, $ext_shared)
fi
