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
#include "asf_namespace.h"

#include "http/asf_http_request.h"
#include "asf_sg.h"

zend_class_entry *asf_sg_ce;

void asf_sg_instance(asf_sg_t *this_ptr) /* {{{ */
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

    zend_hash_str_update(ht, "get",     3, get);
    zend_hash_str_update(ht, "post",    4, post);
    zend_hash_str_update(ht, "cookie",  6, cookie);
    
    zend_update_static_property(asf_sg_ce, ZEND_STRL(ASF_SG_PROPERTY_NAME_VAR), &regs);
    zval_ptr_dtor(&regs);
}
/* }}} */

/* {{{ asf_sg_methods[]
    Will add methods later */
zend_function_entry asf_sg_methods[] = {
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(sg) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_sg, Asf_Sg, Asf\\Sg, ZEND_ACC_FINAL);

    zend_declare_property_null(asf_sg_ce, ZEND_STRL(ASF_SG_PROPERTY_NAME_VAR), ZEND_ACC_PUBLIC | ZEND_ACC_STATIC);

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
