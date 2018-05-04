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
#include "php_asf.h"
#include "ext/standard/basic_functions.h" /* BG */
#include "ext/standard/php_ext_syslog.h" /* php_syslog */
#include "Zend/zend_interfaces.h" /* zend_call_ */

#include "asf_namespace.h"
#include "asf_exception.h"
#include "asf_log_adapter_syslog.h"
#include "log/asf_log_adapter.h"
#include "log/asf_log_level.h"
#include "log/formatter/asf_log_formatter_syslog.h"
#include "asf_func.h"

zend_class_entry *asf_log_adapter_syslog_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_log_adapter_syslog_construct_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, ident)
    ZEND_ARG_INFO(0, options)
    ZEND_ARG_INFO(0, facility)
    ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_log_adapter_syslog_dolog_arginfo, 0, 0, 3)
    ZEND_ARG_INFO(0, level)
    ZEND_ARG_INFO(0, time)
    ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()
/* }}}*/

/* {{{ proto object Asf_Log_Adapter_Syslog::__construct(string $ident [, int $options = LOG_PID [, int $facility = LOG_LOCAL0]])
*/
PHP_METHOD(asf_log_adapter_syslog, __construct)
{
    zend_string *ident = NULL;
    zend_long options = 0, facility = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|ll", &ident, &options, &facility) == FAILURE) {
        return;
    }

    if (options == 0) {
        options = LOG_ODELAY;
    }

    if (facility == 0) {
        facility = LOG_USER;
    }

    if (BG(syslog_device)) {
        free(BG(syslog_device));
        BG(syslog_device) = NULL;
    }

    BG(syslog_device) = zend_strndup(ZSTR_VAL(ident), ZSTR_LEN(ident));
    if(BG(syslog_device) == NULL) {
        return;
    }

    openlog(BG(syslog_device), options, facility);

    ASF_FUNC_REGISTER_SHUTDOWN_FUNCTION_CLOSE(getThis());
}
/* }}} */

/* {{{ proto object Asf_Log_Adapter_Syslog::getFormatter(void)
*/
PHP_METHOD(asf_log_adapter_syslog, getFormatter)
{
    zval *zformatter = zend_read_property(asf_log_adapter_syslog_ce, getThis(), ZEND_STRL(ASF_LOG_ADAPTER_PRONAME_FORMATTER), 1, NULL);

    if (IS_OBJECT == Z_TYPE_P(zformatter)) {
        RETURN_ZVAL(zformatter, 1, 0);
    }

    object_init_ex(return_value, asf_log_formatter_syslog_ce);

    zend_update_property(asf_log_adapter_syslog_ce, getThis(), ZEND_STRL(ASF_LOG_ADAPTER_PRONAME_FORMATTER), return_value);
}
/* }}} */

/* {{{ proto bool Asf_Log_Adapter_Syslog::doLog(string $level, int time, string $message)
*/
PHP_METHOD(asf_log_adapter_syslog, doLog)
{
    zval *message = NULL, *level = NULL;
    zend_long time = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "zlz", &level, &time, &message) == FAILURE) {
        return;
    }

    zval *self = getThis();
    zval zret_1, zret_2;
    zval zmn_1, zmn_2;
    zval args[3];

    ZVAL_STRING(&zmn_1, "getFormatter");
    call_user_function_ex(&(asf_log_adapter_syslog_ce)->function_table, self, &zmn_1, &zret_1, 0, 0, 1, NULL);

    zval_ptr_dtor(&zmn_1);

    if (IS_OBJECT != Z_TYPE(zret_1)) {
        RETURN_FALSE;
    }

    ZVAL_STRING(&zmn_2, "format");
    ZVAL_COPY_VALUE(&args[0], level);
    ZVAL_LONG(&args[1], time);
    ZVAL_COPY_VALUE(&args[2], message);

    call_user_function_ex(&Z_OBJCE(zret_1)->function_table, &zret_1, &zmn_2, &zret_2, 3, args, 1, NULL);

    zval_ptr_dtor(&zret_1);
    zval_ptr_dtor(&zmn_2);

    if (IS_STRING != Z_TYPE(zret_2)) {
        RETURN_FALSE;
    }

    php_syslog(LOG_INFO, "%s", Z_STRVAL(zret_2));
    zval_ptr_dtor(&zret_2);

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool Asf_Log_Adapter_Syslog::close(void)
*/
PHP_METHOD(asf_log_adapter_syslog, close)
{
    zend_call_method_with_0_params(NULL, NULL, NULL, "closelog", return_value);
}
/* }}} */

/* {{{ asf_log_adapter_syslog_methods[]
*/
zend_function_entry asf_log_adapter_syslog_methods[] = {
    PHP_ME(asf_log_adapter_syslog, __construct,  asf_log_adapter_syslog_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(asf_log_adapter_syslog, close,		 NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter_syslog, doLog,		 asf_log_adapter_syslog_dolog_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter_syslog, getFormatter, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(log_adapter_syslog) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERNAL(asf_log_adapter_syslog, Asf_Log_Adapter_Syslog, Asf\\Log\\Adapter\\Syslog, asf_log_adapter_ce, asf_log_adapter_syslog_methods);

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
