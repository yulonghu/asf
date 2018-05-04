/*
  +----------------------------------------------------------------------+
  | API Services Framework                                               |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Jiapeng Fan <fanjiapeng@126.com>                             |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_ASF_H
#define PHP_ASF_H

extern zend_module_entry asf_module_entry;
#define phpext_asf_ptr &asf_module_entry

#define PHP_ASF_VERSION "2.1.5"
#define PHP_ASF_AUTHOR  "fanjiapeng"
#define PHP_ASF_DESCRIPTION "This is PHP framework written in C. It is simple, fast, standard, security."

#ifdef PHP_WIN32
#	define PHP_ASF_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_ASF_API __attribute__ ((visibility("default")))
#else
#	define PHP_ASF_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define ASF_INIT_CLASS(name) ZEND_MODULE_STARTUP_D(asf_ ##name)
#define ASF_INIT(name) ZEND_MODULE_STARTUP_N(asf_ ##name)(INIT_FUNC_ARGS_PASSTHRU)
#define ASF_API_NAME "api"
#define ASF_API_LEN	3

#define asf_router_t	zval
#define asf_route_t		zval
#define asf_service_t	zval
#define asf_loader_t	zval
#define asf_sg_t		zval
#define asf_logger_t	zval
#define asf_db_t		zval
#define asf_disp_t		zval

#define asf_http_req_t	zval
#define asf_http_rep_t	zval

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:
*/
ZEND_BEGIN_MODULE_GLOBALS(asf)
	zend_string *root_path;
	zend_string *log_path;
	zend_string *root_path_route;
	zend_string *bootstrap;
	zend_string *constants;
	zend_string *base_uri;

	zend_string *default_module;
	zend_string *default_service;
	zend_string *default_action;
	zend_string *local_library;

    /* router */
	zval		modules;
	zval		*route;
    
    /* log */
	zval		log_buffer;
    zend_bool   log_sql;
    zend_bool   log_err;

	/* ini */
	char		*environ_name;
	zend_bool   lowcase_path;
	zend_bool	file_suffix;
	zend_bool   action_suffix;
	zend_bool   use_namespace;
	zend_ulong	ctype_id;
	zend_bool	use_lcache;
	zend_bool   throw_exception;
ZEND_END_MODULE_GLOBALS(asf)

/* Always refer to the globals in your function as ASF_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
extern ZEND_DECLARE_MODULE_GLOBALS(asf);
#define ASF_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(asf, v)

#if defined(ZTS) && defined(COMPILE_DL_ASF)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_ASF_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
