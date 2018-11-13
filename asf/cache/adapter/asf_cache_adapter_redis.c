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
#include "cache/adapter/asf_cache_adapter_redis.h"

zend_class_entry *asf_cache_adapter_redis_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_cache_redis_init_arginfo, 0, 0, 1)
	ZEND_ARG_ARRAY_INFO(0, options, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_redis_has_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_redis_get_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_redis_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, expire)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_redis_incr_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, step)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_redis_decr_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, step)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_redis_call_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, function_name)
    ZEND_ARG_ARRAY_INFO(0, arguments, 1)
ZEND_END_ARG_INFO()
/* }}}*/

/* {{{ proto object Asf_Cache_Adapter_Redis::__construct(array $options)
*/
PHP_METHOD(asf_cache_adapter_redis, __construct)
{
    zval *options = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &options) == FAILURE) {
        return;
    }

    zval *host = NULL, *port = NULL, *password = NULL, *timeout = NULL, *select = NULL, *persistent = NULL;
    zend_long l_port = 0, l_timeout = 1, l_select = 0;
    zval retval, args[4], redis;

    HashTable *ht = Z_ARRVAL_P(options);

    if (UNEXPECTED((host = zend_hash_str_find(ht, "host", 4)) == NULL)) {
        asf_trigger_error(ASF_ERR_CACHE_OPTIONS, "The options 'host' not found");
        return;
    }

    /* port = 6379 OR port = '6379', we don't check less than 1 */
    port = zend_hash_str_find(ht, "port", 4);
    l_port = (port != NULL) ? zval_get_long(port) : 6379;

    /* timeout = 1 (default), we don't check less than 1 */
    timeout = zend_hash_str_find(ht, "timeout", 7);
    l_timeout = (timeout != NULL) ? zval_get_long(timeout) : 1;

    /* select DB = 0 (default), we don't check less than 0 */
    select = zend_hash_str_find(ht, "select", 6);
    l_select = (select != NULL) ? zval_get_long(select) : 0;

    zend_class_entry *driver = asf_find_driver("redis", 5);
    if (UNEXPECTED(driver == NULL)) {
        return;
    }

    object_init_ex(&redis, driver);

    ZVAL_COPY_VALUE(&args[0], host);
    ZVAL_LONG(&args[1], l_port);
    ZVAL_LONG(&args[2], l_timeout);
    ZVAL_LONG(&args[3], l_select);

    persistent = zend_hash_str_find(ht, "persistent", 10);

    if (persistent && zend_is_true(persistent)) {
        char persistent_id[7] = {0};
        sprintf(persistent_id, "%s%d", "id_", l_select);
        ZVAL_STRING(&args[3], persistent_id);
        ASF_CALL_USER_FUNCTION_EX(&redis, "pconnect", 8, &retval, 4, args);
        ASF_FAST_STRING_PTR_DTOR(args[3]);
    } else {
        ASF_CALL_USER_FUNCTION_EX(&redis, "connect", 7, &retval, 3, args);
    }

    if (Z_TYPE(retval) == IS_FALSE) {
        zval_ptr_dtor(&redis);
        asf_trigger_error(ASF_ERR_CACHE_OTHERS, "Connection failed, %s:"ZEND_LONG_FMT"", Z_STRVAL_P(host), l_port);
        return;
    }

    password = zend_hash_str_find(ht, "password", 8);
    if (UNEXPECTED(password != NULL)) {
        zend_call_method_with_1_params(&redis, Z_OBJCE(redis), NULL, "auth", &retval, password);
        if (Z_TYPE(retval) == IS_FALSE) {
            zval_ptr_dtor(&redis);
            asf_trigger_error(ASF_ERR_CACHE_AUTH, "Authentication failed, %s:"ZEND_LONG_FMT"", Z_STRVAL_P(host), l_port);
            return;
        }
    }

    if (l_select > 0) {
        zend_call_method_with_1_params(&redis, Z_OBJCE(redis), NULL, "select", &retval, select);
        if (Z_TYPE(retval) == IS_FALSE) {
            zval_ptr_dtor(&redis);
            asf_trigger_error(ASF_ERR_CACHE_OTHERS, "Select DB failed, %s:"ZEND_LONG_FMT"", Z_STRVAL_P(host), l_port);
            return;
        }
    }

    zval *self = getThis();

    zend_update_property(asf_cache_adapter_redis_ce, self, ZEND_STRL(ASF_CACHE_PRONAME_HANDLER), &redis);
    zend_update_property(asf_cache_adapter_redis_ce, self, ZEND_STRL(ASF_CACHE_PRONAME_CONNECT_INFO), options);

    zval_ptr_dtor(&redis);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto bool Asf_Cache_Adapter_Redis::has(mixed $key)
*/
PHP_METHOD(asf_cache_adapter_redis, has)
{
    zval *key = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &key) == FAILURE) {
        return;
    }

    (void)asf_cache_adapter_handler_req(getThis(), 1, key, "exists", 6, return_value);
}
/* }}} */

/* {{{ proto mixed Asf_Cache_Adapter_Redis::get(mixed $key)
*/
PHP_METHOD(asf_cache_adapter_redis, get)
{
    zval *key = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &key) == FAILURE) {
        return;
    }

    (void)asf_cache_adapter_handler_req(getThis(), 1, key, "get", 3, return_value);
}
/* }}} */

/* {{{ proto bool Asf_Cache_Adapter_Redis::set(mixed $key, mixed $value)
*/
PHP_METHOD(asf_cache_adapter_redis, set)
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

/* {{{ proto int Asf_Cache_Adapter_Redis::incr(mixed $key [, uint $step = 1])
*/
PHP_METHOD(asf_cache_adapter_redis, incr)
{
    zval *key = NULL, *step = NULL, *redis = NULL;
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

    (void)asf_cache_adapter_handler_req(getThis(), 2, args, "incrby", 6, return_value);
}
/* }}} */

/* {{{ proto int Asf_Cache_Adapter_Redis::decr(mixed $key [, uint $step = 1])
*/
PHP_METHOD(asf_cache_adapter_redis, decr)
{
    zval *key = NULL, *step = NULL, *redis = NULL;
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

    (void)asf_cache_adapter_handler_req(getThis(), 2, args, "decrby", 6, return_value);
}
/* }}} */

/* {{{ proto bool Asf_Cache_Adapter_Redis::clear(void)
*/
PHP_METHOD(asf_cache_adapter_redis, clear)
{
    zval *redis = zend_read_property(asf_cache_adapter_redis_ce, getThis(), ZEND_STRL(ASF_CACHE_PRONAME_HANDLER), 1, NULL);
    ASF_CALL_USER_FUNCTION_EX(redis, "flushDB", 7, return_value, 0, NULL);
}
/* }}} */

/* {{{ proto mixed Asf_Cache_Adapter_Redis::__call(string $function_name, array $args)
*/
PHP_METHOD(asf_cache_adapter_redis, __call)
{
    zval *function_name = NULL;
    zval *args = NULL, *real_args = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "za", &function_name, &args) == FAILURE) {
        return;
    }

    zval *redis = zend_read_property(asf_cache_adapter_redis_ce, getThis(), ZEND_STRL(ASF_CACHE_PRONAME_HANDLER), 1, NULL);
    if (UNEXPECTED(Z_TYPE_P(redis) != IS_OBJECT)) {
        RETURN_FALSE;   
    }

    size_t arg_count = 0;
    (void)asf_func_format_args(args, &real_args, &arg_count);

    call_user_function_ex(&Z_OBJCE_P(redis)->function_table, redis, function_name, return_value, arg_count, real_args, 1, NULL);
    if (arg_count > 0) {
        efree(real_args);
    }
}
/* }}} */

/* {{{ asf_cache_adapter_redis_methods[]
*/
zend_function_entry asf_cache_adapter_redis_methods[] = {
    PHP_ME(asf_cache_adapter_redis, __construct, asf_cache_init_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(asf_cache_adapter_redis, has, asf_cache_has_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_redis, get, asf_cache_get_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_redis, set, asf_cache_set_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_redis, incr, asf_cache_incr_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_redis, decr, asf_cache_decr_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_redis, __call, asf_cache_call_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_redis, clear,  NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(cache_adapter_redis) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERNAL(asf_cache_adapter_redis, Asf_Cache_Adapter_Redis, Asf\\Cache\\Adapter\\Redis, asf_cache_absadapter_ce, asf_cache_adapter_redis_methods);

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
