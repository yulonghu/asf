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
#include "asf_exception.h"
#include "kernel/asf_func.h"

#include "cache/asf_cache_abstractadapter.h"

zend_class_entry *asf_cache_absadapter_ce;

/* {{{ ARG_INFO
*/
/*
ZEND_BEGIN_ARG_INFO_EX(asf_absadapter_doquery_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, sql)
    ZEND_ARG_ARRAY_INFO(0, bind_value, 1)
    ZEND_ARG_INFO(0, mode)
    ZEND_ARG_INFO(0, fetch_one)
ZEND_END_ARG_INFO()
*/
/* }}} */

/* {{{ asf_cache_absadapter_methods[]
*/
zend_function_entry asf_cache_absadapter_methods[] = {
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(cache_absadapter) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_cache_absadapter, Asf_Cache_AbstractAdapter, Asf\\Cache\\AbstractAdapter, ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);
    
//    zend_class_implements(asf_cache_absadapter_ce, 1, asf_db_adapterinterface_ce);

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
