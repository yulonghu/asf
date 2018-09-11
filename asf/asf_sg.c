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
#include "php_asf.h"
#include "kernel/asf_namespace.h"

#include "http/asf_http_request.h"
#include "asf_sg.h"

zend_class_entry *asf_sg_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_sg_common_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(asf_sg_get_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, strtok)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(asf_sg_set_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
/* }}} */

static inline zval *asf_sg_strtok_get(HashTable *vars, zend_string *name) /* {{{ */
{
    zval *pzval = NULL;

    if (zend_memrchr(ZSTR_VAL(name), '.', ZSTR_LEN(name))) {
        char *seg = NULL, *entry = NULL, *ptr = NULL;

        entry = estrndup(ZSTR_VAL(name), ZSTR_LEN(name));
        if ((seg = php_strtok_r(entry, ".", &ptr))) {
            do {
                if (vars == NULL || (pzval = zend_symtable_str_find(vars, seg, strlen(seg))) == NULL) {
                    efree(entry);
                    return NULL;
                }

                if (Z_TYPE_P(pzval) == IS_ARRAY) {
                    vars = Z_ARRVAL_P(pzval);
                } else {
                    vars = NULL;
                }

            } while ((seg = php_strtok_r(NULL, ".", &ptr)));
        }
        efree(entry);
    } else {
        pzval = zend_symtable_find(vars, name);
    }

    return pzval;
}
/* }}} */

void asf_sg_instance() /* {{{ */
{
    zval regs;

    array_init(&regs);

    zval *get = asf_http_req_pg_find(TRACK_VARS_GET);
    Z_TRY_ADDREF_P(get);

    zval *post = asf_http_req_pg_find(TRACK_VARS_POST);
    Z_TRY_ADDREF_P(post);

    zval *cookie = asf_http_req_pg_find(TRACK_VARS_COOKIE);
    Z_TRY_ADDREF_P(cookie);

    HashTable *ht = Z_ARRVAL(regs);

    zend_hash_str_add_new(ht, "get",     3, get);
    zend_hash_str_add_new(ht, "post",    4, post);
    zend_hash_str_add_new(ht, "cookie",  6, cookie);
    
    zend_update_static_property(asf_sg_ce, ZEND_STRL(ASF_SG_PRONAME_VAR), &regs);
    zval_ptr_dtor(&regs);
}
/* }}} */

/* {{{ proto bool Asf_Sg::has(string $name [, bool $strtok = 1])
*/
PHP_METHOD(asf_sg, has)
{
    zend_string *name = NULL;
    _Bool strtok = 1;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|b", &name, &strtok) == FAILURE) {
        return;
    }

    zval *vars = zend_read_static_property(asf_sg_ce, ZEND_STRL(ASF_SG_PRONAME_VAR), 1);
    if (Z_TYPE_P(vars) != IS_ARRAY) {
        RETURN_FALSE;
    }

    if (strtok) {
        if (asf_sg_strtok_get(Z_ARRVAL_P(vars), name) != NULL) {
            RETURN_TRUE;
        } else {
            RETURN_FALSE;
        }
    } else {
        RETURN_BOOL(zend_hash_exists(Z_ARRVAL_P(vars), name));
    }
}
/* }}} */

/* {{{ proto mixed Asf_Sg::get(string $name [, bool $strtok = 1])
*/
PHP_METHOD(asf_sg, get)
{
    zend_string *name = NULL;
    zval *vars = NULL, *pzval = NULL;
    _Bool strtok = 1;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|b", &name, &strtok) == FAILURE) {
        return;
    }

    vars = zend_read_static_property(asf_sg_ce, ZEND_STRL(ASF_SG_PRONAME_VAR), 1);
    if (Z_TYPE_P(vars) != IS_ARRAY) {
        RETURN_NULL();
    }

    if (strtok) {
        if ((pzval = asf_sg_strtok_get(Z_ARRVAL_P(vars), name)) != NULL) {
            RETURN_ZVAL(pzval, 1, 0);
        }
    } else {
        if ((pzval = zend_hash_find(Z_ARRVAL_P(vars), name)) != NULL) {
            RETURN_ZVAL(pzval, 1, 0);
        }
    }

    RETURN_NULL();
}
/* }}} */

/* {{{ proto bool Asf_Sg::set(string $name, mixed $value)
*/
PHP_METHOD(asf_sg, set)
{
    zend_string *name = NULL;
    zval *vars = NULL, *value = NULL, regs;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &value) == FAILURE) {
        return;
    }

    vars = zend_read_static_property(asf_sg_ce, ZEND_STRL(ASF_SG_PRONAME_VAR), 1);
    if (Z_TYPE_P(vars) != IS_ARRAY) {
        array_init(&regs);
        vars = &regs;
    }

    if (zend_hash_update(Z_ARRVAL_P(vars), name, value) != NULL) {
        Z_TRY_ADDREF_P(value);
        RETVAL_TRUE;
    } else {
        RETVAL_FALSE; 
    }

    if (Z_TYPE(regs) == IS_ARRAY) {
        zend_update_static_property(asf_sg_ce, ZEND_STRL(ASF_SG_PRONAME_VAR), &regs);
        zval_ptr_dtor(&regs);
    }
}
/* }}} */

/* {{{ proto bool Asf_Sg::del(string $name)
*/
PHP_METHOD(asf_sg, del)
{
    zend_string *name = NULL;
    zval *vars = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
        return;
    }

    vars = zend_read_static_property(asf_sg_ce, ZEND_STRL(ASF_SG_PRONAME_VAR), 1);
    if (Z_TYPE_P(vars) != IS_ARRAY) {
        RETURN_FALSE;
    }

    if (zend_hash_del(Z_ARRVAL_P(vars), name) == SUCCESS) {
        RETURN_TRUE;
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ asf_sg_methods[] */
zend_function_entry asf_sg_methods[] = {
    PHP_ME(asf_sg, has, asf_sg_get_arginfo,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_sg, get, asf_sg_get_arginfo,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_sg, set, asf_sg_set_arginfo,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_sg, del, asf_sg_common_arginfo,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(sg) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_sg, Asf_Sg, Asf\\Sg, ZEND_ACC_FINAL);

    zend_declare_property_null(asf_sg_ce, ZEND_STRL(ASF_SG_PRONAME_VAR), ZEND_ACC_PUBLIC | ZEND_ACC_STATIC);

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
