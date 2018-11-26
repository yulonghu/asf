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
#include "Zend/zend_interfaces.h"
#include "ext/standard/php_string.h" /* for php_basename */

#include "php_asf.h"
#include "kernel/asf_namespace.h"
#include "asf_exception.h"
#include "kernel/asf_func.h"

#include "asf_logger.h"
#include "log/asf_log_level.h"
#include "log/asf_log_loggerinterface.h"

#include "log/asf_log_adapter.h"
#include "log/adapter/asf_log_adapter_file.h"
#include "log/adapter/asf_log_adapter_syslog.h"

zend_class_entry *asf_logger_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_logger_adapter_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, adapter_id)
    ZEND_ARG_INFO(0, file_name)
ZEND_END_ARG_INFO()
/* }}}*/

void asf_logger_instance(asf_logger_t *this_ptr, zend_string *file_name, zend_string *file_path) /* {{{ */
{
    (void)asf_log_adapter_file_instance(this_ptr, file_name, file_path);
}
/* }}} */

/* {{{ proto object Asf_Logger::adapter(int $adapter_id, string $file_name)
*/
PHP_METHOD(asf_logger, adapter)
{
    zend_long adapter_id = 0;
    zend_string *file_name = NULL;
    zval *self = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "lS", &adapter_id, &file_name) == FAILURE) {
        return;
    }

    /* The empty filename is meaningless */
    if (asf_func_isempty(ZSTR_VAL(file_name))) {
        RETURN_FALSE;
    }

    switch (adapter_id) {
        case ASF_LOGGER_FILE:
            (void)asf_logger_instance(return_value, file_name, NULL);
            break;
        case ASF_LOGGER_SYSLOG:
            object_init_ex(return_value, asf_log_adapter_syslog_ce);
            break;
        default:
            RETURN_FALSE;
            break;
    }
}
/* }}} */

/* {{{ proto object Asf_Logger::init(string $full_path)
*/
PHP_METHOD(asf_logger, init)
{
    zend_string *full_path = NULL;
    zval *self = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &full_path) == FAILURE) {
        return;
    }

    /* The empty filename is meaningless */
    if (asf_func_isempty(ZSTR_VAL(full_path))) {
        php_error_docref(NULL, E_WARNING, "The parameter expected a string");
        RETURN_FALSE;
    }

    size_t path_len = ZSTR_LEN(full_path);
    if (ZSTR_VAL(full_path)[0] != '/' || ZSTR_VAL(full_path)[path_len - 1] == '/') {
        php_error_docref(NULL, E_WARNING, "The parameter expected the full path");
        RETURN_FALSE;   
    }

    zval *find = NULL;
    zval *ins = zend_read_static_property(asf_logger_ce, ZEND_STRL(ASF_LOGGER_PRONAME_INS), 1);
    if (Z_TYPE_P(ins) != IS_NULL && (find = zend_symtable_find(Z_ARRVAL_P(ins), full_path))) {
        RETURN_ZVAL(find, 1, 0);
    }

    zend_string *file_name = php_basename(ZSTR_VAL(full_path), path_len, NULL, 0);
    if (!ZSTR_LEN(file_name)) {
        zend_string_release(file_name);
        RETURN_FALSE;
    }

    size_t dir_path_len = path_len - ZSTR_LEN(file_name);
    zend_string *dir_path = zend_string_alloc(dir_path_len, 0);
    memcpy(ZSTR_VAL(dir_path), ZSTR_VAL(full_path), dir_path_len);
    ZSTR_VAL(dir_path)[dir_path_len] = '\0';

    (void)asf_logger_instance(return_value, file_name, dir_path);
    zend_string_release(file_name);
    zend_string_release(dir_path);

    if (Z_TYPE_P(return_value) == IS_OBJECT) {
        zval connect;
        if (Z_ISNULL_P(ins)) {
            array_init(&connect);
            add_assoc_zval_ex(&connect, ZSTR_VAL(full_path), ZSTR_LEN(full_path), return_value);
            zend_update_static_property(asf_logger_ce, ZEND_STRL(ASF_LOGGER_PRONAME_INS), &connect);
            zval_ptr_dtor(&connect);
        } else {
            zend_hash_add(Z_ARRVAL_P(ins), full_path, return_value);
            zend_update_static_property(asf_logger_ce, ZEND_STRL(ASF_LOGGER_PRONAME_INS), ins);
        }
        Z_TRY_ADDREF_P(return_value);
    }
}
/* }}} */

/* {{{ proto mixed Asf_Logger::getHandles(void)
*/
PHP_METHOD(asf_logger, getHandles)
{
    ZVAL_COPY(return_value,
            zend_read_static_property(asf_logger_ce, ZEND_STRL(ASF_LOGGER_PRONAME_INS), 1));
}
/* }}} */

/* {{{ asf_logger_methods[]
*/
zend_function_entry asf_logger_methods[] = {
    PHP_ME(asf_logger, adapter, asf_logger_adapter_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_logger, init, asf_logger_adapter_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_logger, getHandles, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(logger) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_logger, Asf_Logger, Asf\\Logger, ZEND_ACC_FINAL);

    zend_declare_property_null(asf_logger_ce, ZEND_STRL(ASF_LOGGER_PRONAME_INS), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);

    ASF_INIT(log_level);
    ASF_INIT(log_loggerinterface);
    ASF_INIT(log_adapter);
    ASF_INIT(log_adapter_file);
    ASF_INIT(log_adapter_syslog);

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
