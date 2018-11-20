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
#include "main/SAPI.h"
#include "zend_smart_str.h" /* smart_str */
#include "Zend/zend_interfaces.h" /* zend_call_ */
#include "ext/standard/basic_functions.h"
#include "ext/standard/php_var.h" /* php_var_export */
#include "ext/date/php_date.h" /* php_format_date */

#include "php_asf.h"
#include "kernel/asf_namespace.h"
#include "asf_exception.h"
#include "asf_log_adapter.h"
#include "asf_log_loggerinterface.h"
#include "asf_log_level.h"
#include "kernel/asf_func.h"
#include "log/adapter/asf_log_adapter_file.h"

zend_class_entry *asf_log_adapter_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_log_adapter_inter_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, message)
    ZEND_ARG_ARRAY_INFO(0, context, 0)
ZEND_END_ARG_INFO()
/* }}}*/

void asf_log_adapter_write_file(const char *fname, size_t fname_len, const char *method, uint method_len, const char *msg, size_t msg_len) /* {{{ */
{
    zval zmn_1, args[1], ret;
    zval logger;

    ZVAL_STRINGL(&zmn_1, method, method_len);
    ZVAL_STRINGL(&args[0], msg, msg_len);
    ZVAL_UNDEF(&logger);

    zend_string *s_fname = zend_string_init(fname, fname_len, 0);
    (void)asf_log_adapter_file_instance(&logger, s_fname, NULL);
    
    /* The ret result type is a boolean */
    call_user_function_ex(&Z_OBJCE(logger)->function_table, &logger, &zmn_1, &ret, 1, args, 1, NULL);

    ASF_FAST_STRING_PTR_DTOR(zmn_1);
    ASF_FAST_STRING_PTR_DTOR(args[0]);
    zend_string_release(s_fname);
    zval_ptr_dtor(&logger);
}
/* }}} */

static void asf_log_adapter_extract_message(zval *return_value, zval *logger, zend_string *message, zval *context) /* {{{ */
{
    if (context && IS_ARRAY == Z_TYPE_P(context)) {
        zval zmn_1, args[2];

        ZVAL_STRING(&zmn_1, "interpolate");
        ZVAL_STR(&args[0], message);
        ZVAL_COPY_VALUE(&args[1], context);

        call_user_function_ex(&Z_OBJCE_P(logger)->function_table, logger, &zmn_1, return_value, 2, args, 1, NULL);
        zval_ptr_dtor(&zmn_1);
    } else {
        ZVAL_STR_COPY(return_value, message);
    }
}
/* }}} */

/* {{{ proto bool Asf_Log_Adapter::log(string $level, string $message [, array $content]) 
*/
PHP_METHOD(asf_log_adapter, log)
{
    zend_string *message = NULL, *level = NULL;
    zval *context = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|a", &level, &message, &context) == FAILURE) {
        return;
    }

    zval *self = getThis();

    /* strtr message */
    zval retval;
    (void)asf_log_adapter_extract_message(&retval, self, message, context);

    /* format message */
    char *str_df = "d-M-Y H:i:s e", *str_ret = NULL;
    zend_string *str_date = NULL;
    size_t str_ret_len = 0;
    time_t curtime;

    time(&curtime);
    str_date = php_format_date(str_df, strlen(str_df), curtime, 1);
    str_ret_len = spprintf(&str_ret, 0, "%s [%s] %s %s", str_date->val, ZSTR_VAL(level), Z_STRVAL(retval), PHP_EOL); 
    zend_string_release(str_date);

    /* call doLog() */
    zval zmn_1, zlog[1];

    ZVAL_STRINGL(&zmn_1, "doLog", 5);
    ZVAL_STRINGL(&zlog[0], str_ret, str_ret_len);
    call_user_function_ex(&(asf_log_adapter_ce)->function_table, self, &zmn_1, return_value, 1, zlog, 1, NULL);

    zval_ptr_dtor(&zmn_1);
    zval_ptr_dtor(&retval);
    zend_string_release(Z_STR(zlog[0]));
    efree(str_ret);
}
/* }}} */

/* {{{ proto bool Asf_Log_Adapter::emergency(string $message [, array $context])
*/
ASF_LOG_ADAPTER_METHOD(emergency, "EMERGENCY", 9);
/* }}} */

/* {{{ proto bool Asf_Log_Adapter::alert(string $message [, array $context])
*/
ASF_LOG_ADAPTER_METHOD(alert, "ALERT", 5);
/* }}} */

/* {{{ proto bool Asf_Log_Adapter::critical(string $message [, array $context])
*/
ASF_LOG_ADAPTER_METHOD(critical, "CRITICAL", 8);
/* }}} */

/* {{{ proto bool Asf_Log_Adapter::error(string $message [, array $context])
*/
ASF_LOG_ADAPTER_METHOD(error, "ERROR", 5);
/* }}} */

/* {{{ proto bool Asf_Log_Adapter::warning(string $message [, array $context])
*/
ASF_LOG_ADAPTER_METHOD(warning, "WARNING", 7);
/* }}} */

/* {{{ proto bool Asf_Log_Adpater::notice(string $message [, array $context])
*/
ASF_LOG_ADAPTER_METHOD(notice, "NOTICE", 6);
/* }}} */

/* {{{ proto bool Asf_Log_Adapter::info(string $message [, array $context])
*/
ASF_LOG_ADAPTER_METHOD(info, "INFO", 4);
/* }}} */

/* {{{ proto bool Asf_Log_Adapter::debug(string $message [, array $context])
*/
ASF_LOG_ADAPTER_METHOD(debug, "DEBUG", 5);
/* }}} */

/* {{{ proto bool Asf_Log_Adapter::interpolate(string $message, array $context)
*/
PHP_METHOD(asf_log_adapter, interpolate)
{
    zend_string *message = NULL;
    zval *context = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sa", &message, &context) == FAILURE) {
        return;
    }

    zend_string *key = NULL;
    zval *val = NULL;
    smart_str buf = {0};
    zval replace;

    array_init(&replace);

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(context), key, val) {
        if (key && IS_STRING == Z_TYPE_P(val)) {
            smart_str_free(&buf);
            smart_str_appendc(&buf, '{');
            smart_str_append(&buf, key);
            smart_str_appendc(&buf, '}');
            smart_str_0(&buf);

            Z_TRY_ADDREF_P(val);
            add_assoc_zval(&replace, ZSTR_VAL(buf.s), val);
        }
    } ZEND_HASH_FOREACH_END();

    smart_str_free(&buf);

    if (IS_ARRAY == Z_TYPE(replace) && zend_hash_num_elements(Z_ARRVAL(replace)) > 0) {
        zval zmn_1, zret_1, args[2];

        ZVAL_STRING(&zmn_1, "strtr");
        ZVAL_STR(&args[0], message);
        ZVAL_COPY_VALUE(&args[1], &replace);

        if (call_user_function_ex(CG(function_table), NULL, &zmn_1, &zret_1, 2, args, 1, NULL) == SUCCESS) {
            ZVAL_COPY(return_value, &zret_1);
            zval_ptr_dtor(&zret_1);
        }

        zval_ptr_dtor(&zmn_1);
    } else {
        ZVAL_STR_COPY(return_value, message);
    }

    zval_ptr_dtor(&replace);
}
/* }}} */

/* {{{ proto object Asf_Log_Adapter::getFormatter(void)
*/
PHP_METHOD(asf_log_adapter, getFormatter)
{
    return;
}
/* }}} */

/* {{{ proto bool Asf_Log_Adapter::doLog(string $level, int time, string $message)
*/
PHP_METHOD(asf_log_adapter, doLog)
{
    return;
}
/* }}} */

/* {{{ asf_log_adapter_methods[]
*/
zend_function_entry asf_log_adapter_methods[] = {
    PHP_ME(asf_log_adapter, emergency,	   asf_log_loggerinterface_common_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter, alert,		   asf_log_loggerinterface_common_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter, critical,	   asf_log_loggerinterface_common_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter, error,		   asf_log_loggerinterface_common_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter, warning,	   asf_log_loggerinterface_common_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter, notice,		   asf_log_loggerinterface_common_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter, info,		   asf_log_loggerinterface_common_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter, debug,		   asf_log_loggerinterface_common_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter, log,		   asf_log_loggerinterface_log_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter, interpolate,   asf_log_adapter_inter_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter, getFormatter,  NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter, doLog,		   NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(log_adapter) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_log_adapter, Asf_Log_AbstractLogger, Asf\\Log\\AbstractLogger, ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);

    zend_class_implements(asf_log_adapter_ce, 1, asf_log_loggerinterface_ce);

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
