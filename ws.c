/*
  +----------------------------------------------------------------------+
  | WebSocket Extension                                                  |
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <websock/websock.h>
#include "php.h"
#include "php_ws.h"

extern zend_class_entry *ws_ce;



static zend_function_entry ws_methods[] = {
	PHP_ME(ws, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
        PHP_ME(ws, bind, NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ws, run, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static void ws_free_storage(void *object TSRMLS_DC) {
        ws_object *intern = (ws_object *)object;
        zend_hash_destroy(intern->zo.properties);
        FREE_HASHTABLE(intern->zo.properties);
        efree(intern);
}

static zend_object_value ws_object_create(zend_class_entry *class_type TSRMLS_DC) {
        zend_object_value retval;
        ws_object *intern;
        intern = (ws_object *) emalloc(sizeof(ws_object));
        memset(intern, 0, sizeof(ws_object));
        intern->zo.ce = class_type;
        ALLOC_HASHTABLE(intern->zo.properties);
        zend_hash_init(intern->zo.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
#if ZEND_MODULE_API_NO >= 20100525
        object_properties_init(&(intern->zo), class_type);
#else
        zval *tmp;
        zend_hash_copy(intern->zo.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
#endif

        retval.handle = zend_objects_store_put(intern, NULL, ws_free_storage, NULL TSRMLS_CC);
        retval.handlers = (zend_object_handlers *) &ws_object_handlers;
        return retval;
}


PHP_METHOD(ws, __construct) {
	zval *this;
	

	this = getThis();
	ws_object *ws_obj = (ws_object *)zend_object_store_get_object(this TSRMLS_CC);
        ws_obj->ws_ctx = libwebsock_init();

	RETURN_TRUE;
}

PHP_METHOD(ws, run) {
  zval *this;

  this = getThis();

  ws_object *ws_obj = (ws_object *) zend_object_store_get_object(this TSRMLS_CC);
  libwebsock_wait(ws_obj->ws_ctx);
  //never reached
  RETURN_TRUE;
}

PHP_METHOD(ws, bind) {
  zval *this;

  this = getThis();
  char *host;
  char *port;
  int host_len;
  int port_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &host, &host_len, &port, &port_len) == FAILURE) {
    return;
  }

  ws_object *ws_obj = (ws_object *) zend_object_store_get_object(this TSRMLS_CC);
  libwebsock_bind(ws_obj->ws_ctx, host, port);
  RETURN_TRUE;
}

void register_ws_class(TSRMLS_D) {
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "WebSocket", ws_methods);
	ce.create_object = ws_object_create;
	memcpy(&ws_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	ws_object_handlers.clone_obj = NULL;
	ws_ce = zend_register_internal_class(&ce TSRMLS_CC);
}


