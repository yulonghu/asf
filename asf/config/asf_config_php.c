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
#include "kernel/asf_namespace.h"
#include "asf_exception.h"

#include "asf_loader.h"
#include "config/asf_config_php.h"
#include "config/asf_config_abstractconfig.h"

zend_class_entry *asf_config_php_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_config_php_construct_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
/* }}} */

zval *asf_config_php_instance(zval *this_ptr, zend_string *file_path) /* {{{ */
{
    uint retval = 0;
    zval values;

    if (EXPECTED(Z_ISUNDEF_P(this_ptr))) {
        object_init_ex(this_ptr, asf_config_php_ce);
    }

    retval = asf_loader_import(file_path, &values);

    if (UNEXPECTED(!retval)) {
        asf_trigger_error(ASF_ERR_BOOTSTRAP_FAILED, "No such file %s", ZSTR_VAL(file_path));
        return NULL;
    }

    if (Z_TYPE(values) == IS_ARRAY) {
        zend_update_property(asf_config_php_ce, this_ptr, ZEND_STRL(ASF_ABSCONFIG_PROPERTY_NAME), &values);
    }

    if (Z_REFCOUNTED(values)) {
        zval_ptr_dtor(&values);
    }

    return this_ptr;
}
/* }}} */

/* {{{ proto object Asf_Config_Php::__construct(string $file_path)
    Loaded the php file */
PHP_METHOD(asf_config_php, __construct)
{
    zval *file_path = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &file_path) == FAILURE) {
        return;
    }

    if (UNEXPECTED(Z_TYPE_P(file_path) != IS_STRING)) {
        asf_trigger_error(ASF_ERR_CONFIG_PARAM, "The parameters 'file_path' must be a string");
        return;
    }

    if(UNEXPECTED(strncasecmp(Z_STRVAL_P(file_path) + Z_STRLEN_P(file_path) - 3, "php", 3))) {
        asf_trigger_error(ASF_ERR_CONFIG_PARAM, "The parameters 'file_path' must be an php file");
        return;
    }

    (void)asf_absconfig_instance(getThis(), file_path, NULL);
}
/** }}} */

/* {{{ asf_config_php_methods[]
*/
zend_function_entry asf_config_php_methods[] = {
    PHP_ME(asf_config_php, __construct, asf_config_php_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(config_php) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERNAL(asf_config_php, Asf_Config_Php, Asf\\Config\\Php, asf_absconfig_ce, asf_config_php_methods);

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
