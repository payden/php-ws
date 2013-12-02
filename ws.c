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

static int ws_onopen(libwebsock_client_state *state);
static int ws_onclose(libwebsock_client_state *state);
static int ws_onmessage(libwebsock_client_state *state, libwebsock_message *msg);
extern zend_class_entry *ws_ce;
extern zend_class_entry *ws_client_ce;
extern zend_class_entry *ws_message_ce;

static zend_function_entry ws_message_methods[] = {
  {NULL, NULL, NULL}
};

static zend_function_entry ws_client_methods[] = {
  PHP_ME(ws_client, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(ws_client, sendText, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(ws_client, sendBinary, NULL, ZEND_ACC_PUBLIC)
  {NULL, NULL, NULL}
};

static zend_function_entry ws_methods[] = {
	PHP_ME(ws, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
        PHP_ME(ws, bind, NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ws, bind_ssl, NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ws, run, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static void ws_message_free_storage(void *object TSRMLS_DC) {
  ws_message_object *intern = (ws_message_object *) object;
  if (intern->zo.properties) {
    zend_hash_destroy(intern->zo.properties);
    FREE_HASHTABLE(intern->zo.properties);
  }
  efree(intern);
}

static void ws_client_free_storage(void *object TSRMLS_DC) {
  ws_client_object *intern = (ws_client_object *) object;
  if (intern->zo.properties) {
    zend_hash_destroy(intern->zo.properties);
    FREE_HASHTABLE(intern->zo.properties);
  }
  efree(intern);
}

static void ws_free_storage(void *object TSRMLS_DC) {
        ws_object *intern = (ws_object *)object;
        if (intern->zo.properties) {
          zend_hash_destroy(intern->zo.properties);
          FREE_HASHTABLE(intern->zo.properties);
        }
        efree(intern);
}

static zend_object_value ws_message_object_create(zend_class_entry *class_type TSRMLS_DC) {
  zend_object_value retval;
  ws_message_object *intern;
  intern = (ws_message_object *) emalloc(sizeof(ws_message_object));
  memset(intern, 0, sizeof(ws_message_object));
  intern->zo.ce = class_type;
  ALLOC_HASHTABLE(intern->zo.properties);
  zend_hash_init(intern->zo.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
#if ZEND_MODULE_API_NO >= 20100525
  object_properties_init(&(intern->zo), class_type);
#else
  zval *tmp;
  zend_hash_copy(intern->zo.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
#endif
  retval.handle = zend_objects_store_put(intern, NULL, ws_message_free_storage, NULL TSRMLS_CC);
  retval.handlers = (zend_object_handlers *) &ws_message_object_handlers;
  return retval;
}

static zend_object_value ws_client_object_create(zend_class_entry *class_type TSRMLS_DC) {
  zend_object_value retval;
  ws_client_object *intern;
  intern = (ws_client_object *) emalloc(sizeof(ws_client_object));
  memset(intern, 0, sizeof(ws_client_object));
  intern->zo.ce = class_type;
  ALLOC_HASHTABLE(intern->zo.properties);
  zend_hash_init(intern->zo.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
#if ZEND_MODULE_API_NO >= 20100525
  object_properties_init(&(intern->zo), class_type);
#else
  zval *tmp;
  zend_hash_copy(intern->zo.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
#endif
  retval.handle = zend_objects_store_put(intern, NULL, ws_client_free_storage, NULL TSRMLS_CC);
  retval.handlers = (zend_object_handlers *) &ws_client_object_handlers;
  return retval;
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
        object_properties_init(&intern->zo, class_type);
#else
        zval *tmp;
        zend_hash_copy(intern->zo.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
#endif

        retval.handle = zend_objects_store_put(intern, NULL, ws_free_storage, NULL TSRMLS_CC);
        retval.handlers = (zend_object_handlers *) &ws_object_handlers;
        return retval;
}

PHP_METHOD(ws_client, __construct) {
  zval *this;
  this = getThis();

  RETURN_TRUE;
}

PHP_METHOD(ws_client, sendBinary) {
  zval *this;
  this = getThis();
  char *to_send;
  int to_send_len;
  long payload_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &to_send, &to_send_len, &payload_len) == FAILURE) {
    RETURN_FALSE;
  }

  ws_client_object *ws_client_obj = (ws_client_object *) zend_object_store_get_object(this TSRMLS_CC);
  libwebsock_client_state *state = ws_client_obj->ws_state;
  if (state == NULL) {
    RETURN_FALSE;
  }

  libwebsock_send_binary(state, to_send, (unsigned int) payload_len);
  RETURN_TRUE;
}

PHP_METHOD(ws_client, sendText) {
  zval *this;
  this = getThis();
  char *to_send;
  int to_send_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &to_send, &to_send_len) == FAILURE) {
    RETURN_FALSE;
  }

  ws_client_object *ws_client_obj = (ws_client_object *) zend_object_store_get_object(this TSRMLS_CC);
  libwebsock_client_state *state = ws_client_obj->ws_state;
  if (state == NULL) {
    RETURN_FALSE;
  }
  
  libwebsock_send_text_with_length(state, to_send, (unsigned int) to_send_len);

  RETURN_TRUE;
}

PHP_METHOD(ws, __construct) {
	zval *this;
	

	this = getThis();
	ws_object *ws_obj = (ws_object *)zend_object_store_get_object(this TSRMLS_CC);
        ws_obj->ws_ctx = libwebsock_init();
        ws_obj->ws_ctx->onopen = ws_onopen;
        ws_obj->ws_ctx->onclose = ws_onclose;
        ws_obj->ws_ctx->onmessage = ws_onmessage;
        ws_obj->ws_ctx->user_data = (void *) this; //save this instance in context user data

	RETURN_TRUE;
}

PHP_METHOD(ws, run) {
  zval *this;

  this = getThis();

  ws_object *ws_obj = (ws_object *) zend_object_store_get_object(this TSRMLS_CC);
  libwebsock_wait(ws_obj->ws_ctx);
  RETURN_TRUE;
}

PHP_METHOD(ws, bind_ssl) {
  zval *this;

  this = getThis();
  char *host;
  char *port;
  char *cert_file;
  char *key_file;
  char *chain_file = NULL;
  int host_len;
  int port_len;
  int cert_file_len;
  int key_file_len;
  int chain_file_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssss|s", &host, &host_len, &port, &port_len,
     &key_file, &key_file_len, &cert_file, &cert_file_len, &chain_file, &chain_file_len) == FAILURE) {
    return;
  }

  ws_object *ws_obj = (ws_object *) zend_object_store_get_object(this TSRMLS_CC);
  libwebsock_bind_ssl_real(ws_obj->ws_ctx, host, port, key_file, cert_file, chain_file);
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

static int ws_onopen(libwebsock_client_state *state) {
  libwebsock_context *ctx = (libwebsock_context *) state->ctx;
  zval function_name;
  zval *return_user_call;
  ZVAL_STRING(&function_name, "__invoke", 0);
  zval *this = ctx->user_data;
  zval *closure = zend_read_property(ws_ce, this, "onopen", sizeof("onopen")-1, 0 TSRMLS_CC);
  if (closure) { //if onopen callback set in PHP
    zval *client_obj;
    zval **params[1];
    ws_client_object *intern;
    //create WebSocketClient object
    MAKE_STD_ZVAL(client_obj);
    object_init_ex(client_obj, ws_client_ce);
    zend_update_property_long(ws_client_ce, client_obj, "sockfd", sizeof("sockfd")-1, (long) state->sockfd TSRMLS_CC);
    //set call_user_function_ex params
    params[0] = &client_obj;
    //grab internal object and associate this libwebsock state with this php obj
    intern = (ws_client_object *) zend_object_store_get_object(client_obj TSRMLS_CC);
    intern->ws_state = state;
    if (call_user_function_ex(NULL, &closure, &function_name, &return_user_call, 1, params, 0, NULL TSRMLS_CC) == FAILURE) {
      //throw exception eventually
    }
    zval_ptr_dtor(&client_obj);
  }
  return 0;
}

static int ws_onclose(libwebsock_client_state *state) {
  libwebsock_context *ctx = (libwebsock_context *) state->ctx;
  zval function_name;
  zval *return_user_call;
  ZVAL_STRING(&function_name, "__invoke", 0);
  zval *this = ctx->user_data;
  zval *closure = zend_read_property(ws_ce, this, "onclose", sizeof("onclose")-1, 0 TSRMLS_CC);
  if (closure) {
    zval *client_obj;
    zval **params[1];
    ws_client_object *intern;
    //create WebSocketClient object
    MAKE_STD_ZVAL(client_obj);
    object_init_ex(client_obj, ws_client_ce);
    zend_update_property_long(ws_client_ce, client_obj, "sockfd", sizeof("sockfd")-1, (long) state->sockfd TSRMLS_CC);
    params[0] = &client_obj;
    intern = (ws_client_object *) zend_object_store_get_object(client_obj TSRMLS_CC);
    intern->ws_state = state;
    if (call_user_function_ex(NULL, &closure, &function_name, &return_user_call, 1, params, 0, NULL TSRMLS_CC) == FAILURE) {
      //something bad happened
    }
    zval_ptr_dtor(&client_obj);
  }

  return 0;
}

static int ws_onmessage(libwebsock_client_state *state, libwebsock_message *msg) {
  libwebsock_context *ctx = (libwebsock_context *) state->ctx;
  zval *this = ctx->user_data;
  zval *closure = zend_read_property(ws_ce, this, "onmessage", sizeof("onmessage")-1, 0 TSRMLS_CC);
  if (closure) {
    zval function_name;
    zval *return_user_call;
    zval *client_obj;
    zval *message_obj;
    zval **params[2];
    ws_client_object *intern_client;
    ZVAL_STRING(&function_name, "__invoke", 0);
    //create WebSocketClient object
    MAKE_STD_ZVAL(client_obj);
    object_init_ex(client_obj, ws_client_ce);
    zend_update_property_long(ws_client_ce, client_obj, "sockfd", sizeof("sockfd")-1, (long) state->sockfd TSRMLS_CC);
    params[0] = &client_obj;
    intern_client = (ws_client_object *) zend_object_store_get_object(client_obj TSRMLS_CC);
    intern_client->ws_state = state;

    //create WebSocketMessage object
    MAKE_STD_ZVAL(message_obj);
    object_init_ex(message_obj, ws_message_ce);
    zend_update_property_long(ws_message_ce, message_obj, "opcode", sizeof("opcode")-1, (long) msg->opcode TSRMLS_CC);
    zend_update_property_string(ws_message_ce, message_obj, "payload", sizeof("payload")-1, msg->payload TSRMLS_CC);
    zend_update_property_long(ws_message_ce, message_obj, "payload_len", sizeof("payload_len")-1, (long) msg->payload_len TSRMLS_CC);
    params[1] = &message_obj;
    if (call_user_function_ex(NULL, &closure, &function_name, &return_user_call, 2, params, 0, NULL TSRMLS_CC) == FAILURE) {
      //something bad happened
    }
    zval_ptr_dtor(&client_obj);
    zval_ptr_dtor(&message_obj);
  }
  return 0;
}

void register_ws_message_class(TSRMLS_D) {
  zend_class_entry ce;
  INIT_CLASS_ENTRY(ce, "WebSocketMessage", ws_message_methods);
  ce.create_object = ws_message_object_create;
  memcpy(&ws_message_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  ws_message_object_handlers.clone_obj = NULL;
  ws_message_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_declare_property_null(ws_message_ce, "payload", sizeof("payload")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
  zend_declare_property_null(ws_message_ce, "payload_len", sizeof("payload_len")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
  zend_declare_property_null(ws_message_ce, "opcode", sizeof("opcode")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
  zend_declare_class_constant_long(ws_message_ce, "OPCODE_TEXT", sizeof("OPCODE_TEXT")-1, 1 TSRMLS_CC);
  zend_declare_class_constant_long(ws_message_ce, "OPCODE_BINARY", sizeof("OPCODE_BINARY")-1, 2 TSRMLS_CC);

}

void register_ws_client_class(TSRMLS_D) {
  zend_class_entry ce;
  INIT_CLASS_ENTRY(ce, "WebSocketClient", ws_client_methods);
  ce.create_object = ws_client_object_create;
  memcpy(&ws_client_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  ws_client_object_handlers.clone_obj = NULL;
  ws_client_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_declare_property_null(ws_client_ce, "sockfd", sizeof("sockfd")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
}

void register_ws_class(TSRMLS_D) {
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "WebSocketServer", ws_methods);
	ce.create_object = ws_object_create;
	memcpy(&ws_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	ws_object_handlers.clone_obj = NULL;
	ws_ce = zend_register_internal_class(&ce TSRMLS_CC);
        zend_declare_property_null(ws_ce, "onopen", sizeof("onopen")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
        zend_declare_property_null(ws_ce, "onclose", sizeof("onclose")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
        zend_declare_property_null(ws_ce, "onmessage", sizeof("onmessage")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
}


