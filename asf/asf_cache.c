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
#include "asf_cache.h"
#include "asf_exception.h"

#include "cache/asf_cache_abstractadapter.h"
#include "cache/adapter/asf_cache_adapter_redis.h"
#include "cache/adapter/asf_cache_adapter_memcached.h"

zend_class_entry *asf_cache_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_cache_setconf_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, configs, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_store_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_close_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ proto array Asf_Cache::getConfig(void)
*/
PHP_METHOD(asf_cache, getConfig)
{
    ZVAL_COPY(return_value,
            zend_read_static_property(asf_cache_ce, ZEND_STRL(ASF_CACHE_PRONAME_CONF), 1));
}
/* }}} */

/* {{{ proto bool Asf_Cache::setConfig(array $config)
*/
PHP_METHOD(asf_cache, setConfig)
{
    zval *config = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &config) == FAILURE) {
        return;
    }

    zend_update_static_property(asf_cache_ce, ZEND_STRL(ASF_CACHE_PRONAME_CONF), config);
    RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed Asf_Cache::getLinks(void)
*/
PHP_METHOD(asf_cache, getLinks)
{
    ZVAL_COPY(return_value,
            zend_read_static_property(asf_cache_ce, ZEND_STRL(ASF_CACHE_PRONAME_INS), 1));
}
/* }}} */

/* {{{ proto object Asf_Cache::store(string $name)
*/
PHP_METHOD(asf_cache, store)
{
    zval *name = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &name) == FAILURE) {
        return;
    }

    zval *config = zend_read_static_property(asf_cache_ce, ZEND_STRL(ASF_CACHE_PRONAME_CONF), 1);
    if (UNEXPECTED(Z_TYPE_P(config) != IS_ARRAY)) {
        asf_trigger_error(ASF_ERR_CACHE_OTHERS, "Please Cache::setConfig() on the first");
        return;
    }

    if (Z_TYPE_P(name) != IS_STRING && Z_TYPE_P(name) != IS_LONG) {
        asf_trigger_error(ASF_ERR_CACHE_OTHERS, "Parameter 'name' only support int and string");
        return;
    }

    zend_string *tmp_name = zval_get_string(name);
    zval *find_conf = zend_symtable_find(Z_ARRVAL_P(config), tmp_name);
    if (UNEXPECTED(find_conf == NULL)) {
        zend_string_release(tmp_name);
        asf_trigger_error(ASF_ERR_CACHE_OTHERS, "Parameter 'name' not found in Cache::$_config[]");
        return;
    }

    /* found name in Cache::$_ins[name] */
    zval *ins = zend_read_static_property(asf_cache_ce, ZEND_STRL(ASF_CACHE_PRONAME_INS), 1);
    zval *find = NULL;
    if (Z_TYPE_P(ins) != IS_NULL && (find = zend_symtable_find(Z_ARRVAL_P(ins), tmp_name))) {
        zval ping_ret;
        zval *handler = zend_read_property(Z_OBJCE_P(find), find, ZEND_STRL(ASF_CACHE_PRONAME_HANDLER), 1, NULL);
        zend_call_method_with_0_params(handler, Z_OBJCE_P(handler), NULL, "ping", &ping_ret);
        zval_ptr_dtor(&ping_ret);
        
        /* Throws a RedisException object on connectivity error */
        if (!EG(exception)) {
            RETURN_ZVAL(find, 1, 0);
        } else {
            /* Reconnect Redis once */
            zend_hash_del(Z_ARRVAL_P(ins), tmp_name);
            zend_clear_exception();
        }
        zend_string_release(tmp_name);
    }

    /* found name.type */
    if (UNEXPECTED(((find = zend_hash_str_find(Z_ARRVAL_P(find_conf), "type", 4)) == NULL) || Z_TYPE_P(find) != IS_STRING)) {
        zend_string_release(tmp_name);
        asf_trigger_error(ASF_ERR_CACHE_OTHERS, "'type.type' not found in Cache::$_config[]");
        return;
    }

    const char *str = Z_STRVAL_P(find);
    zval connect;

    do {
        if (strncasecmp("redis", str, 5) == 0) {
            object_init_ex(&connect, asf_cache_adapter_redis_ce);
            zend_call_method_with_1_params(&connect, asf_cache_adapter_redis_ce, NULL, "__construct", return_value, find_conf);
            zval_ptr_dtor(&connect);
            ZVAL_UNDEF(&connect);
            break;
        }

        if (strncasecmp("memcached", str, 9) == 0) {
            object_init_ex(&connect, asf_cache_adapter_memcached_ce);
            zend_call_method_with_1_params(&connect, asf_cache_adapter_memcached_ce, NULL, "__construct", return_value, find_conf);
            zval_ptr_dtor(&connect);
            ZVAL_UNDEF(&connect);
        }
    } while (0);

    if (Z_TYPE_P(return_value) == IS_OBJECT) {
        if (Z_ISNULL_P(ins)) {
            array_init(&connect);
            add_assoc_zval_ex(&connect, ZSTR_VAL(tmp_name), ZSTR_LEN(tmp_name), return_value);
            zend_update_static_property(asf_cache_ce, ZEND_STRL(ASF_CACHE_PRONAME_INS), &connect);
            zval_ptr_dtor(&connect);

            /* close redis_sock */
            ASF_FUNC_REGISTER_SHUTDOWN_FUNCTION(asf_cache_ce->name, "closeAll", 8);
        } else {
            zend_hash_add(Z_ARRVAL_P(ins), tmp_name, return_value);
            zend_update_static_property(asf_cache_ce, ZEND_STRL(ASF_CACHE_PRONAME_INS), ins);
        }
        Z_TRY_ADDREF_P(return_value);
    }

    zend_string_release(tmp_name);
}
/* }}} */

/* {{{ proto bool Asf_Cache::close(string $name)
*/
PHP_METHOD(asf_cache, close)
{
    zval *name = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &name) == FAILURE) {
        return;
    }

    zend_string *tmp_name = zval_get_string(name);

    /* found name in Cache::$_ins[name] */
    zval *ins = zend_read_static_property(asf_cache_ce, ZEND_STRL(ASF_CACHE_PRONAME_INS), 1);
    zval *find = NULL;

    if (Z_TYPE_P(ins) != IS_NULL && (find = zend_symtable_find(Z_ARRVAL_P(ins), tmp_name))) {
        ASF_CALL_USER_FUNCTION_EX(find, "close", 5, return_value, 0, NULL);
        zend_hash_del(Z_ARRVAL_P(ins), tmp_name);
        zend_string_release(tmp_name);
        return;
    }
    zend_string_release(tmp_name);

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool Asf_Cache::closeAll(string $name)
*/
PHP_METHOD(asf_cache, closeAll)
{
    /* found name in Cache::$_ins[name] */
    zval *ins = zend_read_static_property(asf_cache_ce, ZEND_STRL(ASF_CACHE_PRONAME_INS), 1);
    zval *find = NULL;

    if (Z_TYPE_P(ins) == IS_NULL) {
        RETURN_FALSE;
    }

    zval *entry = NULL;

    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(ins), entry) {
        ZVAL_DEREF(entry);
        ASF_CALL_USER_FUNCTION_EX(entry, "close", 5, return_value, 0, NULL);
    } ZEND_HASH_FOREACH_END();

    zend_update_static_property_null(asf_cache_ce, ZEND_STRL(ASF_CACHE_PRONAME_INS));
}
/* }}} */

/* {{{ asf_cache_methods[]
*/
zend_function_entry asf_cache_methods[] = {
    PHP_ME(asf_cache, getConfig,    NULL,                       ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_cache, setConfig,    asf_cache_setconf_arginfo,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_cache, getLinks,     NULL,                       ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_cache, store,        asf_cache_store_arginfo,    ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_cache, close,        asf_cache_close_arginfo,    ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_cache, closeAll,     NULL,                       ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(cache) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_cache, Asf_Cache, Asf\\Cache, ZEND_ACC_FINAL);

    zend_declare_property_null(asf_cache_ce, ZEND_STRL(ASF_CACHE_PRONAME_CONF), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
    zend_declare_property_null(asf_cache_ce, ZEND_STRL(ASF_CACHE_PRONAME_INS), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);

    ASF_INIT(cache_absadapter);
    ASF_INIT(cache_adapter_redis);
    ASF_INIT(cache_adapter_memcached);

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
