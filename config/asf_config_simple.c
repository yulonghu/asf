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
#include "Zend/zend_interfaces.h"

#include "php_asf.h"
#include "asf_namespace.h"
#include "asf_exception.h"

#include "config/asf_config_abstractconfig.h"
#include "config/asf_config_simple.h"

zend_class_entry *asf_config_simple_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_config_simple_construct_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
/* }}} */

zval *asf_config_simple_instance(zval *this_ptr, zval *values) /* {{{ */
{
    if (Z_TYPE_P(values) != IS_ARRAY) {
        return NULL;
    }

    if (EXPECTED(Z_ISUNDEF_P(this_ptr))) {
        object_init_ex(this_ptr, asf_config_simple_ce);
    }

    zend_update_property(asf_config_simple_ce, this_ptr, ZEND_STRL(ASF_ABSCONFIG_PROPERTY_NAME), values);
    return this_ptr;
}
/* }}} */

/* {{{ proto object Asf_Config_Simple::__construct(array $values)
    Loaded php variable */
PHP_METHOD(asf_config_simple, __construct)
{
    zval *values = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &values) == FAILURE) {
        return;
    }

    (void)asf_config_simple_instance(getThis(), values);
}
/** }}} */

/* {{{ asf_config_simple_methods[]
*/
zend_function_entry asf_config_simple_methods[] = {
    PHP_ME(asf_config_simple, __construct, asf_config_simple_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(config_simple) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERNAL(asf_config_simple, Asf_Config_Simple, Asf\\Config\\Simple, asf_absconfig_ce, asf_config_simple_methods);

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
