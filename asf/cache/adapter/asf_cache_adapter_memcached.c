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

    zval *host = NULL, *timeout = NULL, *prefix = NULL; // *username = NULL, *password = NULL;
    zval retval, memcached, opts;

    object_init_ex(&memcached, driver);
    zend_call_method_with_0_params(&memcached, Z_OBJCE(memcached), NULL, "__construct", NULL);

    array_init(&opts);
    HashTable *ht = Z_ARRVAL_P(options);

    /* Memcached::OPT_DISTRIBUTION == Memcached::DISTRIBUTION_CONSISTENT */
    add_index_long(&opts, 9, 1);

    /* Memcached::OPT_LIBKETAMA_COMPATIBLE == TRUE */
    add_index_long(&opts, 16, 1);

    /* Memcached::OPT_TCP_NODELAY == true */
    add_index_bool(&opts, 1, 1);

    /* Memcached::OPT_CONNECT_TIMEOUT == 1 */
    timeout = zend_hash_str_find(ht, "timeout", 7);
    if (timeout && Z_TYPE_P(timeout) == IS_LONG) {
        add_index_long(&opts, 14, Z_LVAL_P(timeout));
    }

    /* Memcached::OPT_PREFIX_KEY */
    prefix = zend_hash_str_find(ht, "prefix", 6);
    if (prefix) {
        Z_TRY_ADDREF_P(prefix);
        add_index_zval(&opts, -1002, prefix);
    }

    ASF_CALL_USER_FUNCTION_EX(&memcached, "setoptions", 10, &retval, 1, &opts);
    zval_ptr_dtor(&opts);

    /* servers */
    if (UNEXPECTED((host = zend_hash_str_find(ht, "host", 4)) == NULL)) {
        asf_trigger_error(ASF_ERR_CACHE_OPTIONS, "The options 'host' not found");
        return;
    }
    
    /* trace log */
    double start_time = asf_func_trace_gettime();
    ASF_CALL_USER_FUNCTION_EX(&memcached, "addservers", 10, &retval, 1, host);
    (void)asf_func_trace_str_add(ASF_TRACE_MEMCACHED, start_time, "addServers", 18, 1, host, &retval);

    if (Z_TYPE(retval) == IS_FALSE) {
        asf_trigger_error(ASF_ERR_CACHE_CONNECT, "Memcached addServers failed");
        return;
    }

    zval *self = getThis();

    zend_update_property(asf_cache_adapter_memcached_ce, self, ZEND_STRL(ASF_CACHE_PRONAME_HANDLER), &memcached);
    zend_update_property(asf_cache_adapter_memcached_ce, self, ZEND_STRL(ASF_FUNC_PRONAME_CONNECT_INFO), options);

    zval_ptr_dtor(&memcached);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto bool Asf_Cache_Adapter_Memcached::has(mixed $key)
*/
ASF_CACHE_ADAPTER_METHOD_HAS(ASF_TRACE_MEMCACHED, asf_cache_adapter_memcached, "get", 3);
/* }}} */

/* {{{ proto mixed Asf_Cache_Adapter_Memcached::get(mixed $key)
*/
ASF_CACHE_ADAPTER_METHOD_GET_DEL(ASF_TRACE_MEMCACHED, asf_cache_adapter_memcached, get, "get", 3);
/* }}} */

/* {{{ proto bool Asf_Cache_Adapter_Memcached::set(mixed $key, mixed $value [, int $expiration])
*/
ASF_CACHE_ADAPTER_METHOD_SET(ASF_TRACE_MEMCACHED, asf_cache_adapter_memcached, "set", 3);
/* }}} */

/* {{{ proto int Asf_Cache_Adapter_Memcached::incr(mixed $key [, uint $step = 1])
*/
ASF_CACHE_ADAPTER_METHOD_DECR_INCR(ASF_TRACE_MEMCACHED, asf_cache_adapter_memcached, incr, "increment", 9);
/* }}} */

/* {{{ proto int Asf_Cache_Adapter_Memcached::decr(mixed $key [, uint $step = 1])
*/
ASF_CACHE_ADAPTER_METHOD_DECR_INCR(ASF_TRACE_MEMCACHED, asf_cache_adapter_memcached, decr, "decrement", 9);
/* }}} */

/* {{{ proto mixed Asf_Cache_Adapter_Memcached::del(mixed $key)
*/
ASF_CACHE_ADAPTER_METHOD_GET_DEL(ASF_TRACE_MEMCACHED, asf_cache_adapter_memcached, del, "delete", 6);
/* }}} */

/* {{{ proto bool Asf_Cache_Adapter_Memcached::close(void)
*/
PHP_METHOD(asf_cache_adapter_memcached, close)
{
    double start_time = asf_func_trace_gettime();
    zval *memcached = zend_read_property(asf_cache_adapter_memcached_ce,
            getThis(), ZEND_STRL(ASF_CACHE_PRONAME_HANDLER), 1, NULL);

    ASF_CALL_USER_FUNCTION_EX(memcached, "quit", 4, return_value, 0, NULL);
    (void)asf_func_trace_str_add(ASF_TRACE_MEMCACHED, start_time, "quit", 4, 0, NULL, NULL);
}
/* }}} */

/* {{{ proto bool Asf_Cache_Adapter_Memcached::clear(void)
*/
PHP_METHOD(asf_cache_adapter_memcached, clear)
{
    double start_time = asf_func_trace_gettime();
    zval *memcached = zend_read_property(asf_cache_adapter_memcached_ce,
            getThis(), ZEND_STRL(ASF_CACHE_PRONAME_HANDLER), 1, NULL);
    
    ASF_CALL_USER_FUNCTION_EX(memcached, "flush", 5, return_value, 0, NULL);
    (void)asf_func_trace_str_add(ASF_TRACE_MEMCACHED, start_time, "flush", 5, 0, NULL, NULL);
}
/* }}} */

/* {{{ proto mixed Asf_Cache_Adapter_Memcached::__call(string $function_name, array $args)
*/
ASF_METHOD_CALL(ASF_TRACE_MEMCACHED, asf_cache_adapter_memcached, ASF_CACHE_PRONAME_HANDLER)
/* }}} */

/* {{{ asf_cache_adapter_memcached_methods[]
*/
zend_function_entry asf_cache_adapter_memcached_methods[] = {
    PHP_ME(asf_cache_adapter_memcached, __construct, asf_cache_init_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(asf_cache_adapter_memcached, has, asf_cache_has_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_memcached, get, asf_cache_get_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_memcached, del, asf_cache_get_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_memcached, set, asf_cache_set_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_memcached, incr, asf_cache_incr_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_memcached, decr, asf_cache_decr_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_memcached, __call, asf_cache_call_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_memcached, close,  NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_adapter_memcached, clear,  NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(cache_adapter_memcached) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERNAL(asf_cache_adapter_memcached, Asf_Cache_Adapter_Memcached,
            Asf\\Cache\\Adapter\\Memcached, asf_cache_absadapter_ce, asf_cache_adapter_memcached_methods);

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
