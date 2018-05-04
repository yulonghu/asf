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
#include "ext/standard/php_string.h"
#include "ext/date/php_date.h" /* php_format_date */

#include "php_asf.h"
#include "asf_namespace.h"
#include "asf_exception.h"
#include "asf_log_formatter_syslog.h"
#include "log/asf_log_formatterinterface.h"

zend_class_entry *asf_log_formatter_syslog_ce;

/* {{{ proto string Asf_Log_Formatter_Syslog::format(string $level, int $time, string $message)
*/
PHP_METHOD(asf_log_formatter_syslog, format)
{
    zval *level = NULL, *message = NULL;
    zend_long time = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "zlz", &level, &time, &message) == FAILURE) {
        return;
    }

    ZVAL_COPY(return_value, message);
}
/* }}} */

/* {{{ asf_log_formatter_syslog_methods[]
*/
zend_function_entry asf_log_formatter_syslog_methods[] = {
    PHP_ME(asf_log_formatter_syslog, format, asf_log_formatterinterface_f_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(log_formatter_syslog) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_log_formatter_syslog, Asf_Log_Formatter_Syslog, Asf\\Log\\Formatter\\Syslog, ZEND_ACC_FINAL);

    zend_class_implements(asf_log_formatter_syslog_ce, 1, asf_log_formatterinterface_ce);

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
