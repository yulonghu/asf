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

void asf_cache_adapter_handler_req(zval *self, uint32_t param_count, zval params[],
        char *method, uint method_len, zval *retval) /*{{{*/
{
    zval *handler = zend_read_property(Z_OBJCE_P(self), self,
            ZEND_STRL(ASF_CACHE_PRONAME_HANDLER), 1, NULL);

    /* trace log */
    double start_time = 0.0;
    if (ASF_G(trace_enable)) {
        start_time = asf_func_gettimeofday();
    }

    ASF_CALL_USER_FUNCTION_EX(handler, method, method_len, retval, param_count, params);

    /* trace log */
    (void)asf_func_add_trace(start_time, method, method_len, param_count, params, retval);
}/*}}}*/

/* {{{ proto mixed Asf_Cache_AbstractAdapter::getConnectInfo(void)
*/
PHP_METHOD(asf_cache_absadapter, getConnectInfo)
{
    ZVAL_COPY(return_value,
            zend_read_property(asf_cache_absadapter_ce, getThis(), ZEND_STRL(ASF_CACHE_PRONAME_CONNECT_INFO), 1, NULL));
}
/* }}} */

/* {{{ proto object Asf_Cache_AbstractAdapter::getHandler(void)
*/
PHP_METHOD(asf_cache_absadapter, getHandler)
{
    ZVAL_COPY(return_value,
            zend_read_property(asf_cache_absadapter_ce, getThis(), ZEND_STRL(ASF_CACHE_PRONAME_HANDLER), 1, NULL));
}
/* }}} */

/* {{{ asf_cache_absadapter_methods[]
*/
zend_function_entry asf_cache_absadapter_methods[] = {
    PHP_ME(asf_cache_absadapter, getConnectInfo,  NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_cache_absadapter, getHandler,  NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(cache_absadapter) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_cache_absadapter, Asf_Cache_AbstractAdapter, Asf\\Cache\\AbstractAdapter, ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);
    
    zend_declare_property_null(asf_cache_absadapter_ce, ZEND_STRL(ASF_CACHE_PRONAME_HANDLER), ZEND_ACC_PROTECTED);
    zend_declare_property_long(asf_cache_absadapter_ce, ZEND_STRL(ASF_CACHE_PRONAME_CONSUME), 0, ZEND_ACC_PROTECTED);
    zend_declare_property_null(asf_cache_absadapter_ce, ZEND_STRL(ASF_CACHE_PRONAME_CONNECT_INFO), ZEND_ACC_PROTECTED);

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
