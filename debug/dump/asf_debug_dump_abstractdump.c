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
#include "asf_debug_dump_abstractdump.h"

/* {{{ #DEFINE */

/* COLOR: https://en.wikipedia.org/wiki/ANSI_escape_code#graphics */
#define ASF_COLOR_BLACK_0      "30"
#define ASF_COLOR_RED_0        "31"
#define ASF_COLOR_GREEN_0      "32"
#define ASF_COLOR_YELLOW_0     "33"
#define ASF_COLOR_BLUE_0       "34"
#define ASF_COLOR_MAGENTA_0    "35"
#define ASF_COLOR_CYAN_0       "36"
#define ASF_COLOR_WHITE_0      "37"

#define ASF_COLOR_BLACK_1      "black"
#define ASF_COLOR_RED_1        "#C1275B"
#define ASF_COLOR_GREEN_1      "green"
#define ASF_COLOR_YELLOW_1     "#b8860b" /* darkgoldenrod */
#define ASF_COLOR_BLUE_1       "blue"
#define ASF_COLOR_MAGENTA_1    "#98488F"
#define ASF_COLOR_CYAN_1       "cyan"
#define ASF_COLOR_WHITE_1      "#ccc"

/* ASF_CFS => ASF_COLOR_FORMAT_START */
#define ASF_CFS_RED(b) (b ? "<font color=" ASF_COLOR_RED_1 ">" : "\e[" ASF_COLOR_RED_0 "m")
#define ASF_CFS_GREEN(b) (b ? "<font color=" ASF_COLOR_GREEN_1 ">" : "\e[" ASF_COLOR_GREEN_0 "m")
#define ASF_CFS_YELLOW(b) (b ? "<font color=" ASF_COLOR_YELLOW_1 ">" : "\e[" ASF_COLOR_YELLOW_0 "m")
#define ASF_CFS_BLACK_I(b) (b ? "<font color=" ASF_COLOR_BLACK_1 "><i>" : "\e[" ASF_COLOR_BLACK_0 "m")
#define ASF_CFS_MAGENTA(b) (b ? "<font color=" ASF_COLOR_MAGENTA_1 ">" : "\e[" ASF_COLOR_MAGENTA_0 "m")

/* ASF_COLOR_FORMAT_END */
#define ASF_CFD(b) (b ? ("</font>") : "\e[0m")
#define ASF_CFD_I(b) (b ? ("</i></font>") : "\e[0m")
/* }}} */

static inline void asf_var_dump(zval *struc, uint level, _Bool b);

zend_class_entry *asf_debug_dump_abstractdump_ce;

static void asf_array_element_dump(zval *zv, zend_ulong index, zend_string *key, int level, _Bool b) /* {{{ */
{
    if (key == NULL) {
        php_printf("%*c %s" ZEND_LONG_FMT "%s %s=>%s ", level + 1, ' ',
                ASF_CFS_RED(b), index, ASF_CFD(b), ASF_CFS_MAGENTA(b), ASF_CFD(b));
    } else {
        php_printf("%*c%s'%s'%s %s=>%s ", level + 1, ' ',
                ASF_CFS_GREEN(b), ZSTR_VAL(key), ASF_CFD(b), ASF_CFS_MAGENTA(b), ASF_CFD(b));
    }

    asf_var_dump(zv, level + 2, b);
}
/* }}} */

static void asf_object_property_dump(zval *zv, zend_ulong index, zend_string *key, int level, _Bool b) /* {{{ */
{
    const char *prop_name = NULL, *class_name = NULL;

    if (key == NULL) {
        php_printf("%*c[%s" ZEND_LONG_FMT "%s] %s=>%s ", level + 1, ' ',
                ASF_CFS_RED(b), index, ASF_CFD(b), ASF_CFS_MAGENTA(b), ASF_CFD(b));
    } else {
        int unmangle = zend_unmangle_property_name(key, &class_name, &prop_name);
        php_printf("%*c[", level + 1, ' ');

        if (class_name && unmangle == SUCCESS) {
            if (class_name[0] == '*') {
                php_printf("%s'%s'%s:protected", ASF_CFS_GREEN(b), prop_name, ASF_CFD(b));
            } else {
                php_printf("%s'%s'%s:%s'%s'%s:private", ASF_CFS_GREEN(b), prop_name, ASF_CFD(b), ASF_CFS_GREEN(b), class_name, ASF_CFD(b));
            }
        } else {
            php_printf("%s'%s'%s", ASF_CFS_GREEN(b), ZSTR_VAL(key), ASF_CFD(b));
        }
        php_printf("] %s=>%s ", ASF_CFS_MAGENTA(b), ASF_CFD(b));
    }
    asf_var_dump(zv, level + 2, b);
}
/* }}} */

static inline void asf_var_dump(zval *struc, uint level, _Bool b) /* {{{ */
{
    HashTable *myht = NULL;
    zend_string *class_name = NULL, *key = NULL;
    int is_temp = 0;
    zend_ulong num = 0;
    zval *val = NULL;
    uint32_t count = 0;

again:
    switch (Z_TYPE_P(struc)) {
        case IS_FALSE:
            php_printf("%sboolean%s %sfalse%s\n", ASF_CFS_YELLOW(b), ASF_CFD(b), ASF_CFS_RED(b), ASF_CFD(b));
            break;
        case IS_TRUE:
            php_printf("%sboolean%s %strue%s\n", ASF_CFS_YELLOW(b), ASF_CFD(b), ASF_CFS_RED(b), ASF_CFD(b));
            break;
        case IS_NULL:
            php_printf("%snull%s\n", ASF_CFS_YELLOW(b), ASF_CFD(b));
            break;
        case IS_LONG:
            php_printf("%sint%s %s" ZEND_LONG_FMT "%s\n", ASF_CFS_YELLOW(b), ASF_CFD(b), ASF_CFS_RED(b), Z_LVAL_P(struc), ASF_CFD(b));
            break;
        case IS_DOUBLE:
            php_printf("%sfloat%s %s%.*G%s\n", ASF_CFS_YELLOW(b), ASF_CFD(b), ASF_CFS_RED(b), (int) EG(precision), Z_DVAL_P(struc), ASF_CFD(b));
            break;
        case IS_STRING:
            php_printf("%sstring%s %s'%s'%s %s(len=%zd)%s\n", ASF_CFS_YELLOW(b), ASF_CFD(b), ASF_CFS_GREEN(b),
                    Z_STRVAL_P(struc), ASF_CFD(b), ASF_CFS_BLACK_I(b), Z_STRLEN_P(struc), ASF_CFD_I(b));
            break;
        case IS_ARRAY:
            myht = Z_ARRVAL_P(struc);
            if (level > 1 && ZEND_HASH_APPLY_PROTECTION(myht) && ++myht->u.v.nApplyCount > 1) {
                PUTS("*RECURSION*\n");
                --myht->u.v.nApplyCount;
                return;
            }
            count = zend_array_count(myht);
            php_printf("%sarray%s %s(size=%d)%s {\n", ASF_CFS_YELLOW(b), ASF_CFD(b), ASF_CFS_BLACK_I(b), count, ASF_CFD_I(b));

            ZEND_HASH_FOREACH_KEY_VAL_IND(myht, num, key, val) {
                asf_array_element_dump(val, num, key, level, b);
            } ZEND_HASH_FOREACH_END();

            if (level > 1 && ZEND_HASH_APPLY_PROTECTION(myht)) {
                --myht->u.v.nApplyCount;
            }

            if (level > 1) {
                php_printf("%*c", level - 1, ' ');
            }
            PHPWRITE("}\n", 2);
            break;
        case IS_OBJECT:
            if (Z_OBJ_APPLY_COUNT_P(struc) > 0) {
                PUTS("*RECURSION*\n");
                return;
            }
            Z_OBJ_INC_APPLY_COUNT_P(struc);

            myht = Z_OBJDEBUG_P(struc, is_temp);
            class_name = Z_OBJ_HANDLER_P(struc, get_class_name)(Z_OBJ_P(struc));
            php_printf("%sobject%s(%s)#%d (%ssize=%d%s) {\n", ASF_CFS_YELLOW(b), ASF_CFD(b),
                    ZSTR_VAL(class_name), Z_OBJ_HANDLE_P(struc), ASF_CFS_BLACK_I(b), myht ? zend_array_count(myht) : 0, ASF_CFD_I(b));
            zend_string_release(class_name);

            if (myht) {
                zend_ulong num;
                zend_string *key;
                zval *val;

                ZEND_HASH_FOREACH_KEY_VAL_IND(myht, num, key, val) {
                    asf_object_property_dump(val, num, key, level, b);
                } ZEND_HASH_FOREACH_END();
                if (is_temp) {
                    zend_hash_destroy(myht);
                    efree(myht);
                }
            }
            if (level > 1) {
                php_printf("%*c", level - 1, ' ');
            }
            PHPWRITE("}\n", 2);
            Z_OBJ_DEC_APPLY_COUNT_P(struc);
            break;
        case IS_RESOURCE: {
              const char *type_name = zend_rsrc_list_get_rsrc_type(Z_RES_P(struc));
              php_printf("%sresource%s(%d) of type (%s)\n", ASF_CFS_YELLOW(b), ASF_CFD(b), Z_RES_P(struc)->handle, type_name ? type_name : "Unknown");
              break;
          }
        case IS_REFERENCE:
          struc = Z_REFVAL_P(struc);
          goto again;
          break;
        default:
          PUTS("UNKNOWN:0\n");
          break;
    }
}
/* }}} */

/* {{{ proto void Asf_Debug_Dump_AbstractDump::vars(mixed $zv, [, mixed $... ])
*/
PHP_METHOD(asf_debug_dump_abstractdump, vars)
{
    zval *args = NULL;
    int argc = 0;
    uint i = 0;

    ZEND_PARSE_PARAMETERS_START(1, -1)
        Z_PARAM_VARIADIC('+', args, argc)
    ZEND_PARSE_PARAMETERS_END();

    zend_long mode = Z_LVAL_P(zend_read_property(asf_debug_dump_abstractdump_ce
                , getThis(), ZEND_STRL(ASF_DEBUG_DUMP_PRONAME_MODE), 1, NULL));

    if (mode) {
        ASF_G(debug_dump) = 1;
        php_printf("<pre>%s:%u\n", zend_get_executed_filename(), zend_get_executed_lineno());
    }
    
    for (i = 0; i < argc; i++) {
        asf_var_dump(&args[i], 1, mode);
    }

    if (mode) {
        zend_write("</pre>", 6);
    }
}
/** }}} */

/* {{{ asf_debug_dump_abstractdump_methods[]
*/
zend_function_entry asf_debug_dump_abstractdump_methods[] = {
    PHP_ME(asf_debug_dump_abstractdump, vars, arginfo_asf_zval_dump, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(debug_dump_abstractdump) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_debug_dump_abstractdump, Asf_Debug_Dump_AbstractDump, Asf\\Debug\\Dump\\AbstractDump, ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);

    zend_declare_property_long(asf_debug_dump_abstractdump_ce, ZEND_STRL(ASF_DEBUG_DUMP_PRONAME_MODE), 1, ZEND_ACC_PROTECTED);

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
