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

#include "asf_loader.h"
#include "config/asf_config_ini.h"
#include "config/asf_config_abstractconfig.h"

zend_class_entry *asf_config_ini_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_config_ini_construct_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, section)
ZEND_END_ARG_INFO()
/* }}} */

zval *asf_config_ini_instance(zval *this_ptr, zval *file_path, zval *section) /* {{{ */
{
    zval retval, arg2;

    ZVAL_BOOL(&arg2, 1);

    zend_call_method(NULL, NULL, NULL, ASF_CONFIG_INI_PARSE_NAME, ASF_CONFIG_INI_PARSE_NAME_LEN, &retval, 2, file_path, &arg2);

    if (Z_ISUNDEF(retval) || Z_TYPE(retval) != IS_ARRAY) {
        zval_ptr_dtor(&arg2);
        return NULL;
    }

    if (section && EXPECTED(Z_TYPE_P(section) == IS_STRING && Z_STRLEN_P(section))) {
        zval *section_val = NULL;
        if ((section_val = zend_symtable_find(Z_ARRVAL(retval), Z_STR_P(section))) == NULL) {
            zval_ptr_dtor(&retval);
            zval_ptr_dtor(&arg2);
            asf_trigger_error(E_ERROR, "There is no section '%s' in '%s'", Z_STRVAL_P(section), file_path);
            return NULL;
        }

        zval zcopy, garbage;
        array_init(&zcopy);

        zend_hash_copy(Z_ARRVAL(zcopy), Z_ARRVAL_P(section_val), (copy_ctor_func_t) zval_add_ref); 
        ZVAL_COPY_VALUE(&garbage, &retval);
        ZVAL_COPY_VALUE(&retval, &zcopy);
        zval_ptr_dtor(&garbage);
    }

    if (EXPECTED(Z_ISUNDEF_P(this_ptr))) {
        object_init_ex(this_ptr, asf_config_ini_ce);
    }

    zend_update_property(asf_config_ini_ce, this_ptr, ZEND_STRL(ASF_ABSCONFIG_PROPERTY_NAME), &retval);

    zval_ptr_dtor(&retval);
    zval_ptr_dtor(&arg2);

    return this_ptr;
}
/* }}} */

/* {{{ proto object Asf_Config_Ini::__construct(string $file_path [, string $section = ''])
    Loaded the ini file */
PHP_METHOD(asf_config_ini, __construct)
{
    zval *file_path = NULL, *section = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z", &file_path, &section) == FAILURE) {
        return;
    }

    if (UNEXPECTED(Z_TYPE_P(file_path) != IS_STRING)) {
        asf_trigger_error(ASF_ERR_CONFIG_PARAM, "The parameters 'file_path' must be a string");
        return;
    }

    if(UNEXPECTED(strncasecmp(Z_STRVAL_P(file_path) + Z_STRLEN_P(file_path) - 3, "ini", 3))) {
        asf_trigger_error(ASF_ERR_CONFIG_PARAM, "The parameters 'file_path' must be an ini file");
        return;
    }

    (void)asf_absconfig_instance(getThis(), file_path, section);
}
/** }}} */

/* {{{ asf_config_ini_methods[]
*/
zend_function_entry asf_config_ini_methods[] = {
    PHP_ME(asf_config_ini, __construct, asf_config_ini_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(config_ini) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERNAL(asf_config_ini, Asf_Config_Ini, Asf\\Config\\Ini, asf_absconfig_ce, asf_config_ini_methods);

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
