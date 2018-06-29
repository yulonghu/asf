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
#include "Zend/zend_interfaces.h" /* zend_call_method* */

#include "php_asf.h"
#include "kernel/asf_namespace.h"
#include "asf_exception.h"
#include "kernel/asf_func.h"

#include "http/asf_http_request.h"
#include "http/asf_http_cookieinterface.h"
#include "http/asf_http_cookie.h"

zend_class_entry *asf_http_cookie_ce;

/* {{{ proto object Asf_Http_Cookie::__construct(void)
*/
PHP_METHOD(asf_http_cookie, __construct)
{
    zval *configs = NULL, *pzval = NULL, zconfig;
    HashTable *st = NULL, *sz = NULL;
    zend_ulong expire = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &configs) == FAILURE) {
        return;
    }

    st = Z_ARRVAL_P(configs);

    if (zend_hash_num_elements(st) < 1) {
        return;
    }

    array_init(&zconfig);
    sz = Z_ARRVAL(zconfig);

    if ((pzval = zend_hash_str_find(st, ASF_COOKIE_EXPIRE, ASF_COOKIE_EXPIRE_LEN)) && Z_TYPE_P(pzval) == IS_LONG) {
        expire = Z_LVAL_P(pzval);
    } else {
        expire = time(NULL);
    }
    add_assoc_long_ex(&zconfig, ASF_COOKIE_EXPIRE, ASF_COOKIE_EXPIRE_LEN, expire + 86400);

    if ((pzval = zend_hash_str_find(st, ASF_COOKIE_PATH, ASF_COOKIE_PATH_LEN)) && Z_TYPE_P(pzval) == IS_STRING) {
        add_assoc_stringl_ex(&zconfig, ASF_COOKIE_PATH, ASF_COOKIE_PATH_LEN, Z_STRVAL_P(pzval), Z_STRLEN_P(pzval));
    }

    if ((pzval = zend_hash_str_find(st, ASF_COOKIE_DOMAIN, ASF_COOKIE_DOMAIN_LEN)) && Z_TYPE_P(pzval) == IS_STRING) {
        add_assoc_stringl_ex(&zconfig, ASF_COOKIE_PATH, ASF_COOKIE_PATH_LEN, Z_STRVAL_P(pzval), Z_STRLEN_P(pzval));
    }
    
    if ((pzval = zend_hash_str_find(st, ASF_COOKIE_SECURE, ASF_COOKIE_SECURE_LEN)) && Z_TYPE_P(pzval) == IS_LONG) {
        add_assoc_long_ex(&zconfig, ASF_COOKIE_SECURE, ASF_COOKIE_SECURE_LEN, Z_LVAL_P(pzval));
    }
    
    if ((pzval = zend_hash_str_find(st, ASF_COOKIE_HTTPONLY, ASF_COOKIE_HTTPONLY_LEN))) {
        add_assoc_long_ex(&zconfig, ASF_COOKIE_HTTPONLY, ASF_COOKIE_HTTPONLY_LEN, Z_LVAL_P(pzval));
    }
    
    zend_update_property(asf_http_cookie_ce, getThis(), ZEND_STRL(ASF_COOKIE_PRONAME_CONFIG), &zconfig);
}
/* }}} */

/* {{{ proto bool Asf_Http_Cookie::prefix([string $prefix = ''])
*/
PHP_METHOD(asf_http_cookie, prefix)
{
    zend_string *key = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &key) == FAILURE) {
        return;
    }

    zend_update_property_str(asf_http_cookie_ce, getThis(), ZEND_STRL(ASF_COOKIE_PRONAME_PREFIX), key);

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool Asf_Http_Cookie::set(string $name, mixed $value)
*/
PHP_METHOD(asf_http_cookie, set)
{
    zval *key = NULL, *value = NULL;
    zval *self = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &key, &value) == FAILURE) {
        return;
    }

    self = getThis();
    
    zval *pzval = zend_read_property(asf_http_cookie_ce, self, ZEND_STRL(ASF_COOKIE_PRONAME_CONFIG), 1, NULL);
    if (Z_ISNULL_P(pzval)) {
        asf_trigger_error(ASF_ERR_COOKIE_CONFIG, "Please initialize 'cookie' first");
        return;
    }

    HashTable *ht = Z_ARRVAL_P(pzval);

    zval *path      = zend_hash_str_find(ht, ASF_COOKIE_PATH, ASF_COOKIE_PATH_LEN);
    zval *domain    = zend_hash_str_find(ht, ASF_COOKIE_DOMAIN, ASF_COOKIE_DOMAIN_LEN);
    zval *secure    = zend_hash_str_find(ht, ASF_COOKIE_SECURE, ASF_COOKIE_SECURE_LEN);
    zval *httponly  = zend_hash_str_find(ht, ASF_COOKIE_HTTPONLY, ASF_COOKIE_HTTPONLY_LEN);

    php_setcookie(Z_STR_P(key), Z_STR_P(value), 0, 
            (path ? Z_STR_P(path) : NULL),
            (domain ? Z_STR_P(domain) : NULL),
            (secure ? Z_LVAL_P(secure) : 0),
            1,
            (httponly ? Z_LVAL_P(httponly) : 0));

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool Asf_Http_Cookie::forever(string $name, string $value)
*/
PHP_METHOD(asf_http_cookie, forever)
{
}
/* }}} */

/* {{{ proto bool Asf_Http_Cookie::has(string $name)
*/
PHP_METHOD(asf_http_cookie, has)
{
    zval *key = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &key) == FAILURE) {
        return;
    }

    zval *cookie = asf_http_req_pg_find(TRACK_VARS_COOKIE);
    if (zend_hash_num_elements(Z_ARRVAL_P(cookie)) < 1) {
        RETURN_FALSE;
    }

    RETURN_BOOL(asf_func_array_isset(cookie, key));
}
/* }}} */

/* {{{ proto mixed Asf_Http_Cookie::get(string $name)
*/
PHP_METHOD(asf_http_cookie, get)
{
    zval *key = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &key) == FAILURE) {
        return;
    }

    zval *cookie = asf_http_req_pg_find(TRACK_VARS_COOKIE);
    if (zend_hash_num_elements(Z_ARRVAL_P(cookie)) < 1) {
        RETURN_FALSE;
    }

    zval *ret = asf_func_array_fetch(cookie, key);

    if (ret) {
        RETURN_ZVAL(ret, 1, 0);
    }
    
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool Asf_Http_Cookie::del(string $name [, string $...])
*/
PHP_METHOD(asf_http_cookie, del)
{
    zval *args = NULL;
    int argc = 0;
    uint i = 0;

    ZEND_PARSE_PARAMETERS_START(1, -1)
        Z_PARAM_VARIADIC('+', args, argc)
    ZEND_PARSE_PARAMETERS_END();

    zval *cookie = asf_http_req_pg_find(TRACK_VARS_COOKIE);
    if (zend_hash_num_elements(Z_ARRVAL_P(cookie)) < 1) {
        RETURN_FALSE;
    }

    zval *pzval = zend_read_property(asf_http_cookie_ce, getThis(), ZEND_STRL(ASF_COOKIE_PRONAME_CONFIG), 1, NULL);
    if (Z_ISNULL_P(pzval)) {
        asf_trigger_error(ASF_ERR_COOKIE_CONFIG, "Please initialize 'cookie' first");
        return;
    }

    HashTable *ht = Z_ARRVAL_P(pzval);

    zval *path      = zend_hash_str_find(ht, ASF_COOKIE_PATH, ASF_COOKIE_PATH_LEN);
    zval *domain    = zend_hash_str_find(ht, ASF_COOKIE_DOMAIN, ASF_COOKIE_DOMAIN_LEN);
    zval *secure    = zend_hash_str_find(ht, ASF_COOKIE_SECURE, ASF_COOKIE_SECURE_LEN);
    zval *httponly  = zend_hash_str_find(ht, ASF_COOKIE_HTTPONLY, ASF_COOKIE_HTTPONLY_LEN);

    for (i = 0; i < argc; i++) {
        asf_func_array_del(cookie, &args[i]);
        php_setcookie(Z_STR(args[i]), NULL, 0, 
                (path ? Z_STR_P(path) : NULL),
                (domain ? Z_STR_P(domain) : NULL),
                (secure ? Z_LVAL_P(secure) : 0),
                1,
                (httponly ? Z_LVAL_P(httponly) : 0));
    }

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool Asf_Http_Cookie::clear()
*/
PHP_METHOD(asf_http_cookie, clear)
{
    zval *cookie = asf_http_req_pg_find(TRACK_VARS_COOKIE);
    HashTable *ht = Z_ARRVAL_P(cookie);

    size_t argc = zend_hash_num_elements(ht);
    if (argc < 1) {
        RETURN_FALSE;
    }
    
    zval args[argc], zmn;
    zend_string *key = NULL;
    uint i = 0;
    zval *self = getThis();

    ZEND_HASH_FOREACH_STR_KEY(ht, key) {
        if (key) {
            ZVAL_STR_COPY(&args[i++], key);
        }
    } ZEND_HASH_FOREACH_END();

    ZVAL_STRINGL(&zmn, "del", 3); 
    call_user_function_ex(&Z_OBJCE_P(self)->function_table, self, &zmn, return_value, argc, args, 1, NULL);
    zend_string_release(Z_STR(zmn)); 
}
/* }}} */

/* {{{ asf_http_cookie_methods[]
*/
zend_function_entry asf_http_cookie_methods[] = {
    PHP_ME(asf_http_cookie, __construct,    asf_http_cookieinterface_init_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(asf_http_cookie, prefix,         asf_http_cookieinterface_prefix_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_cookie, set,            asf_http_cookieinterface_set_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_cookie, forever,        asf_http_cookieinterface_set_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_cookie, has,            asf_http_cookieinterface_has_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_cookie, get,            asf_http_cookieinterface_get_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_cookie, del,            asf_http_cookieinterface_del_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_cookie, clear,          NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(http_cookie) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_http_cookie, Asf_Http_Cookie, Asf\\HTTP\\Cookie, ZEND_ACC_FINAL);

    zend_declare_property_null(asf_http_cookie_ce, ZEND_STRL(ASF_COOKIE_PRONAME_CONFIG), ZEND_ACC_PROTECTED);
    zend_declare_property_null(asf_http_cookie_ce, ZEND_STRL(ASF_COOKIE_PRONAME_PREFIX), ZEND_ACC_PROTECTED);

    ASF_INIT(http_cookieinterface);

    zend_class_implements(asf_http_cookie_ce, 1, asf_http_cookieinterface_ce);

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
