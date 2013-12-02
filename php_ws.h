/*
  +----------------------------------------------------------------------+
  | Pusher Extension                                                     |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Payden Sutherland <payden@paydensutherland.com>              |
  +----------------------------------------------------------------------+
*/


#ifndef PHP_WS_H
#define PHP_WS_H 1
#include <websock/websock.h>

#define PHP_WS_VERSION NO_VERSION_YET
#define PHP_WS_EXTNAME "ws"

//PHP Function prototypes
PHP_METHOD(ws, __construct);
PHP_METHOD(ws, bind);
PHP_METHOD(ws, bind_ssl);
PHP_METHOD(ws, run);

PHP_METHOD(ws_client, __construct);
PHP_METHOD(ws_client, sendText);
PHP_METHOD(ws_client, sendBinary);
PHP_METHOD(ws_client, close);

PHP_MINIT_FUNCTION(ws);
PHP_MSHUTDOWN_FUNCTION(ws);
PHP_MINFO_FUNCTION(ws);

//Typedefs

typedef struct {
  zend_object zo;
} ws_message_object;

typedef struct {
  zend_object zo;
  libwebsock_client_state *ws_state;
} ws_client_object;

typedef struct {
	zend_object zo;
        libwebsock_context *ws_ctx;
} ws_object;

//global symbols
static zend_class_entry *ws_ce;
static zend_class_entry *ws_client_ce;
static zend_class_entry *ws_message_ce;
zend_object_handlers ws_object_handlers;
zend_object_handlers ws_client_object_handlers;
zend_object_handlers ws_message_object_handlers;

//function prototypes
void register_ws_class(TSRMLS_D);
void register_ws_client_class(TSRMLS_D);
void register_ws_message_class(TSRMLS_D);

extern zend_module_entry ws_module_entry;

#define phpext_ws_ptr &ws_module_entry

#endif
