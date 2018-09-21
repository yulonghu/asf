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
#include "kernel/asf_namespace.h"
#include "asf_exception.h"
#include "asf_log_formatter_file.h"
#include "log/asf_log_formatterinterface.h"

zend_class_entry *asf_log_formatter_file_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_log_formatter_file_construct_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, format)
    ZEND_ARG_INFO(0, date_format)
ZEND_END_ARG_INFO()
/* }}}*/

/* {{{ proto string Asf_Log_Formatter_File::format(string $level, int $time, string $message)
*/
PHP_METHOD(asf_log_formatter_file, format)
{
    zval *level = NULL, *message = NULL;
    zend_long time = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "zlz", &level, &time, &message) == FAILURE) {
        return;
    }

    char *str_df = "d-M-Y H:i:s e", *str_ret = NULL;
    zend_string *str_date = NULL, *str_msg = NULL;
    zval *self = getThis();
    size_t str_ret_len = 0;

    str_date = php_format_date(str_df, strlen(str_df), time, 1);
    str_msg = zval_get_string(message);
    str_ret_len = spprintf(&str_ret, 0, "%s [%s] %s %s", str_date->val, Z_STRVAL_P(level), ZSTR_VAL(str_msg), PHP_EOL); 
    
    zend_string_release(str_msg);
    zend_string_release(str_date);

    ZVAL_STRINGL(return_value, str_ret, str_ret_len);
    efree(str_ret);
}
/* }}} */

/* {{{ asf_log_formatter_file_methods[]
*/
zend_function_entry asf_log_formatter_file_methods[] = {
    PHP_ME(asf_log_formatter_file, format, asf_log_formatterinterface_f_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(log_formatter_file) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_log_formatter_file, Asf_Log_Formatter_File, Asf\\Log\\Formatter\\File, ZEND_ACC_FINAL);

    zend_class_implements(asf_log_formatter_file_ce, 1, asf_log_formatterinterface_ce);

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
