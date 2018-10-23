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

/* Excerpt from the yaf, http://yaf.laruence.com/manual/ */
/* Excerpt from the yaconf, https://github.com/laruence/yaconf */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_asf.h"
#include "kernel/asf_namespace.h"
#include "asf_exception.h"

#include "asf_config_abstractconfig.h"
#include "asf_config_simple.h"
#include "asf_config_php.h"
#include "asf_config_ini.h"
#include "ext/standard/php_filestat.h" /* php_stat */

zend_class_entry *asf_absconfig_ce;

#define PMALLOC_HASHTABLE(ht) ((ht) = __zend_malloc(sizeof(HashTable)))

static void asf_absconfig_cache_hash_copy(HashTable *target, HashTable *source);

static inline _Bool asf_absconfig_cache_file_mtime(const char *filename, size_t filename_len, time_t time, _Bool check_expire) /* {{{ */
{
    zval mtime;

    (void)php_stat(filename, filename_len, FS_MTIME, &mtime);

    if (UNEXPECTED(Z_TYPE(mtime) != IS_LONG)) {
        return 0;
    }

    /* If get mtime successful, Then renewal Expire */
    if (!check_expire || ASF_G(cache_config_mtime) == Z_LVAL(mtime)) {
        ASF_G(cache_config_last_time) = time + ASF_G(cache_config_expire);
        ASF_G(cache_config_mtime) = Z_LVAL(mtime);
        return 1;
    }

    return 0;
}
/* }}} */

static zend_string *asf_absconfig_cache_str_persistent(const char *str, const size_t len) /* {{{ */
{
    /* Exclude Interned String */
    zend_string *new_key = zend_string_init(str, len, 1);

    if (UNEXPECTED(new_key == NULL)) {
        zend_out_of_memory();
    }

    return new_key;
}
/* }}} */

static void asf_absconfig_cache_zval_persistent(zval *zv, zval *rv) /* {{{ */
{
    switch (Z_TYPE_P(zv)) {
        case IS_CONSTANT:
        case IS_STRING:
            ZVAL_NEW_STR(rv, asf_absconfig_cache_str_persistent(Z_STRVAL_P(zv), Z_STRLEN_P(zv)));
            break;
        case IS_ARRAY:
            {
                ZVAL_NEW_PERSISTENT_ARR(rv);
                zend_hash_init(Z_ARRVAL_P(rv), zend_hash_num_elements(Z_ARRVAL_P(zv)), NULL, ZVAL_PTR_DTOR, 1);
                (void)asf_absconfig_cache_hash_copy(Z_ARRVAL_P(rv), Z_ARRVAL_P(zv));
            }
            break;
        case IS_RESOURCE:
        case IS_OBJECT:
            break;
        case IS_FALSE:
            ZVAL_FALSE(rv);
            break;
        case IS_TRUE:
            ZVAL_TRUE(rv);
            break;
        case IS_LONG:
            ZVAL_LONG(rv, Z_LVAL_P(zv));
            break;
        case IS_DOUBLE:
            ZVAL_DOUBLE(rv, Z_DVAL_P(zv));
            break;
        case IS_NULL:
            ZVAL_NULL(rv);
            break;
        default:
            break;
    }
}
/* }}} */

static void asf_absconfig_cache_hash_copy(HashTable *target, HashTable *source) /* {{{ */
{
    zend_string *key = NULL;
    zend_long idx = 0;
    zval *element = NULL, rv;

    ZEND_HASH_FOREACH_KEY_VAL(source, idx, key, element) {
        (void)asf_absconfig_cache_zval_persistent(element, &rv);
        if (key) {
            zend_hash_update(target, asf_absconfig_cache_str_persistent(ZSTR_VAL(key), ZSTR_LEN(key)), &rv);
        } else {
            zend_hash_index_update(target, idx, &rv);
        }
    } ZEND_HASH_FOREACH_END();
}
/* }}} */

static inline void asf_absconfig_cache_hash_symtable_update(HashTable *ht, zend_string *key, zval *zv) /* {{{ */
{
    zend_ulong idx = 0;
    zval rv;

    ZVAL_NEW_PERSISTENT_ARR(&rv);
    zend_hash_init(Z_ARRVAL(rv), zend_hash_num_elements(Z_ARRVAL_P(zv)), NULL, ZVAL_PTR_DTOR, 1);

    (void)asf_absconfig_cache_hash_copy(Z_ARRVAL(rv), Z_ARRVAL_P(zv));
    zend_hash_update(ASF_G(cache_config_buf), asf_absconfig_cache_str_persistent(ZSTR_VAL(key), ZSTR_LEN(key)), &rv);
}
/* }}} */

zval *asf_absconfig_instance(zval *this_ptr, zval *arg1, zval *arg2) /* {{{ */
{
    zval *instance = NULL, *pzval = NULL;
    time_t cur_time = time(NULL);
    zend_uchar uc_arg1 = Z_TYPE_P(arg1);

    if (UNEXPECTED(!arg1 || (uc_arg1 != IS_ARRAY && uc_arg1 != IS_STRING))) {
        return NULL;
    }

    if (uc_arg1 == IS_ARRAY) {
        instance = asf_config_simple_instance(this_ptr, arg1);
        return instance;
    }

    /* If cache_config_enable is true */
    if (ASF_G(cache_config_enable) && ASF_G(cache_config_buf) != NULL) {
        if ((pzval = zend_symtable_find(ASF_G(cache_config_buf), Z_STR_P(arg1))) != NULL) {
            if (cur_time < ASF_G(cache_config_last_time)) {
                return (instance = asf_config_simple_instance(this_ptr, pzval));
            } else {
                /* Check file_mtime */
                if (asf_absconfig_cache_file_mtime(Z_STRVAL_P(arg1), Z_STRLEN_P(arg1), cur_time, 1)) {
                    return (instance = asf_config_simple_instance(this_ptr, pzval));
                }
            }
        }
    }

    if (strncasecmp(Z_STRVAL_P(arg1) + Z_STRLEN_P(arg1) - 3, "php", 3) == 0) {
        instance = asf_config_php_instance(this_ptr, Z_STR_P(arg1));
    } else if (strncasecmp(Z_STRVAL_P(arg1) + Z_STRLEN_P(arg1) - 3, "ini", 3) == 0) {
        instance = asf_config_ini_instance(this_ptr, arg1, arg2);
    }

    if (ASF_G(cache_config_enable) && Z_TYPE_P(this_ptr) == IS_OBJECT) {
        zval *config = zend_read_property(Z_OBJCE_P(this_ptr), this_ptr, ZEND_STRL(ASF_ABSCONFIG_PROPERTY_NAME), 1, NULL);
        if (EXPECTED(Z_TYPE_P(config) == IS_ARRAY)) {
            (void)asf_absconfig_cache_file_mtime(Z_STRVAL_P(arg1), Z_STRLEN_P(arg1), cur_time, 0);
            /* Initialize persistent memory */
            if (!ASF_G(cache_config_buf)) {
                PMALLOC_HASHTABLE(ASF_G(cache_config_buf));
                zend_hash_init(ASF_G(cache_config_buf), 8, NULL, NULL, 1);
            }
            (void)asf_absconfig_cache_hash_symtable_update(ASF_G(cache_config_buf), Z_STR_P(arg1), config);
        }
    }
   
    return instance;
}
/* }}} */

/* {{{ proto mixed Asf_Config_AbstractConfig::get([string $name = '' [, mixed $default = '']])
    Return find result in configs */
PHP_METHOD(asf_absconfig, get)
{
    zval *ret = NULL, *pzval = NULL, *def = NULL;
    zend_string *name = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|Sz", &name, &def) == FAILURE) {
        return;
    }

    if (!name) {
        RETURN_ZVAL(getThis(), 1, 0);
    } else {
        zval *properties = NULL;
        HashTable *hash = NULL;
        zend_long lval = 0;
        double dval = 0;

        properties = zend_read_property(asf_absconfig_ce, getThis(), ZEND_STRL(ASF_ABSCONFIG_PROPERTY_NAME), 1, NULL);
        if (Z_TYPE_P(properties) == IS_NULL) {
            RETURN_FALSE;
        }
        hash  = Z_ARRVAL_P(properties);

        if (is_numeric_string(ZSTR_VAL(name), ZSTR_LEN(name), &lval, &dval, 0) != IS_LONG) {
            if ((pzval = zend_hash_find(hash, name)) == NULL) {
                if (def != NULL) {
                    RETURN_ZVAL(def, 1, 0);
                } else {
                    RETURN_FALSE;
                }
            } 
        } else {
            if ((pzval = zend_hash_index_find(hash, lval)) == NULL) {
                if (def != NULL) {
                    RETURN_ZVAL(def, 1, 0);
                } else {
                    RETURN_FALSE;
                }
            } 
        }

        if (Z_TYPE_P(pzval) == IS_ARRAY) {
            zval rv = {{0}};
            if ((ret = asf_config_simple_instance(&rv, pzval))) {
                RETURN_ZVAL(ret, 1, 1);
            } else {
                RETURN_FALSE;
            }
        } else {
            RETURN_ZVAL(pzval, 1, 0);
        }
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Asf_Config_AbstractConfig::toArray(void)
    Convert to array */
PHP_METHOD(asf_absconfig, toArray)
{
    zval *properties = zend_read_property(asf_absconfig_ce, getThis(), ZEND_STRL(ASF_ABSCONFIG_PROPERTY_NAME), 1, NULL);
    RETURN_ZVAL(properties, 1, 0);
}
/* }}} */

/* {{{ proto bool Asf_Config_AbstractConfig::__isset(string $name)
*/
PHP_METHOD(asf_absconfig, __isset)
{
    zend_string *name = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
        return;
    } else {
        zval *prop = zend_read_property(asf_absconfig_ce, getThis(), ZEND_STRL(ASF_ABSCONFIG_PROPERTY_NAME), 1, NULL);
        RETURN_BOOL(zend_hash_exists(Z_ARRVAL_P(prop), name));
    }
}
/* }}} */

/* {{{ asf_absconfig_methods[]
*/
zend_function_entry asf_absconfig_methods[] = {
    PHP_ME(asf_absconfig, __isset, asf_absconfig_construct_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_absconfig, get, asf_absconfig_get_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_absconfig, toArray, NULL, ZEND_ACC_PUBLIC)
    PHP_MALIAS(asf_absconfig, __get, get, asf_absconfig_get_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(config_abstractconfig) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_absconfig, Asf_Config_AbstractConfig, Asf\\Config\\AbstractConfig, ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);

    zend_declare_property_null(asf_absconfig_ce, ZEND_STRL(ASF_ABSCONFIG_PROPERTY_NAME), ZEND_ACC_PROTECTED);

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
