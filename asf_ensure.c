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

#include "php_asf.h"
#include "asf_namespace.h"
#include "asf_ensure.h"
#include "asf_loader.h"
#include "asf_exception.h"
#include "kernel/asf_constants.h"
#include "http/asf_http_response.h"

zend_class_entry *asf_ensure_ce;

/* {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(asf_ensure_common_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, data)
    ZEND_ARG_INFO(0, code)
ZEND_END_ARG_INFO()
/* }}} */

static _Bool asf_ensure_write_stdout(char *class_name, size_t class_name_len, zend_ulong code) /* {{{ */
{
    zend_class_entry *ce = NULL;
    zval *retval = NULL;

    if (!class_name_len) {
        class_name_len  = ASF_CONSTANTS_DEFAULT_LEN;
        class_name      = ASF_CONSTANTS_DEFAULT_NAME_LC;
    } else {
        zend_str_tolower(class_name, class_name_len);
    }

    if ((ce = zend_hash_str_find_ptr(EG(class_table), class_name, class_name_len)) == NULL) {
        asf_trigger_error(ASF_ERR_NOTFOUND_CONSTANTS_CLASS, "Class '%s' not found", class_name);
        return 0;
    }

    /*
       if (UNEXPECTED(!instanceof_function(ce, asf_constants_ce))) {
       asf_trigger_error(ASF_ERR_CONSTANTS_FAILED, "Expect a %s instance, %s give", ZSTR_VAL(asf_constants_ce->name), ZSTR_VAL(ce->name));	
       return 0;
       }
       */

    /*
       zend_class_constant *constant = NULL;
       if ((constant = zend_hash_str_find_ptr(&((ce)->constants_table), code, code_len)) == NULL) {
       asf_trigger_error(ASF_ERR_NOTFOUND_CONSTANTS_TABLE, "Class Constant %s::%s does not exist", class_name, code);
       return 0;
       }
       */

    zend_property_info *prop_info;
    if ((prop_info = zend_hash_str_find_ptr(&((ce)->properties_info), ASF_CONSTANTS_ERR_PROPERTY_NAME, ASF_CONSTANTS_ERR_PROPERTY_LEN)) == NULL) {
        asf_trigger_error(ASF_ERR_NOTFOUND_CONSTANTS_PROP, "Class Properties %s::%s does not exist", class_name, ASF_CONSTANTS_ERR_PROPERTY_NAME);
        return 0;
    }

    if ((prop_info->flags & ZEND_ACC_STATIC) && (prop_info->flags & ZEND_ACC_PUBLIC)) {
        zval *rets = zend_read_static_property(ce, ASF_CONSTANTS_ERR_PROPERTY_NAME, ASF_CONSTANTS_ERR_PROPERTY_LEN, 1);
        if (Z_TYPE_P(rets) != IS_ARRAY) {
            asf_trigger_error(ASF_ERR_NOTFOUND_CONSTANTS_PROP, "Class Properties %s::%s value is must be array", class_name, ASF_CONSTANTS_ERR_PROPERTY_NAME);
            return 0;
        }

        retval = zend_hash_index_find(Z_ARRVAL_P(rets), code);

        if (retval == NULL) {
            asf_trigger_error(ASF_ERR_NOTFOUND_CONSTANTS_PROP, "Class Properties %s::$%s[%d] does not exist",
                    class_name, ASF_CONSTANTS_ERR_PROPERTY_NAME, code);
            return 0;
        }
    }

    if (Z_TYPE_P(retval) != IS_STRING) {
        asf_trigger_error(ASF_ERR_NOTFOUND_CONSTANTS_PROP, "Class Properties %s::$%s[%d] value is must be string",
                class_name, ASF_CONSTANTS_ERR_PROPERTY_NAME, code);
        return 0;
    }

    (void)asf_http_rep_display_error(code, retval);

    return 1;
}
/* }}} */

/* {{{ proto bool Asf_Ensure::isNull(mixed $data, int $code)
*/
ASF_ENSURE_COMMON_METHOD(asf_ensure, isNull, ASF_ENSURE_FLAG_ISNULL);
/* }}} */

/* {{{ proto bool Asf_Ensure::notNull(mixed $data, int $code)
*/
ASF_ENSURE_COMMON_METHOD(asf_ensure, notNull, ASF_ENSURE_FLAG_NOTNULL);
/* }}} */

/* {{{ proto bool Asf_Ensure::isEmpty(mixed $data, int $code)
*/
ASF_ENSURE_COMMON_METHOD(asf_ensure, isEmpty, ASF_ENSURE_FLAG_ISEMPTY);
/* }}} */

/* {{{ proto bool Asf_Ensure::NotEmpty(mixed $data, int $code)
*/
ASF_ENSURE_COMMON_METHOD(asf_ensure, NotEmpty, ASF_ENSURE_FLAG_NOTEMPTY);
/* }}} */

/* {{{ proto bool Asf_Ensure::isFalse(mixed $data, int $code)
*/
ASF_ENSURE_COMMON_METHOD(asf_ensure, isFalse, ASF_ENSURE_FLAG_ISFALSE);
/* }}} */

/* {{{ proto bool Asf_Ensure::notFalse(mixed $data, int $code)
*/
ASF_ENSURE_COMMON_METHOD(asf_ensure, notFalse, ASF_ENSURE_FLAG_NOTFALSE);
/* }}} */

/* {{{ proto bool Asf_Ensure::isTrue(mixed $data, int $code)
*/
ASF_ENSURE_COMMON_METHOD(asf_ensure, isTrue, ASF_ENSURE_FLAG_ISTRUE);
/* }}} */

/* {{{ asf_ensure_methods[]
*/
zend_function_entry asf_ensure_methods[] = {
    PHP_ME(asf_ensure, isNull,		asf_ensure_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(asf_ensure, notNull,		asf_ensure_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(asf_ensure, isEmpty,		asf_ensure_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(asf_ensure, NotEmpty,	asf_ensure_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(asf_ensure, isFalse,		asf_ensure_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(asf_ensure, notFalse,	asf_ensure_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(asf_ensure, isTrue,		asf_ensure_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(ensure) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_ensure, Asf_Ensure, Asf\\Ensure, ZEND_ACC_FINAL);

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
