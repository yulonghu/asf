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
#include "main/SAPI.h"
#include "php_asf.h"
#include "kernel/asf_namespace.h"
#include "asf_debug_trace.h"

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_trace_set_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
/* }}} */

zend_class_entry *asf_debug_trace_ce;

/* {{{ proto void Asf_Debug_Trace::show()
*/
PHP_METHOD(asf_debug_trace, show)
{
    if (ASF_G(trace_enable)) {
        RETURN_ZVAL(&ASF_G(trace_buf), 1, 0);
    } else {
        RETURN_NULL();
    }
}
/* }}} */

/* {{{ proto void Asf_Debug_Trace::log()
*/
PHP_METHOD(asf_debug_trace, log)
{
    zval *value = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
        return;
    }

    if (ASF_G(trace_enable)) {
        if (Z_TYPE(ASF_G(trace_buf)) != IS_ARRAY) {
            array_init(&ASF_G(trace_buf));
        }

        Z_TRY_ADDREF_P(value);
        add_next_index_zval(&ASF_G(trace_buf), value);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */



/* {{{ asf_debug_trace_methods[]
*/
zend_function_entry asf_debug_trace_methods[] = {
    PHP_ME(asf_debug_trace, show, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_debug_trace, log, asf_trace_set_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(debug_trace) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_debug_trace, Asf_Debug_Trace, Asf\\Debug\\Trace, ZEND_ACC_FINAL);

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
