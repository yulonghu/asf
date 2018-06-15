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
#include "zend_smart_str.h" /* for smart_str */

#include "php_asf.h"
#include "asf_namespace.h"
#include "asf_exception.h"
#include "kernel/asf_func.h"

#include "db/asf_db_abstractquerybuilder.h"
#include "db/querybuilder/asf_db_querybuilder_select.h"

zend_class_entry *asf_db_qb_select_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_db_qb_select_ab_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, col)
    ZEND_ARG_INFO(0, alias_col)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_db_qb_select_cols_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, cols)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_db_qb_select_having_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, columns)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, oprator)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_db_qb_select_gb_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, col)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_db_qb_select_ob_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, col)
    ZEND_ARG_INFO(0, oprator)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Select::join(string $columns [, string $alias_columns])
*/
ASF_DB_QB_SELECT_METHOD(join, "JOIN", 0);
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Select::leftJoin(string $columns [, string $alias_columns])
*/
ASF_DB_QB_SELECT_METHOD(leftJoin, "LEFT JOIN", 0);
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Select::rightJoin(string $columns [, string $alias_columns])
*/
ASF_DB_QB_SELECT_METHOD(rightJoin, "RIGHT JOIN", 0);
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Select::innerJoin(string $columns [, string $alias_columns])
*/
ASF_DB_QB_SELECT_METHOD(innerJoin, "INNER JOIN", 0);
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Select::count(string $columns [, string $alias_columns])
*/
ASF_DB_QB_SELECT_METHOD(count, "", 1);
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Select::distinct(string $columns [, string $alias_columns])
*/
ASF_DB_QB_SELECT_METHOD(distinct, "", 1);
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Select::max(string $columns [, string $alias_columns])
*/
ASF_DB_QB_SELECT_METHOD(max, "", 1);
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Select::min(string $columns [, string $alias_columns])
*/
ASF_DB_QB_SELECT_METHOD(min, "", 1);
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Select::sum(string $columns [, string $alias_columns])
*/
ASF_DB_QB_SELECT_METHOD(sum, "", 1);
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Select::on(string $columns_a, string $columns_b)
*/
ASF_DB_QB_SELECT_METHOD(on, "ON", 2);
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Select::groupBy(string $columns)
*/
PHP_METHOD(asf_db_qb_select, groupBy)
{
    zend_string *col = NULL, *cols_set = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &col) == FAILURE) {
        return;
    }

    zval *self = getThis();
    cols_set = asf_db_backquote_columns(col);

    (void)asf_db_absqb_sql_format(self, "GROUP BY %s", ZSTR_VAL(cols_set));
    
    zend_string_release(cols_set);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Select::having(string $columns, mixed $value [, string $operator = '='])
*/
PHP_METHOD(asf_db_qb_select, having)
{
    zend_string *columns = NULL, *oprator = NULL;
    zval *value = NULL, *self = NULL;
    zval args[4], ret;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz|S", &columns, &value, &oprator) == FAILURE) {
        return;
    }

    self = getThis();

    /* Call the function 'where' */
    ZVAL_STR(&args[0], columns);
    ZVAL_COPY_VALUE(&args[1], value);
    ZVAL_STR(&args[2], oprator);
    ZVAL_LONG(&args[3], 3);

    ASF_CALL_USER_FUNCTION_EX(self, "where", 5, return_value, 4, args);

    (void)asf_db_absqb_clear_where_set(self);
}
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Select::cols(array $array1 [, array $... ])
*/
PHP_METHOD(asf_db_qb_select, cols)
{
    zval *cols = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|z", &cols) == FAILURE) {
        return;
    }

    smart_str buf = {0}; _Bool mod = 0;
    zval *entry = NULL; zend_string *cols_ret = NULL;
    zval *self = getThis();

    /* speed */
    if (!cols) {
        smart_str_appendl(&buf, "*", 1);
    } else if (Z_TYPE_P(cols) == IS_ARRAY) {
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(cols), entry) {
            if (Z_TYPE_P(entry) != IS_STRING) {
                continue;
            }
            
            if (mod) {
                smart_str_appendl(&buf, ", ", 2);
            }

            /* trim */
            cols_ret = asf_db_backquote_columns(Z_STR_P(entry));
            smart_str_appendl(&buf, ZSTR_VAL(cols_ret), ZSTR_LEN(cols_ret));
            zend_string_release(cols_ret); cols_ret = NULL;
            mod = 1;
        } ZEND_HASH_FOREACH_END();
    } else if (Z_TYPE_P(cols) == IS_STRING) {
        smart_str_appendl(&buf, Z_STRVAL_P(cols), Z_STRLEN_P(cols));
    } else {
        smart_str_appendl(&buf, "*", 1);
    }

    smart_str_0(&buf);

    (void)asf_db_absqb_sql_format(self, ZSTR_VAL(buf.s));

    smart_str_free(&buf);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Select::orderBy(string $columns [, string $operator = 'DESC'])
*/
PHP_METHOD(asf_db_qb_select, orderBy)
{
    zend_string *col = NULL, *operator = NULL, *cols_ret = NULL;
    /* ASC and DESC */
    char *ad = ASF_DB_QB_SELECT_DESC;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &col, &operator) == FAILURE) {
        return;
    }

    do {
        if (!operator  || asf_func_isempty(ZSTR_VAL(operator))) {
            ad = ASF_DB_QB_SELECT_DESC;
            break;
        }
        if (strncasecmp(ZSTR_VAL(operator), ASF_DB_QB_SELECT_ASC, 3) == 0) {
            ad = ASF_DB_QB_SELECT_ASC;
            break;
        }
    } while (0);

    zval *self = getThis();
    cols_ret = asf_db_backquote_columns(col);

    (void)asf_db_absqb_sql_format(self, "ORDER BY %s %s", ZSTR_VAL(cols_ret), ad);

    zend_string_release(cols_ret);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Select::union(void)
*/
PHP_METHOD(asf_db_qb_select, union)
{
    zval *self = getThis();

    (void)asf_db_absqb_sql_format(self, "UNION");

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Select::unionAll(void)
*/
PHP_METHOD(asf_db_qb_select, unionAll)
{
    zval *self = getThis();

    (void)asf_db_absqb_sql_format(self, "UNION ALL");

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Select::__construct(void)
*/
PHP_METHOD(asf_db_qb_select, __construct)
{
    zval *self = getThis();

    (void)asf_db_absqb_sql_format(self, "SELECT");

    zend_update_property_long(Z_OBJCE_P(self), self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_CURD), ASF_DB_ABSQB_SELECT);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ asf_db_qb_select_methods[]
*/
zend_function_entry asf_db_qb_select_methods[] = {
    PHP_ME(asf_db_qb_select, join,        asf_db_qb_select_ab_arginfo,      ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_qb_select, leftJoin,    asf_db_qb_select_ab_arginfo,      ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_qb_select, rightJoin,   asf_db_qb_select_ab_arginfo,      ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_qb_select, innerJoin,   asf_db_qb_select_ab_arginfo,      ZEND_ACC_PUBLIC)
    
    PHP_ME(asf_db_qb_select, cols,        asf_db_qb_select_cols_arginfo,    ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_qb_select, having,      asf_db_qb_select_having_arginfo,  ZEND_ACC_PUBLIC)
    
    PHP_ME(asf_db_qb_select, groupBy,     asf_db_qb_select_gb_arginfo,      ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_qb_select, orderBy,     asf_db_qb_select_ob_arginfo,      ZEND_ACC_PUBLIC)
    
    PHP_ME(asf_db_qb_select, union,       NULL,         ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_qb_select, unionAll,    NULL,         ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_qb_select, __construct, NULL,         ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_MALIAS(asf_db_qb_select, select, __construct, NULL, ZEND_ACC_PUBLIC)
    
    PHP_ME(asf_db_qb_select, count,       asf_db_qb_select_ab_arginfo,      ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_qb_select, max,         asf_db_qb_select_ab_arginfo,      ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_qb_select, min,         asf_db_qb_select_ab_arginfo,      ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_qb_select, sum,         asf_db_qb_select_ab_arginfo,      ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_qb_select, on,          asf_db_qb_select_ab_arginfo,      ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_qb_select, distinct,    asf_db_qb_select_ab_arginfo,      ZEND_ACC_PUBLIC)

    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(db_qb_select) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERNAL(asf_db_qb_select, Asf_Db_QueryBuilder_Select, Asf\\Db\\QueryBuilder\\Select, asf_db_absqb_ce, asf_db_qb_select_methods);

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
