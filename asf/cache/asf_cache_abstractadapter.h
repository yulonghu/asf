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

#ifndef ASF_CACHE_ABSADAPTER_H
#define ASF_CACHE_ABSADAPTER_H

#define ASF_CACHE_PRONAME_HANDLER "_handler"
#define ASF_CACHE_PRONAME_CONSUME "_consume"
#define ASF_CACHE_PRONAME_CONNECT_INFO "_connect_info"

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_cache_init_arginfo, 0, 0, 1)
	ZEND_ARG_ARRAY_INFO(0, options, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_has_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_get_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, expiration)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_incr_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, step)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_decr_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, step)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_call_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, function_name)
    ZEND_ARG_ARRAY_INFO(0, arguments, 1)
ZEND_END_ARG_INFO()
/* }}} */

void asf_cache_adapter_handler_req(zval *self, uint32_t param_count, zval params[],
        char *method, uint method_len, zval *retval);

#define ASF_CACHE_ADAPTER_METHOD_HAS(name, func_name, func_name_len) /*{{{*/ \
PHP_METHOD(name, has) \
{ \
    zval *key = NULL, retval; \
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &key) == FAILURE) { \
        return; \
    } \
    (void)asf_cache_adapter_handler_req(getThis(), 1, key, func_name, func_name_len, &retval); \
    RETURN_BOOL((Z_TYPE(retval) == IS_FALSE) ? 0 : 1); \
} \
/*}}}*/

#define ASF_CACHE_ADAPTER_METHOD_GET_DEL(name, method_name, func_name, func_name_len) /*{{{*/ \
PHP_METHOD(name, method_name) \
{ \
    zval *key = NULL; \
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &key) == FAILURE) { \
        return; \
    } \
    (void)asf_cache_adapter_handler_req(getThis(), 1, key, func_name, func_name_len, return_value); \
} \
/*}}}*/

#define ASF_CACHE_ADAPTER_METHOD_SET(name, func_name, func_name_len) /*{{{*/ \
PHP_METHOD(name, set) \
{ \
    zval *key = NULL, *value = NULL, *expiration = NULL; \
    int num_args = ZEND_NUM_ARGS(); \
    if (zend_parse_parameters(num_args, "zz|z", &key, &value, &expiration) == FAILURE) { \
        return; \
    } \
    zval args[3]; \
    ZVAL_COPY_VALUE(&args[0], key); \
    ZVAL_COPY_VALUE(&args[1], value); \
    if (expiration) { \
        ZVAL_COPY_VALUE(&args[2], expiration); \
    } \
    (void)asf_cache_adapter_handler_req(getThis(), num_args, args, func_name, func_name_len, return_value); \
} \
/*}}}*/

#define ASF_CACHE_ADAPTER_METHOD_DECR_INCR(name, method_name, func_name, func_name_len) /*{{{*/ \
PHP_METHOD(name, method_name) \
{ \
    zval *key = NULL, *step = NULL; \
    zval tmp_step; \
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z", &key, &step) == FAILURE) { \
        return; \
    } \
    if (step == NULL) { \
        ZVAL_LONG(&tmp_step, 1); \
        step = &tmp_step; \
    } \
    zval args[2]; \
    ZVAL_COPY_VALUE(&args[0], key); \
    ZVAL_COPY_VALUE(&args[1], step); \
    (void)asf_cache_adapter_handler_req(getThis(), 2, args, func_name, func_name_len, return_value); \
} \
/*}}}*/

#define ASF_CACHE_ADAPTER_METHOD_CALL(name) /*{{{*/ \
PHP_METHOD(name, __call) \
{ \
    zval *function_name = NULL; \
    zval *args = NULL, *real_args = NULL; \
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "za", &function_name, &args) == FAILURE) { \
        return; \
    } \
    zval *handler = zend_read_property(name ## _ce, getThis(), ZEND_STRL(ASF_CACHE_PRONAME_HANDLER), 1, NULL); \
    if (UNEXPECTED(Z_TYPE_P(handler) != IS_OBJECT)) { \
        RETURN_FALSE; \
    } \
    size_t arg_count = 0; \
    (void)asf_func_format_args(args, &real_args, &arg_count); \
    /* trace log */ \
    double start_time = 0.0; \
    if (ASF_G(trace_enable)) { \
        start_time = asf_func_gettimeofday(); \
    } \
    call_user_function_ex(&Z_OBJCE_P(handler)->function_table, handler, function_name, return_value, arg_count, real_args, 1, NULL); \
    if (ASF_G(trace_enable)) { /*{{{*/ \
        double exec_time = (double)((asf_func_gettimeofday() - start_time)); \
        if (Z_TYPE(ASF_G(trace_buf)) != IS_ARRAY) { \
            array_init(&ASF_G(trace_buf)); \
        } \
        zval line; \
        array_init(&line); \
        Z_TRY_ADDREF_P(return_value); \
        Z_TRY_ADDREF_P(function_name); \
        Z_TRY_ADDREF_P(args); \
        add_assoc_zval_ex(&line, "s", 1, function_name); \
        add_assoc_zval_ex(&line, "v", 1, args); \
        add_assoc_double_ex(&line, "t", 1, exec_time); \
        add_assoc_zval_ex(&line, "r", 1, return_value); \
        add_next_index_zval(&ASF_G(trace_buf), &line); \
    }/*}}}*/ \
    if (arg_count > 0) { \
        efree(real_args); \
    } \
} \
/*}}}*/

extern zend_class_entry *asf_cache_absadapter_ce;

ASF_INIT_CLASS(cache_absadapter);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
