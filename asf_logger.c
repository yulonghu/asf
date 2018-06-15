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

#include "php_asf.h"
#include "asf_namespace.h"
#include "asf_exception.h"
#include "kernel/asf_func.h"

#include "asf_logger.h"
#include "log/asf_log_level.h"
#include "log/asf_log_loggerinterface.h"

#include "log/asf_log_adapter.h"
#include "log/adapter/asf_log_adapter_file.h"
#include "log/adapter/asf_log_adapter_syslog.h"

#include "log/asf_log_formatterinterface.h"
#include "log/formatter/asf_log_formatter_file.h"
#include "log/formatter/asf_log_formatter_syslog.h"

zend_class_entry *asf_logger_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_logger_adapter_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, adapter)
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
    zend_long adapter = 0;
    zend_string *file_name = NULL;
    zval *self = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "lS", &adapter, &file_name) == FAILURE) {
        return;
    }

    /* The empty filename is meaningless */
    if (asf_func_isempty(ZSTR_VAL(file_name))) {
        RETURN_FALSE;
    }

    switch (adapter) {
        case 0:
            (void)asf_logger_instance(return_value, file_name, NULL);
            break;
        case 1:
            object_init_ex(return_value, asf_log_adapter_syslog_ce);
            break;
        default:
            RETURN_FALSE;
            break;
    }
}
/* }}} */

/* {{{ asf_logger_methods[]
*/
zend_function_entry asf_logger_methods[] = {
    PHP_ME(asf_logger, adapter, asf_logger_adapter_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(logger) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_logger, Asf_Logger, Asf\\Logger, ZEND_ACC_FINAL);

    ASF_INIT(log_level);
    ASF_INIT(log_loggerinterface);
    ASF_INIT(log_adapter);
    ASF_INIT(log_adapter_file);
    ASF_INIT(log_adapter_syslog);
    ASF_INIT(log_formatterinterface);
    ASF_INIT(log_formatter_file);
    ASF_INIT(log_formatter_syslog);

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
