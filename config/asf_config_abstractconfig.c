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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_asf.h"
#include "asf_namespace.h"
#include "asf_exception.h"

#include "asf_config_abstractconfig.h"
#include "asf_config_simple.h"
#include "asf_config_php.h"
#include "asf_config_ini.h"

zend_class_entry *asf_absconfig_ce;

zval *asf_absconfig_instance(zval *this_ptr, zval *arg1, zval *arg2) /* {{{ */
{
    zval *instance = NULL;

    if (UNEXPECTED(!arg1)) {
        return NULL;
    }

    if (Z_TYPE_P(arg1) == IS_STRING) {
        if (strncasecmp(Z_STRVAL_P(arg1) + Z_STRLEN_P(arg1) - 3, "php", 3) == 0) {
            zend_string *file_path = zend_string_init(Z_STRVAL_P(arg1), Z_STRLEN_P(arg1), 0);
            instance = asf_config_php_instance(this_ptr, file_path);
            zend_string_release(file_path);
        } else if (strncasecmp(Z_STRVAL_P(arg1) + Z_STRLEN_P(arg1) - 3, "ini", 3) == 0) {
            instance = asf_config_ini_instance(this_ptr, arg1, arg2);
        }
    } else if (Z_TYPE_P(arg1) == IS_ARRAY) {
        instance = asf_config_simple_instance(this_ptr, arg1);
    }

    return NULL;
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
