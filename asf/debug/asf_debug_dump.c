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
#include "Zend/zend_interfaces.h" /* zend_call_ */
#include "php_asf.h"
#include "kernel/asf_namespace.h"

#include "asf_debug_dump.h"
#include "dump/asf_debug_dump_abstractdump.h"
#include "dump/asf_debug_dump_html.h"
#include "dump/asf_debug_dump_cli.h"

zend_class_entry *asf_debug_dump_ce;

/* {{{ proto void Asf_Debug_Dump::vars(mixed $zv, [, mixed $... ])
*/
PHP_METHOD(asf_debug_dump, vars)
{
    zval *args = NULL, *instance = NULL, rv = {{0}}, zmn;
    int argc = 0;
    uint i = 0;

    ZEND_PARSE_PARAMETERS_START(1, -1)
        Z_PARAM_VARIADIC('+', args, argc)
    ZEND_PARSE_PARAMETERS_END();

    instance = zend_read_static_property(asf_debug_dump_ce, ZEND_STRL(ASF_DEBUG_DUMP_PRONAME_INSTANCE), 1);

    if (Z_TYPE_P(instance) != IS_OBJECT) {
        if (ASF_G(cli) || strncmp(sapi_module.name, "phpdbg", 6) == 0) {
            instance = asf_debug_dump_cli_instance(&rv);
        } else {
            instance = asf_debug_dump_html_instance(&rv);
        }
    }

    if (Z_TYPE(rv) == IS_OBJECT) {
        zend_update_static_property(asf_debug_dump_ce, ZEND_STRL(ASF_DEBUG_DUMP_PRONAME_INSTANCE), instance);
        zval_ptr_dtor(&rv);
    }

    ZVAL_STRINGL(&zmn, "vars", 4);
    call_user_function_ex(&Z_OBJCE_P(instance)->function_table, instance, &zmn, return_value, argc, args, 1, NULL);
    zend_string_release(Z_STR(zmn));
}
/* }}} */

/* {{{ asf_debug_dump_methods[]
*/
zend_function_entry asf_debug_dump_methods[] = {
    PHP_ME(asf_debug_dump, vars, arginfo_asf_zval_dump, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(debug_dump) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_debug_dump, Asf_Debug_Dump, Asf\\Debug\\Dump, ZEND_ACC_FINAL);

    ASF_INIT(debug_dump_abstractdump);
    ASF_INIT(debug_dump_cli);
    ASF_INIT(debug_dump_html);

    zend_declare_property_null(asf_debug_dump_ce, ZEND_STRL(ASF_DEBUG_DUMP_PRONAME_INSTANCE), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);

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
