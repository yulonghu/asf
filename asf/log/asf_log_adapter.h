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

#ifndef ASF_LOG_ADAPTER_H
#define ASF_LOG_ADAPTER_H

#define ASF_LOG_ADAPTER_METHOD(mname, mname_upper, mname_upper_len) \
PHP_METHOD(asf_log_adapter, mname) \
{ \
    zend_string *message = NULL; zval *context = NULL; \
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|a", &message, &context) == FAILURE) { \
        return; \
    } \
    zval zmn_1, args[3]; \
    ZVAL_STRINGL(&zmn_1, "log", 3); \
    ZVAL_STRINGL(&args[0], mname_upper, mname_upper_len); \
    ZVAL_STR(&args[1], message); \
    if (context) { \
        ZVAL_COPY_VALUE(&args[2], context); \
        call_user_function_ex(&(Z_OBJCE_P(getThis()))->function_table, getThis(), &zmn_1, return_value, 3, args, 1, NULL); \
    } else { \
        call_user_function_ex(&(Z_OBJCE_P(getThis()))->function_table, getThis(), &zmn_1, return_value, 2, args, 1, NULL); \
    } \
    zval_ptr_dtor(&zmn_1); \
    zval_ptr_dtor(&args[0]); \
}

/* PHP 7.2 zend_class_constant */
#ifndef zend_class_constant
#define zend_class_constant zend_constant
#endif

extern zend_class_entry *asf_log_adapter_ce;

ASF_INIT_CLASS(log_adapter);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
