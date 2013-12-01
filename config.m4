dnl
dnl $Id: config.m4 242949 2007-09-26 15:44:16Z cvs2svn $
dnl

PHP_ARG_ENABLE(ws, for websocket support,
[  --enable-ws        Enable websocket support])

if test "$PHP_WS" != "no"; then
  LIBNAME=websock
  LIBSYMBOL=libwebsock_init
  PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $WS_DIR/lib, WS_SHARED_LIBADD)
    AC_DEFINE(HAVE_WSLIB,1,[ ])
  ],[
    AC_MSG_ERROR([must have libwebsock installed first])
  ],[
    -L$WS_DIR/lib
  ])
  PHP_SUBST(WS_SHARED_LIBADD)
  PHP_NEW_EXTENSION(ws, ws.c php_ws.c, $ext_shared)
fi
