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

#include "php.h"
#include "php_ws.h"

static zend_function_entry ws_functions[] = {
	{NULL, NULL, NULL}
};

zend_module_entry ws_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	PHP_WS_EXTNAME,
	ws_functions,
	PHP_MINIT(ws),
	PHP_MSHUTDOWN(ws),
	NULL, //RINIT
	NULL, //RSHUTDOWN
	PHP_MINFO(ws),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_WS_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_WS
ZEND_GET_MODULE(ws)
#endif


PHP_MINIT_FUNCTION(ws)
{
	register_ws_class(TSRMLS_C);
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(ws)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(ws)
{
  php_info_print_table_start();
  php_info_print_table_header(2, "WebSocket support", "enabled");
  php_info_print_table_header(2, "libwebsock version", libwebsock_version_string());
  php_info_print_table_end();
}
