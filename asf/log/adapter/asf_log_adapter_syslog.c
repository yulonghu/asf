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

#include "kernel/asf_namespace.h"
#include "asf_exception.h"
#include "asf_log_adapter_syslog.h"
#include "log/asf_log_adapter.h"
#include "log/asf_log_level.h"
#include "kernel/asf_func.h"

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
    zval *self = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|ll", &ident, &options, &facility) == FAILURE) {
        return;
    }

    self = getThis();

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

    ASF_FUNC_REGISTER_SHUTDOWN_FUNCTION_CLOSE(self, 1);
}
/* }}} */

/* {{{ proto object Asf_Log_Adapter_Syslog::getFormatter(void)
*/
PHP_METHOD(asf_log_adapter_syslog, getFormatter)
{
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool Asf_Log_Adapter_Syslog::doLog(string $message)
*/
PHP_METHOD(asf_log_adapter_syslog, doLog)
{
    zend_string *message = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &message) == FAILURE) {
        return;
    }

    php_syslog(LOG_INFO, "%s", ZSTR_VAL(message));

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
