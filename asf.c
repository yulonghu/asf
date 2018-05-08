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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "main/SAPI.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"

#include "php_asf.h"
#include "asf_application.h"
#include "asf_config.h"
#include "asf_dispatcher.h"
#include "asf_loader.h"
#include "http/asf_http_request.h"
#include "http/asf_http_response.h"
#include "asf_service.h"
#include "asf_router.h"
#include "asf_ensure.h"
#include "asf_exception.h"
#include "asf_sg.h"
#include "asf_logger.h"
#include "asf_db.h"
#include "asf_func.h"
#include "asf_util.h"

ZEND_DECLARE_MODULE_GLOBALS(asf)

/* {{{ PHP_INI
*/
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("asf.env",			  "online", PHP_INI_SYSTEM, OnUpdateString, environ_name, zend_asf_globals, asf_globals)
    STD_PHP_INI_BOOLEAN("asf.use_namespace",  "0", PHP_INI_SYSTEM, OnUpdateBool, use_namespace, zend_asf_globals, asf_globals)

    STD_PHP_INI_BOOLEAN("asf.lowcase_path",   "0", PHP_INI_ALL, OnUpdateBool, lowcase_path, zend_asf_globals, asf_globals)
    STD_PHP_INI_BOOLEAN("asf.file_suffix",    "0", PHP_INI_ALL, OnUpdateBool, file_suffix, zend_asf_globals, asf_globals)
    STD_PHP_INI_BOOLEAN("asf.action_suffix",  "1", PHP_INI_ALL, OnUpdateBool, action_suffix, zend_asf_globals, asf_globals)
    STD_PHP_INI_BOOLEAN("asf.use_lcache",     "0", PHP_INI_ALL, OnUpdateBool, use_lcache, zend_asf_globals, asf_globals)
    STD_PHP_INI_BOOLEAN("asf.throw_exception","1", PHP_INI_ALL, OnUpdateBool, throw_exception, zend_asf_globals, asf_globals)

    STD_PHP_INI_ENTRY("asf.ctype_id",		 "5", PHP_INI_ALL, OnUpdateLong, ctype_id, zend_asf_globals, asf_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_asf_init_globals
*/
static void php_asf_init_globals(zend_asf_globals *asf_globals)
{
    memset(asf_globals, 0, sizeof(*asf_globals));
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
*/
PHP_MINIT_FUNCTION(asf)
{
    ZEND_INIT_MODULE_GLOBALS(asf, php_asf_init_globals, NULL);

    REGISTER_INI_ENTRIES();

    if (ASF_G(use_namespace)) {
        REGISTER_STRINGL_CONSTANT("ASF\\VERSION", PHP_ASF_VERSION, sizeof(PHP_ASF_VERSION) - 1, CONST_PERSISTENT | CONST_CS);
    } else {
        REGISTER_STRINGL_CONSTANT("ASF_VERSION", PHP_ASF_VERSION, sizeof(PHP_ASF_VERSION) - 1, CONST_PERSISTENT | CONST_CS);
    }

    ASF_INIT(application);
    ASF_INIT(config);
    ASF_INIT(dispatcher);
    ASF_INIT(loader);
    ASF_INIT(http_request);
    ASF_INIT(http_response);
    ASF_INIT(service);
    ASF_INIT(router);
    ASF_INIT(ensure);
    ASF_INIT(exception);
    ASF_INIT(sg);
    ASF_INIT(logger);
    ASF_INIT(Db);
    ASF_INIT(util);

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
*/
PHP_MSHUTDOWN_FUNCTION(asf)
{
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
*/
PHP_RINIT_FUNCTION(asf)
{
#if defined(COMPILE_DL_ASF) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif

    /* globals var */
    ASF_G(default_module)	= zend_string_init(ASF_API_NAME, ASF_API_LEN, 0);
    ASF_G(default_service)  = zend_string_init("index", 5, 0);
    ASF_G(default_action)	= zend_string_init("index", 5, 0);

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
*/
PHP_RSHUTDOWN_FUNCTION(asf)
{
    /* one */
    if (ASF_G(root_path)) {
        zend_string_release(ASF_G(root_path));
        ASF_G(root_path) = NULL;
    }

    if (ASF_G(log_path)) {
        zend_string_release(ASF_G(log_path));
        ASF_G(log_path) = NULL;
    }

    if (ASF_G(root_path_route)) {
        zend_string_release(ASF_G(root_path_route));
        ASF_G(root_path_route) = NULL;
    }

    if (ASF_G(bootstrap)) {
        zend_string_release(ASF_G(bootstrap));
        ASF_G(bootstrap) = NULL;
    }

    if (ASF_G(constants)) {
        zend_string_release(ASF_G(constants));
        ASF_G(constants) = NULL;
    }

    if (ASF_G(base_uri)) {
        zend_string_release(ASF_G(base_uri));
        ASF_G(base_uri) = NULL;
    }

    /* two */
    if (ASF_G(default_module)) {
        zend_string_release(ASF_G(default_module));
        ASF_G(default_module) = NULL;
    }

    if (ASF_G(default_service)) {
        zend_string_release(ASF_G(default_service));
        ASF_G(default_service) = NULL;
    }

    if (ASF_G(default_action)) {
        zend_string_release(ASF_G(default_action));
        ASF_G(default_action) = NULL;
    }

    if (ASF_G(local_library)) {
        zend_string_release(ASF_G(local_library));
        ASF_G(local_library) = NULL;
    }

    /* three */
    if (Z_TYPE(ASF_G(modules)) == IS_ARRAY) {
        zval_ptr_dtor(&ASF_G(modules));
        ZVAL_UNDEF(&ASF_G(modules));
    }

    ASF_G(route) = NULL;

    /* if use.lcache = 1, Exclude throw exception on open a file in PHP function set_exception_handler */
    if (ASF_G(use_lcache) && IS_ARRAY == Z_TYPE(ASF_G(log_buffer))) {
        (void)asf_func_shutdown_buffer(1);
    }

    /* four */
    ASF_G(log_sql) = 0;
    ASF_G(log_err) = 0;

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
*/
PHP_MINFO_FUNCTION(asf)
{
    php_info_print_box_start(0);
    php_printf("%s", PHP_ASF_DESCRIPTION);
    php_info_print_box_end();

    php_info_print_table_start();
    php_info_print_table_header(2, "asf support", "enabled");
    php_info_print_table_row(2, "Author", PHP_ASF_AUTHOR);
    php_info_print_table_row(2, "Version", PHP_ASF_VERSION);
    php_info_print_table_row(2, "Build Date", __DATE__ " " __TIME__);
    php_info_print_table_end();

    /* Remove comments if you have entries in php.ini */
    DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ asf_deps[]
*/
zend_module_dep asf_deps[] = {
    ZEND_MOD_REQUIRED("spl")
    ZEND_MOD_REQUIRED("pdo")
    {NULL, NULL, NULL}
};
/* }}} */

/* {{{ asf_functions[]
    Every user visible function must have an entry in asf_functions[] */
const zend_function_entry asf_functions[] = {
    PHP_FE_END
};
/* }}} */

/* {{{ asf_module_entry
*/
zend_module_entry asf_module_entry = {
    STANDARD_MODULE_HEADER_EX,
    NULL,
    asf_deps,
    "asf",
    asf_functions,
    PHP_MINIT(asf),
    PHP_MSHUTDOWN(asf),
    PHP_RINIT(asf),
    PHP_RSHUTDOWN(asf),
    PHP_MINFO(asf),
    PHP_ASF_VERSION,
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ASF
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(asf)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
