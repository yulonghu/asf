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

#ifndef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "Zend/zend_interfaces.h" /* zend_call_method_with_* */
#include "ext/standard/php_var.h"
#include "ext/standard/php_string.h" /* php_basename */

#include "php_asf.h"
#include "asf_namespace.h"
#include "asf_application.h"
#include "asf_loader.h"
#include "asf_dispatcher.h"
#include "kernel/asf_bootstrap.h"
#include "kernel/asf_constants.h"
#include "asf_exception.h"
#include "asf_sg.h"
#include "asf_func.h"

#include "config/asf_config_abstractconfig.h"
#include "http/asf_http_request.h"
#include "http/asf_http_response.h"
#include "log/asf_log_adapter.h"

zend_class_entry * asf_application_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_app_construct_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, config)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_app_errorhandler_arginfo, 0, 0, 4)
	ZEND_ARG_INFO(0, errno)
	ZEND_ARG_INFO(0, errstr)
	ZEND_ARG_INFO(0, errfile)
	ZEND_ARG_INFO(0, errline)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_app_exceptionhandler_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, exception)
ZEND_END_ARG_INFO()
/* }}}*/

static _Bool asf_application_merge_config(zval *zconfig) /* {{{ */
{
    HashTable *pzconf = NULL;
    zval *pzconfig = NULL, *pzapp = NULL, *pzval_1 = NULL, *pzval_2 = NULL, *pzval_3 = NULL;

    pzconfig = zend_read_property(asf_absconfig_ce, zconfig, ZEND_STRL(ASF_ABSCONFIG_PROPERTY_NAME), 1, NULL);

    if (UNEXPECTED(Z_TYPE_P(pzconfig) == IS_NULL)) {
        return 0;
    }

    pzconf = HASH_OF(pzconfig);

    if (UNEXPECTED((pzapp = zend_hash_str_find(pzconf, ZEND_STRL("asf"))) == NULL || 
                Z_TYPE_P(pzapp) != IS_ARRAY)) {
        asf_trigger_error(ASF_ERR_STARTUP_FAILED, "Application config must be a array");
        return 0;
    }

    if (UNEXPECTED((pzval_1 = zend_hash_str_find(Z_ARRVAL_P(pzapp), ZEND_STRL("root_path"))) == NULL || 
                Z_TYPE_P(pzval_1) != IS_STRING || Z_STRLEN_P(pzval_1) == 0)) {
        asf_trigger_error(ASF_ERR_STARTUP_FAILED, "Application root_path option not found in config");
        return 0;
    }
    ASF_G(root_path) = zend_string_copy(Z_STR_P(pzval_1));

    if ((pzval_1 = zend_hash_str_find(Z_ARRVAL_P(pzapp), ZEND_STRL("log_path"))) != NULL && 
            Z_TYPE_P(pzval_1) == IS_STRING && Z_STRLEN_P(pzval_1)) {
        ASF_G(log_path) = zend_string_copy(Z_STR_P(pzval_1));
    }

    if (UNEXPECTED((pzval_1 = zend_hash_str_find(Z_ARRVAL_P(pzapp), ZEND_STRL("bootstrap"))) != NULL &&
                Z_TYPE_P(pzval_1) == IS_STRING && Z_STRLEN_P(pzval_1) > 0)) {
        ASF_G(bootstrap) = zend_string_copy(Z_STR_P(pzval_1));
    }

    if (UNEXPECTED((pzval_1 = zend_hash_str_find(Z_ARRVAL_P(pzapp), ZEND_STRL("constants"))) != NULL &&
                Z_TYPE_P(pzval_1) == IS_STRING && Z_STRLEN_P(pzval_1) > 0)) {
        ASF_G(constants) = zend_string_copy(Z_STR_P(pzval_1));
    }

    if (UNEXPECTED((pzval_1 = zend_hash_str_find(Z_ARRVAL_P(pzapp), ZEND_STRL("base_uri"))) != NULL &&
                Z_TYPE_P(pzval_1) == IS_STRING && Z_STRLEN_P(pzval_1) > 0)) {
        ASF_G(base_uri) = zend_string_copy(Z_STR_P(pzval_1));
    }

    if (EXPECTED((pzval_1 = zend_hash_str_find(Z_ARRVAL_P(pzapp), ZEND_STRL("library"))) != NULL)) {
        if (EXPECTED(IS_STRING == Z_TYPE_P(pzval_1) && Z_STRLEN_P(pzval_1) > 0)) {
            if (*(Z_STRVAL_P(pzval_1) + Z_STRLEN_P(pzval_1)) == DEFAULT_SLASH) {
                ASF_G(local_library) = zend_string_init(Z_STRVAL_P(pzval_1), Z_STRLEN_P(pzval_1) - 1, 0);
            } else {
                ASF_G(local_library) = zend_string_copy(Z_STR_P(pzval_1));
            }
        }
    }

    if (UNEXPECTED((pzval_1 = zend_hash_str_find(Z_ARRVAL_P(pzapp),
                        ZEND_STRL("dispatcher"))) != NULL && Z_TYPE_P(pzval_1) == IS_ARRAY)) {
        if ((pzval_2 = zend_hash_str_find(Z_ARRVAL_P(pzval_1), ZEND_STRL("default_module"))) != NULL &&
                Z_TYPE_P(pzval_2) == IS_STRING && Z_STRLEN_P(pzval_2) > 0) {
            zend_string_release(ASF_G(default_module));
            ASF_G(default_module) = zend_string_dup(Z_STR_P(pzval_2), 0);
        }

        if ((pzval_2 = zend_hash_str_find(Z_ARRVAL_P(pzval_1), ZEND_STRL("default_service"))) != NULL &&
                Z_TYPE_P(pzval_2) == IS_STRING && Z_STRLEN_P(pzval_2) > 0) {
            zend_string_release(ASF_G(default_service));
            ASF_G(default_service) = zend_string_dup(Z_STR_P(pzval_2), 0);
        }

        if ((pzval_2 = zend_hash_str_find(Z_ARRVAL_P(pzval_1), ZEND_STRL("default_action"))) != NULL &&
                Z_TYPE_P(pzval_2) == IS_STRING && Z_STRLEN_P(pzval_2) > 0) {
            zend_string_release(ASF_G(default_action));
            ASF_G(default_action) = zend_string_dup(Z_STR_P(pzval_2), 0);
        }

        if ((pzval_2 = zend_hash_str_find(Z_ARRVAL_P(pzval_1), ZEND_STRL("route"))) != NULL &&
                Z_TYPE_P(pzval_2) == IS_ARRAY) {
            ASF_G(route) = pzval_2;
        }

        /* log.* */
        if ((pzval_2 = zend_hash_str_find(Z_ARRVAL_P(pzval_1),
                        ZEND_STRL("log"))) != NULL && Z_TYPE_P(pzval_2) == IS_ARRAY && Z_STRLEN_P(pzval_2) > 0) {
            if ((pzval_3 = zend_hash_str_find(Z_ARRVAL_P(pzval_2), ZEND_STRL("sql"))) != NULL) {
                ASF_G(log_sql) = zend_is_true(pzval_3);
            }
            if ((pzval_3 = zend_hash_str_find(Z_ARRVAL_P(pzval_2), ZEND_STRL("err"))) != NULL) {
                ASF_G(log_err) = zend_is_true(pzval_3);
            }
        }
    }

    /* (module).sevice.action */
    array_init(&ASF_G(modules));

    if ((pzval_1 = zend_hash_str_find(Z_ARRVAL_P(pzapp), ZEND_STRL("modules"))) != NULL &&
            Z_TYPE_P(pzval_1) == IS_STRING && Z_STRLEN_P(pzval_1) > 0) {

        char *tok_ptr = NULL;
        char *modules = estrndup(Z_STRVAL_P(pzval_1), Z_STRLEN_P(pzval_1));
        char *seg = php_strtok_r(modules, ",", &tok_ptr);

        while (seg) {
            if (strlen(seg) > 0) {
                add_next_index_stringl(&ASF_G(modules), seg, strlen(seg));
            }
            seg = php_strtok_r(NULL, ",", &tok_ptr);
        }

        efree(modules);
    } else {
        add_next_index_stringl(&ASF_G(modules), ZSTR_VAL(ASF_G(default_module)), ZSTR_LEN(ASF_G(default_module)));
    }

    return 1;
}
/* }}}*/

static _Bool asf_application_instance(zval *self, zval *config, zval *section) /* {{{ */
{
    zval zconfig = {{0}},     zrequest = {{0}}, zsg = {{0}};
    zval zdispatcher = {{0}}, zloader = {{0}};

    (void)asf_absconfig_instance(&zconfig, config, section);
    if  (UNEXPECTED(Z_TYPE(zconfig) != IS_OBJECT || !asf_application_merge_config(&zconfig))) {
        asf_trigger_error(ASF_ERR_STARTUP_FAILED, "Initialized config failed");
        zval_ptr_dtor(&zconfig);
        return 0;
    }

    zend_update_property(asf_application_ce, self, ZEND_STRL(ASF_APP_PRONAME_CONFIG), &zconfig);
    zval_ptr_dtor(&zconfig);

    (void)asf_http_req_instance(&zrequest, ASF_G(base_uri), NULL);
    if (UNEXPECTED(Z_TYPE(zrequest) != IS_OBJECT)) {
        asf_trigger_error(ASF_ERR_STARTUP_FAILED, "Initialized request failed");
        return 0;
    }

    (void)asf_dispatcher_instance(&zdispatcher, &zrequest);
    if (UNEXPECTED(Z_TYPE(zdispatcher) != IS_OBJECT)) {
        asf_trigger_error(ASF_ERR_STARTUP_FAILED, "initialized dispatcher failed");
        return 0;
    }

    zend_update_property(asf_application_ce, self, ZEND_STRL(ASF_APP_PRONAME_DISPATCHER), &zdispatcher);
    zval_ptr_dtor(&zrequest);
    zval_ptr_dtor(&zdispatcher);

    (void)asf_sg_instance(&zsg);

    if (ASF_G(local_library)) {
        (void)asf_loader_instance(&zloader, ASF_G(local_library));
    } else {
        zend_string *dir = zend_string_init(ZSTR_VAL(ASF_G(root_path)), ZSTR_LEN(ASF_G(root_path)), 0);
        php_dirname(ZSTR_VAL(dir), ZSTR_LEN(dir));
        zend_string *local_library = strpprintf(0, "%s%c%s", ZSTR_VAL(dir), DEFAULT_SLASH, ASF_LOADER_LIBRARY_DIRECTORY_NAME);
        (void)asf_loader_instance(&zloader, local_library);
        zend_string_release(local_library);
        zend_string_release(dir);
    }

    if (UNEXPECTED(Z_TYPE(zloader) != IS_OBJECT)) {
        asf_trigger_error(ASF_ERR_STARTUP_FAILED, "initialized loader failed");
        return 0;
    }

    if (ASF_G(log_err)) {
        ASF_FUNC_CALL_PHP_FUNC(self, "set_error_handler", "errorHandler", 12, NULL, 1);
        ASF_FUNC_CALL_PHP_FUNC(self, "set_exception_handler", "exceptionHandler", 16, NULL, 1);
    }

    zend_update_static_property(asf_application_ce, ZEND_STRL(ASF_APP_PRONAME_INSTANCE), self);

    return 1;
}
/* }}} */

/* {{{ proto object Asf_Application::__construct(mixed $config [, string $section])
    Application construct, The framework running entrance */
PHP_METHOD(asf_application, __construct)
{
    zval *config = NULL, *section = NULL;
    _Bool ret = 0;
 
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL_DEREF(config)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL_DEREF(section)
    ZEND_PARSE_PARAMETERS_END_EX(zend_bailout());

    /* A small amount of redundancy */
    if (UNEXPECTED(!ZVAL_IS_NULL(zend_read_static_property(asf_application_ce, ZEND_STRL(ASF_APP_PRONAME_INSTANCE), 1)))) {
        asf_trigger_error(ASF_ERR_STARTUP_FAILED, "Application is duplicate initialized");
        RETURN_FALSE;
    }

    ret = asf_application_instance(getThis(), config, section);

    if (UNEXPECTED(!ret)) {
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto object Asf_Application::constants(void)
*/
PHP_METHOD(asf_application, constants)
{
    ASF_APPLICATION_CONS_BOOT(constants, CONSTANTS, ASF_CONSTANTS_DEFAULT_NAME_UF, ASF_CONSTANTS_DEFAULT_NAME_LC, 1);
}
/* }}} */

/* {{{ proto object Asf_Application::bootstrap(void)
*/
PHP_METHOD(asf_application, bootstrap)
{
    ASF_APPLICATION_CONS_BOOT(bootstrap, BOOTSTRAP, ASF_BOOTSTRAP_DEFAULT_NAME_UF, ASF_BOOTSTRAP_DEFAULT_NAME_LC, 0);

    zend_string *func_name = NULL;
    zval bootstrap;

    zval *dispatcher = zend_read_property(asf_application_ce, getThis(), ZEND_STRL(ASF_APP_PRONAME_DISPATCHER), 1, NULL);

    object_init_ex(&bootstrap, ce);

    ZEND_HASH_FOREACH_STR_KEY(&(ce->function_table), func_name) {
        if (strncasecmp(ZSTR_VAL(func_name), ZEND_STRL(ASF_BOOTSTRAP_INITFUNC_PREFIX))) {
            continue;
        }
        zend_call_method(&bootstrap, ce, NULL, ZSTR_VAL(func_name), ZSTR_LEN(func_name), NULL, 1, dispatcher, NULL);

        if (UNEXPECTED(EG(exception))) {
            zval_ptr_dtor(&bootstrap);
            RETURN_FALSE;
        }
    } ZEND_HASH_FOREACH_END();

    zval_ptr_dtor(&bootstrap);

    RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/* {{{ proto bool Asf_Application::run(void)
    The php-fpm mode is start, The CLI mode and the php-fpm mode select only one */
PHP_METHOD(asf_application, run)
{
    asf_http_rep_t response = {{0}};
    asf_disp_t *dispatcher = NULL;

    dispatcher = zend_read_property(asf_application_ce, getThis(),
            ZEND_STRL(ASF_APP_PRONAME_DISPATCHER), 1, NULL);

    /* Unexpected exit before 'asf_dispatcher_instance()' function */
    if (UNEXPECTED(Z_TYPE_P(dispatcher) != IS_OBJECT)) {
        asf_trigger_error(ASF_ERR_STARTUP_FAILED, "initialized dispatcher failed");
        return;
    }

    if (EXPECTED(asf_dispatcher_ondispatch(dispatcher, &response))) {
        if (UNEXPECTED(Z_ISUNDEF(response))) {
            RETURN_FALSE;
        }

        (void)asf_http_rep_display_success(&response);
        zval_ptr_dtor(&response);
    }

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool Asf_Application::task(void)
    The cli mode is start, The CLI mode and the php-fpm mode select only one */
PHP_METHOD(asf_application, task)
{
    zval *self = getThis();
    zval *dispatcher = zend_read_property(asf_application_ce,
            self, ZEND_STRL(ASF_APP_PRONAME_DISPATCHER), 1, NULL);

    if (!dispatcher || IS_OBJECT != Z_TYPE_P(dispatcher)) {
        asf_trigger_error(ASF_ERR_STARTUP_FAILED, "initialized dispatcher failed");
        return;
    }

    zval *request = zend_read_property(Z_OBJCE_P(dispatcher),
            dispatcher, ZEND_STRL(ASF_DISP_PRONAME_REQ), 1, NULL);
    zval *module = zend_read_property(Z_OBJCE_P(request),
            request, ZEND_STRL(ASF_HTTP_REQ_PRONAME_MODULE), 1, NULL);

    zend_update_property_long(asf_application_ce, self, ZEND_STRL(ASF_APP_PRONAME_MODE), 1);

    /* If it is in multi-module mode */
    (void)asf_func_set_cur_module(Z_STRVAL_P(module));

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto object Asf_Application::init(mixed $config [, string $section])
    Application construct, Another the framewok running entrance */
PHP_METHOD(asf_application, init)
{
    zval *config = NULL, *section = NULL;
    _Bool ret = 0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL_DEREF(config)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL_DEREF(section)
    ZEND_PARSE_PARAMETERS_END_EX(zend_bailout());

    /* A small amount of redundancy */
    if (UNEXPECTED(!ZVAL_IS_NULL(zend_read_static_property(asf_application_ce, ZEND_STRL(ASF_APP_PRONAME_INSTANCE), 1)))) {
        asf_trigger_error(ASF_ERR_STARTUP_FAILED, "Application is duplicate initialized");
        RETURN_FALSE;
    }

    if (UNEXPECTED(object_init_ex(return_value, asf_application_ce) != SUCCESS)) {
        RETURN_FALSE;
    }

    ret = asf_application_instance(return_value, config, section);

    if (UNEXPECTED(!ret)) {
        zval_ptr_dtor(return_value);
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto object Asf_Application::getInstance(void)
    Return Application Object */
PHP_METHOD(asf_application, getInstance)
{
    ZVAL_COPY(return_value, zend_read_static_property(asf_application_ce,
                ZEND_STRL(ASF_APP_PRONAME_INSTANCE), 1));
}
/* }}} */

/* {{{ proto object Asf_Application::getConfig(void)
    Return Config Object */
PHP_METHOD(asf_application, getConfig)
{
    ZVAL_COPY(return_value, zend_read_property(asf_application_ce, 
                getThis(), ZEND_STRL(ASF_APP_PRONAME_CONFIG), 1, NULL));
}
/* }}} */

/* {{{ proto array Asf_Application::getModules(void)
    Return all whitelist modules */
PHP_METHOD(asf_application, getModules)
{
    if (Z_TYPE(ASF_G(modules)) == IS_ARRAY) {
        ZVAL_COPY(return_value, & ASF_G(modules));
    } else {
        return;
    }
}
/* }}} */

/* {{{ proto string Asf_Application::getRootPath(void)
    Return asf.root_path config */
PHP_METHOD(asf_application, getRootPath)
{
    RETURN_STR(zend_string_copy(ASF_G(root_path)));
}
/* }}} */

/* {{{ proto string Asf_Application::getEnv(void)
    Return asf.env config */
PHP_METHOD(asf_application, getEnv)
{
    zval *env = zend_read_property(asf_application_ce,
            getThis(), ZEND_STRL(ASF_APP_PRONAME_ENV), 1, NULL);
    ZEND_ASSERT(Z_TYPE_P(env) == IS_STRING);
    RETURN_STR(zend_string_copy(Z_STR_P(env)));
}
/* }}} */

/* {{{ proto string Asf_Application::getMode(void)
    Return application run mode */
PHP_METHOD(asf_application, getMode)
{
    ZVAL_COPY(return_value, zend_read_property(asf_application_ce, 
                getThis(), ZEND_STRL(ASF_APP_PRONAME_MODE), 1, NULL));
    ZEND_ASSERT(Z_TYPE_P(return_value) == IS_LONG);
}
/* }}} */

PHP_METHOD(asf_application, __sleep) /* {{{ */
{
}
/* }}} */

PHP_METHOD(asf_application, __wakeup) /* {{{ */
{
}
/* }}} */

PHP_METHOD(asf_application, __clone) /* {{{ */
{
}
/* }}} */

/* {{{ proto void Asf_Application::__destruct(void)
 */
PHP_METHOD(asf_application, __destruct)
{
    zend_update_static_property_null(asf_application_ce, ZEND_STRL(ASF_APP_PRONAME_INSTANCE));
    zend_update_static_property_null(asf_dispatcher_ce, ZEND_STRL(ASF_DISP_PRONAME_INS));
    zend_update_static_property_null(asf_loader_ce, ZEND_STRL(ASF_LOADER_PROPERTY_NAME_INSTANCE));
}
/* }}} */

/* {{{ proto void Asf_Application::errorHandler(uint $_0, string $_2, string $_3, uint $_1 [, mixed $_4])
     Set a error handler function, Please do not overwrite the call */
PHP_METHOD(asf_application, errorHandler)
{
    zend_long _0 = 0, _1 = 0;
    zend_string *_2 = NULL, *_3 = NULL;
    zval *_4 = NULL;
    char *errmsg = NULL, *errtype = NULL, *level = NULL;
    uint level_len = 6;
    size_t errmsg_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "lSSl|z", &_0, &_2, &_3, &_1, &_4) == FAILURE) {
        return;
    }

    zval *zef = zend_read_property(asf_application_ce, getThis(), ZEND_STRL(ASF_APP_PRONAME_ERR_FNAME), 1, NULL);

    switch (_0) {/*{{{*/
        case E_PARSE:
        case E_ERROR:
        case E_CORE_ERROR:
        case E_COMPILE_ERROR:
        case E_USER_ERROR:
            errtype = "Fatal Error";
            level = "error"; level_len = 5;
            break;
        case E_WARNING:
        case E_USER_WARNING:
        case E_CORE_WARNING:
        case E_COMPILE_WARNING:
        case E_RECOVERABLE_ERROR:
            errtype = "Warning";
            level = "warning";
            break;
        case E_NOTICE:
        case E_USER_NOTICE:
            errtype = "Notice";
            level = "notice";
            break;
        case E_STRICT:
            errtype = "Strict";
            level = "notice";
            break;
        case E_DEPRECATED:
        case E_USER_DEPRECATED:
            errtype = "Deprecated";
            level = "notice";
            break;
        default:
            errtype = "Unknown Error";
            level = "error";
            break;
    }/*}}}*/

    /* If _4 is a string type, it comes from exceptionHandler */
    if (Z_TYPE_P(_4) == IS_STRING) {
        errmsg_len = spprintf(&errmsg, 0, "%s: %s in %s on line %d%s%s", errtype, ZSTR_VAL(_2), ZSTR_VAL(_3), _1, PHP_EOL, Z_STRVAL_P(_4));
    } else {
        errmsg_len = spprintf(&errmsg, 0, "%s: %s in %s on line %d", errtype, ZSTR_VAL(_2), ZSTR_VAL(_3), _1);
    }

    zval zlogger = {{0}};
    (void)asf_log_adapter_create_file_handler(getThis(), &zlogger, Z_STRVAL_P(zef), Z_STRLEN_P(zef));
    (void)asf_log_adapter_write_file(&zlogger, level, level_len, errmsg, errmsg_len);

    efree(errmsg);

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto void Asf_Application::exceptionHandler(object $_0)
     Set a exception handler function, Please do not overwrite the call */
PHP_METHOD(asf_application, exceptionHandler)
{
    zval *_0 = NULL, _1, *_2 = NULL, *_3 = NULL;
    char *errmsg = NULL;
    uint errmsg_len = 0;
    zval args[5], zparam;
    zend_class_entry *ce = NULL;
    zend_long _4 = 0, _5 = 0;
    zval trace;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &_0) == FAILURE) {
        return;
    }

    ce = Z_OBJCE_P(_0);
    zval *self = getThis();

    _2 = zend_read_property(ce, _0, ZEND_STRL("message"), 1, NULL);
    _3 = zend_read_property(ce, _0, ZEND_STRL("file"), 1, NULL);
    _4 = E_USER_ERROR;
    _5 = zval_get_long(zend_read_property(ce, _0, ZEND_STRL("line"), 1, NULL));

    /* getTraceAsString */
    zend_call_method_with_0_params(_0, ce, NULL, "gettraceasstring", &trace);
   
    ZVAL_STRINGL(&_1, "errorHandler", 12);

    ZVAL_LONG(&args[0], 1);
    ZVAL_COPY_VALUE(&args[1], _2);
    ZVAL_COPY_VALUE(&args[2], _3);
    ZVAL_LONG(&args[3], _5);
    ZVAL_COPY_VALUE(&args[4], &trace);

    call_user_function_ex(&(Z_OBJCE_P(self))->function_table, self, &_1, return_value, 5, args, 1, NULL);

    ASF_FAST_STRING_PTR_DTOR(_1);
    if (Z_TYPE(trace) != IS_FALSE) {
        zval_ptr_dtor(&trace);
    }

    ZVAL_NULL(&zparam);
    (void)asf_http_rep_display_error(999, &zparam);
}
/* }}} */

/* {{{ proto bool Asf_Application::setLogErrFileName(void)
    Set log error filename, Related configuration asf.log.err = 1 */
PHP_METHOD(asf_application, setLogErrFileName)
{
    zend_string *fname = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &fname) == FAILURE) {
        return;
    }

    if (UNEXPECTED(!fname || asf_func_isempty(ZSTR_VAL(fname)))) {
        RETURN_FALSE;
    }

    zend_update_property_str(asf_application_ce, getThis(), ZEND_STRL(ASF_APP_PRONAME_ERR_FNAME), fname);

    RETURN_TRUE;
}/*}}}*/

/* {{{ asf_application_methods[]
*/
zend_function_entry asf_application_methods[] = {
    PHP_ME(asf_application, bootstrap,          NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_application, constants,          NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_application, run,                NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_application, task,               NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_application, getConfig,          NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_application, getModules,         NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_application, getRootPath,        NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_application, getEnv,             NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_application, getMode,            NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_application, setLogErrFileName,  NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_application, __sleep,            NULL, ZEND_ACC_PRIVATE)
    PHP_ME(asf_application, __wakeup,           NULL, ZEND_ACC_PRIVATE)
    PHP_ME(asf_application, __destruct,         NULL, ZEND_ACC_PUBLIC  | ZEND_ACC_DTOR)
    PHP_ME(asf_application, __clone,            NULL, ZEND_ACC_PRIVATE)
    PHP_ME(asf_application, getInstance,        NULL, ZEND_ACC_PUBLIC  | ZEND_ACC_STATIC)
    PHP_ME(asf_application, init,               asf_app_construct_arginfo, ZEND_ACC_PUBLIC  | ZEND_ACC_STATIC)
    PHP_ME(asf_application, __construct,        asf_app_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(asf_application, errorHandler,       asf_app_errorhandler_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_application, exceptionHandler,   asf_app_exceptionhandler_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(application) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_application, Asf_Application, Asf\\Application, ZEND_ACC_FINAL);

    zend_declare_property_null(asf_application_ce, ZEND_STRL(ASF_APP_PRONAME_CONFIG), ZEND_ACC_PROTECTED);
    zend_declare_property_null(asf_application_ce, ZEND_STRL(ASF_APP_PRONAME_DISPATCHER), ZEND_ACC_PROTECTED);

    zend_declare_property_string(asf_application_ce, ZEND_STRL(ASF_APP_PRONAME_ENV), ASF_G(environ_name), ZEND_ACC_PROTECTED);
    zend_declare_property_null(asf_application_ce, ZEND_STRL(ASF_APP_PRONAME_INSTANCE), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
    zend_declare_property_long(asf_application_ce, ZEND_STRL(ASF_APP_PRONAME_MODE), 0, ZEND_ACC_PROTECTED);
    zend_declare_property_stringl(asf_application_ce, ZEND_STRL(ASF_APP_PRONAME_ERR_FNAME), "Asf_Err_Log", 11, ZEND_ACC_PROTECTED);

    return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
