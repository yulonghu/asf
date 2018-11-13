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
#include "Zend/zend_interfaces.h" /* for zend_call_method_with_* */

#include "php_asf.h"
#include "kernel/asf_namespace.h"
#include "kernel/asf_func.h"
#include "asf_exception.h"

#include "cache/asf_cache_abstractadapter.h"
#include "cache/adapter/asf_cache_adapter_memcached.h"

zend_class_entry *asf_cache_adapter_memcached_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_cache_mcd_init_arginfo, 0, 0, 1)
	ZEND_ARG_ARRAY_INFO(0, options, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_mcd_has_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_mcd_get_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_mcd_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, expire)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_mcd_incr_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, step)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_mcd_decr_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, step)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_mcd_call_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, function_name)
    ZEND_ARG_ARRAY_INFO(0, arguments, 1)
ZEND_END_ARG_INFO()
/* }}}*/

/* {{{ proto object Asf_Cache_Adapter_Memcached::__construct(array $options)
*/
PHP_METHOD(asf_cache_adapter_memcached, __construct)
{
    zval *options = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &options) == FAILURE) {
        return;
    }

    zend_class_entry *driver = asf_find_driver("memcached", 9);
    if (UNEXPECTED(driver == NULL)) {
        return;
    }

    zval *host = NULL, *password = NULL, *timeout = NULL, *username = NULL;
    zval retval, memcached, opts;

    object_init_ex(&memcached, driver);
    array_init(&opts);
    HashTable *ht = Z_ARRVAL_P(options);

    /* Memcached::OPT_DISTRIBUTION == Memcached::DISTRIBUTION_CONSISTENT */
    add_index_long(&opts, 9, 1);

    /* Memcached::OPT_TCP_NODELAY == true */
    add_index_bool(&opts, 1, 1);

    /* Memcached::OPT_CONNECT_TIMEOUT == 1 */
    timeout = zend_hash_str_find(ht, "timeout", 7);
    if (timeout && Z_TYPE_P(timeout) == IS_LONG) {
        add_index_long(&opts, 14, Z_LVAL_P(timeout));
    } else {
        add_index_long(&opts, 14, 1000);
    }

    ASF_CALL_USER_FUNCTION_EX(&memcached, "setOptions", 10, &retval, 1, &opts);
    zval_ptr_dtor(&opts);

    /* servers */
    if (UNEXPECTED((host = zend_hash_str_find(ht, "host", 4)) == NULL)) {
        asf_trigger_error(ASF_ERR_CACHE_OPTIONS, "The options 'host' not found");
        return;
    }

    ASF_CALL_USER_FUNCTION_EX(&memcached, "addServers", 10, &retval, 1, host);
    if (Z_TYPE(retval) == IS_FALSE) {
        asf_trigger_error(ASF_ERR_CACHE_CONNECT, "Memcached addServers failed");
        return;
    }

    zval *self = getThis();

    zend_update_property(asf_cache_adapter_memcached_ce, self, ZEND_STRL(ASF_CACHE_PRONAME_HANDLER), &memcached);
    zend_update_property(asf_cache_adapter_memcached_ce, self, ZEND_STRL(ASF_CACHE_PRONAME_CONNECT_INFO), options);

    zval_ptr_dtor(&memcached);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto bool Asf_Cache_Adapter_Memcached::has(mixed $key)
*/
PHP_METHOD(asf_cache_adapter_memcached, has)
{
    zval *key = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &key) == FAILURE) {
        return;
    }

    zval retval;
    zval *memcached = zend_read_property(asf_cache_adapter_memcached_ce, getThis(),
            ZEND_STRL(ASF_CACHE_PRONAME_HANDLER), 1, NULL);
    ASF_CALL_USER_FUNCTION_EX(memcached, "get", "3", &retval, 1, key);

    RETURN_BOOL(&retval);
}
/* }}} */

/* {{{ proto mixed Asf_Cache_Adapter_Memcached::get(mixed $key)
*/
PHP_METHOD(asf_cache_adapter_memcached, get)
{
    zval *key = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &key) == FAILURE) {
        return;
    }

    (void)asf_cache_adapter_handler_req(getThis(), 1, key, "get", 3, return_value);
}
/* }}} */

/* {{{ proto bool Asf_Cache_Adapter_Memcached::set(mixed $key, mixed $value)
*/
PHP_METHOD(asf_cache_adapter_memcached, set)
{
    zval *key = NULL, *value = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &key, &value) == FAILURE) {
        return;
    }

    zval args[2];
    ZVAL_COPY_VALUE(&args[0], key);
    ZVAL_COPY_VALUE(&args[1], value);

    (void)asf_cache_adapter_handler_req(getThis(), 2, args, "set", 3, return_value);
}
/* }}} */

/* {{{ proto int Asf_Cache_Adapter_Memcached::incr(mixed $key [, uint $step = 1])
*/
PHP_METHOD(asf_cache_adapter_memcached, incr)
{
    zval *key = NULL, *step = NULL;
    zval tmp_step;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z", &key, &step) == FAILURE) {
        return;
    }

    if (step == NULL) {
        ZVAL_LONG(&tmp_step, 1);
        step = &tmp_step;
    }

    zval args[2];
    ZVAL_COPY_VALUE(&args[0], key);
    ZVAL_COPY_VALUE(&args[1], step);

    (void)asf_cache_adapter_handler_req(getThis(), 2, args, "increment", 9, return_value);
}
/* }}} */

/* {{{ proto int Asf_Cache_Adapter_Memcached::decr(mixed $key [, uint $step = 1])
*/
PHP_METHOD(asf_cache_adapter_memcached, decr)
{
    zval *key = NULL, *step = NULL;
    zval tmp_step;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z", &key, &step) == FAILURE) {
        return;
    }

    if (step == NULL) {
        ZVAL_LONG(&tmp_step, 1);
        step = &tmp_step;
    }

    zval args[2];
    ZVAL_COPY_VALUE(&args[0], key);
    ZVAL_COPY_VALUE(&args[1], step);

    (void)asf_cache_adapter_handler_req(getThis(), 2, args, "decrement", 9, return_value);
}
/* }}} */

/* {{{ proto bool Asf_Cache_Adapter_Memcached::del(mixed $key)
*/
PHP_METHOD(asf_cache_adapter_memcached, del)
{
    zval *key = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &key) == FAILURE) {
        return;
    }

    (void)asf_cache_adapter_handler_req(getThis(), 1, key, "delete", 6, return_value);
}
/* }}} */

/* {{{ proto bool Asf_Cache_Adapter_Memcached::clear(void)
*/
PHP_METHOD(asf_cache_adapter_memcached, clear)
{
    zval *memcached = zend_read_property(asf_cache_adapter_memcached_ce, getThis(), ZEND_STRL(ASF_CACHE_PRONAME_HANDLER), 1, NULL);
    ASF_CALL_USER_FUNCTION_EX(memcached, "flush", 5, return_value, 0, NULL);
}
/* }}} */

/* {{{ proto mixed Asf_Cache_Adapter_Memcached::__call(string $function_name, array $args)
*/
PHP_METHOD(asf_cache_adapter_memcached, __call)
{
    zval *function_name = NULL;
    zval *args = NULL, *real_args = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "za", &function_name, &args) == FAILURE) {
        return;
    }

    zval *handler = zend_read_property(asf_cache_adapter_memcached_ce, getThis(), ZEND_STRL(ASF_CACHE_PRONAME_HANDLER), 1, NULL);
    if (UNEXPECTED(Z_TYPE_P(handler) != IS_OBJECT)) {
        RETURN_FALSE;   
    }

    size_t arg_count = 0;
    (void)asf_func_format_args(args, &real_args, &arg_count);

    call_user_function_ex(&Z_OBJCE_P(handler)->function_table, handler, function_name, return_value, arg_count, real_args, 1, NULL);
    if (arg_count > 0) {
        efree(real_args);
    }
}
/* }}} */

/* {{{ asf_cache_adapter_memcached_methods[]
*/
zend_function_entry asf_cache_adapter_memcached_methods[] = {
    PHP_ME(asf_cache_adapter_memcached, __construct, asf_cache_init_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(asf_cache_adapter_memcached, has, asf_cache_has_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_memcached, get, asf_cache_get_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_memcached, set, asf_cache_set_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_memcached, del, asf_cache_del_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_memcached, incr, asf_cache_incr_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_memcached, decr, asf_cache_decr_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_memcached, __call, asf_cache_call_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_memcached, clear,  NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(cache_adapter_memcached) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERNAL(asf_cache_adapter_memcached, Asf_Cache_Adapter_Memcached, Asf\\Cache\\Adapter\\Memcached, asf_cache_absadapter_ce, asf_cache_adapter_memcached_methods);

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
