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
#include "Zend/zend_interfaces.h" /* for zend_call_method_with_* */
#include "ext/spl/spl_iterators.h"

#include "php_asf.h"
#include "kernel/asf_namespace.h"
#include "asf_exception.h"

#include "asf_config.h"
#include "config/asf_config_abstractconfig.h"
#include "config/asf_config_simple.h"
#include "config/asf_config_php.h"
#include "config/asf_config_ini.h"

#include "asf_application.h"

zend_class_entry *asf_config_ce;

/* {{{ proto mixed Asf_Config::get([mixed $name = '' [, mixed $default = '']])
*/
PHP_METHOD(asf_config, get)
{
    zval *name = NULL, *def = NULL;
    zval ret;

    ZEND_PARSE_PARAMETERS_START(0, 2)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL_DEREF(name)
        Z_PARAM_ZVAL_DEREF(def)
    ZEND_PARSE_PARAMETERS_END_EX();

    zval *instance = zend_read_static_property(asf_application_ce, ZEND_STRL(ASF_APP_PRONAME_INSTANCE), 1);
    if (IS_OBJECT != Z_TYPE_P(instance)) {
        return;
    }

    zval *config = zend_read_property(asf_application_ce, instance, ZEND_STRL(ASF_APP_PRONAME_CONFIG), 1, NULL);
    if (IS_OBJECT != Z_TYPE_P(config)) {
        return;
    }

    if (name) {
        if (def) {
            zend_call_method_with_2_params(config, Z_OBJCE_P(config), NULL, "get", &ret, name, def);
        } else {
            zend_call_method_with_1_params(config, Z_OBJCE_P(config), NULL, "get", &ret, name);
        }
    } else {
        zend_call_method_with_0_params(config, Z_OBJCE_P(config), NULL, "get", &ret);
    }

    RETURN_ZVAL(&ret, 1, 1);
}
/* }}} */

/* {{{ asf_config_methods[]
*/
zend_function_entry asf_config_methods[] = {
    PHP_ME(asf_config, get, asf_absconfig_get_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(config) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_config, Asf_Config, Asf\\Config, 0);

    ASF_INIT(config_abstractconfig);
    ASF_INIT(config_simple);
    ASF_INIT(config_php);
    ASF_INIT(config_ini);

    zend_class_implements(asf_config_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, spl_ce_Countable);

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
