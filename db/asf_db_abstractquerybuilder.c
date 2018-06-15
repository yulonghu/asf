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
#include "ext/standard/php_string.h" /* for php_implode */
#include "Zend/zend_interfaces.h" /* zend_call_ */
#include "ext/standard/php_array.h" /* php_array_merge */

#include "php_asf.h"
#include "kernel/asf_namespace.h"
#include "asf_exception.h"
#include "kernel/asf_func.h"
#include "asf_db.h"
#include "db/asf_db_abstractquerybuilder.h"

zend_class_entry *asf_db_absqb_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_db_absqb_where_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, columns)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, oprator)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_db_absqb_wheres_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, cols, 1)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_db_absqb_wherein_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, columns)
    ZEND_ARG_ARRAY_INFO(0, value, 1)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_db_absqb_from_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, alias_name)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_db_absqb_table_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_db_absqb_set_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, condition)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_db_absqb_limit_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, start)
    ZEND_ARG_INFO(0, end)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_db_absqb_cm_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_db_absqb_between_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, col)
    ZEND_ARG_INFO(0, a)
    ZEND_ARG_INFO(0, b)
    ZEND_ARG_INFO(0, condition)
ZEND_END_ARG_INFO()
/* }}} */

static inline void asf_db_absqb_start(zval *self, zval **sql, zval **bind_value, zval **where) /*{{{*/
{
    *sql         = zend_read_property(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_SQL), 1, NULL);
    *bind_value  = zend_read_property(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_BINDVALUE), 1, NULL);
    *where       = zend_read_property(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_WHERE), 1, NULL);

    if (Z_TYPE_P(*sql) != IS_STRING) {
        ZVAL_EMPTY_STRING(*sql);
    }
}/*}}}*/

static inline void asf_db_absqb_end(zval *self, zval *value, zval *bind_value, zval *where, const char *tmp, size_t tmp_len) /*{{{*/
{
    zval regs;

    if (Z_TYPE_P(bind_value) != IS_ARRAY) {
        array_init(&regs);
        ZVAL_UNDEF(bind_value);
        ZVAL_COPY_VALUE(bind_value, &regs);
    }

    if (Z_TYPE_P(value) == IS_ARRAY) {
        php_array_merge(Z_ARRVAL_P(bind_value), Z_ARRVAL_P(value));
    } else {
        /* If the 'value' not IS_STR_INTERNED, need refcount++ */
        Z_TRY_ADDREF_P(value);
        add_next_index_zval(bind_value, value);
    }

    zend_update_property_stringl(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_SQL), tmp, tmp_len);
    zend_update_property(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_BINDVALUE), bind_value);

    if (Z_TYPE_P(where) == IS_FALSE) {
        zend_update_property_bool(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_WHERE), 1);
    }
}
/*}}}*/

/* Important: clear property where and set */
void asf_db_absqb_clear_where_set(asf_db_t *db) /*{{{*/
{
    zend_update_property_bool(asf_db_absqb_ce, db, ZEND_STRL(ASF_DB_ABSQB_PRONAME_WHERE), 0);
    zend_update_property_bool(asf_db_absqb_ce, db, ZEND_STRL(ASF_DB_ABSQB_PRONAME_SET), 0);
}
/*}}}*/

void asf_db_absqb_sql_format(asf_db_t *db, char *format, ...) /*{{{*/
{
    va_list args;
    char *msg = NULL, *pmsg = NULL;
    uint msg_len = 0, pmsg_len = 0;

    zval *sql = zend_read_property(asf_db_absqb_ce, db, ZEND_STRL(ASF_DB_ABSQB_PRONAME_SQL), 1, NULL);

    if (Z_TYPE_P(sql) != IS_STRING) {
        ZVAL_EMPTY_STRING(sql);
    }

    va_start(args, format);
    msg_len = vspprintf(&msg, 0, format, args);
    va_end(args);

    pmsg_len = spprintf(&pmsg, 0, "%s%s%s", Z_STRVAL_P(sql), Z_STRLEN_P(sql) ? " " : "",  msg);

    zend_update_property_stringl(asf_db_absqb_ce, db, ZEND_STRL(ASF_DB_ABSQB_PRONAME_SQL), pmsg, pmsg_len);

    efree(msg);  msg = NULL;
    efree(pmsg); pmsg = NULL;

    (void)asf_db_absqb_clear_where_set(db);
}/*}}}*/

zend_string *asf_db_backquote_columns(zend_string *cols) /*{{{*/
{
    /* filter space(php_trim) */
    zend_string *strim = php_trim(cols, NULL, 0, 3);

    /* Exclude `cols`, id.cols, '*' */
    if (!strim || ZSTR_VAL(strim)[0] == '`' || ZSTR_VAL(strim)[0] == '*'
            || strchr(ZSTR_VAL(strim), '.')) {
        return strim;
    }

	zend_string *sret = zend_string_alloc(ZSTR_LEN(strim) + 2, 0);
    memcpy(ZSTR_VAL(sret), "`", 1);
    memcpy(ZSTR_VAL(sret) + 1, ZSTR_VAL(strim), ZSTR_LEN(strim));
    memcpy(ZSTR_VAL(sret) + 1 + ZSTR_LEN(strim), "`", 1);
	ZSTR_VAL(sret)[ZSTR_LEN(strim) + 2] = '\0';

    zend_string_release(strim);

    return sret;
}
/*}}}*/

/* {{{ proto object Asf_Db_AbstractQueryBuilder::where(string $columns, mixed $value [, string $operator = '='])
*/
PHP_METHOD(asf_db_absqb, where)
{
    zend_string *columns = NULL, *oprator = NULL, *cols_ret = NULL;
    zend_long is_condition = 0; char *condition = NULL;
    char *tmp = NULL;
    size_t tmp_len = 0;
    zval *value = NULL;
    zval *self = NULL, *sql = NULL, *bind_value = NULL, *where = NULL, *set = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz|Sl", &columns, &value, &oprator, &is_condition) == FAILURE) {
        return;
    }

    self = getThis();
    (void)asf_db_absqb_start(self, &sql, &bind_value, &where);

    set = zend_read_property(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_SET), 1, NULL);

    /* operator: having, set, where, or */
    switch (is_condition) {
        case 2:
            condition = Z_TYPE_P(set) == IS_FALSE ? ASF_DB_ABSQB_SET : ",";
            break;
        case 3:
            condition = ASF_DB_ABSQB_HAVING;
            break;
        default:
            condition = (Z_TYPE_P(where) == IS_FALSE) ? ASF_DB_ABSQB_WHERE
                : (is_condition ? ASF_DB_ABSQB_OR : ASF_DB_ABSQB_AND);
            break;
    }

    cols_ret = asf_db_backquote_columns(columns);
    tmp_len = spprintf(&tmp, 0, "%s %s %s %s ?", Z_STRVAL_P(sql)
            , condition
            , ZSTR_VAL(cols_ret)
            , oprator ? ZSTR_VAL(oprator) : ASF_DB_ABSQB_EQUAL);

    (void)asf_db_absqb_end(self, value, bind_value, where, tmp, tmp_len);

    zend_string_release(cols_ret);
    efree(tmp); tmp = NULL;

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto object Asf_Db_AbstractQueryBuilder::wheres(array $array1 [, array $... ])
*/
ASF_DB_ABSQB_SETS_WHERES(wheres, "where", 5);
/* }}} */

/* {{{ proto object Asf_Db_AbstractQueryBuilder::sets(array $array1 [, array $... ])
*/
ASF_DB_ABSQB_SETS_WHERES(sets, "set", 3);
/* }}} */

/* {{{ proto object Asf_Db_AbstractQueryBuilder::orWhere(string $columns, mixed $value [, string $operator = '='])
*/
PHP_METHOD(asf_db_absqb, orWhere)
{
    zend_string *columns = NULL, *oprator = NULL;
    zval *value = NULL;
    zval args[4], ret;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz|S", &columns, &value, &oprator) == FAILURE) {
        return;
    }

    /* Call the function 'where' */
    ZVAL_STR(&args[0], columns);
    ZVAL_COPY_VALUE(&args[1], value);
    if (oprator) {
        ZVAL_STR_COPY(&args[2], oprator);
    } else {
        ZVAL_STRINGL(&args[2], "=", 1);
    }
    ZVAL_LONG(&args[3], 1);

    ASF_CALL_USER_FUNCTION_EX(getThis(), "where", 5, return_value, 4, args);

    zval_ptr_dtor(&args[2]);
}
/* }}} */

/* {{{ proto object Asf_Db_AbstractQueryBuilder::whereIn(string $columns, array $value)
*/
PHP_METHOD(asf_db_absqb, whereIn)
{
    char *tmp = NULL;
    size_t tmp_len = 0;
    smart_str buf = {0};
    _Bool mod = 0;
    zend_bool is_or = 0;
    zend_string *columns = NULL, *cols_ret = NULL;
    zval *value = NULL;
    zval *self = NULL, *sql = NULL, *bind_value = NULL, *where = NULL, *entry = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sa|b", &columns, &value, &is_or) == FAILURE) {
        return;
    }

    self = getThis();
    (void)asf_db_absqb_start(self, &sql, &bind_value, &where);

    // if num = 0, then continue excute
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(value), entry) {
        if (mod) {
            smart_str_appendl(&buf, ",?", 2);
        } else {
            smart_str_appendl(&buf, "?", 1);
        }
        mod = 1;
    } ZEND_HASH_FOREACH_END();
    smart_str_0(&buf);

    cols_ret = asf_db_backquote_columns(columns);
    tmp_len = spprintf(&tmp, 0, "%s %s %s IN (%s)", Z_STRVAL_P(sql)
            , (Z_TYPE_P(where) == IS_FALSE) ? ASF_DB_ABSQB_WHERE : (is_or ? ASF_DB_ABSQB_OR : ASF_DB_ABSQB_AND)
            , ZSTR_VAL(cols_ret), ZSTR_VAL(buf.s));

    (void)asf_db_absqb_end(self, value, bind_value, where, tmp, tmp_len);

    zend_string_release(cols_ret);
    efree(tmp); tmp = NULL;
    smart_str_free(&buf);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto object Asf_Db_AbstractQueryBuilder::orWhereIn(string $columns, array $value)
*/
PHP_METHOD(asf_db_absqb, orWhereIn)
{
    zend_string *columns = NULL;
    zval *value = NULL;
    zval args[3];

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sa", &columns, &value) == FAILURE) {
        return;
    }

    ZVAL_STR(&args[0], columns);
    ZVAL_COPY_VALUE(&args[1], value);
    ZVAL_BOOL(&args[2], 1);

    ASF_CALL_USER_FUNCTION_EX(getThis(), "whereIn", 7, return_value, 3, args);
}
/* }}} */

/* {{{ proto object Asf_Db_AbstractQueryBuilder::from(string $name [, string $alias_name = ''])
*/
PHP_METHOD(asf_db_absqb, from)
{
    zend_string *name = NULL, *alias_name = NULL, *cols_ret = NULL;;
    zval *self = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &name, &alias_name) == FAILURE) {
        return;
    }

    self = getThis();
    cols_ret = asf_db_backquote_columns(name);

    (void)asf_db_absqb_sql_format(self, "FROM %s%s%s", ZSTR_VAL(cols_ret)
            , alias_name ? " " : ""
            , alias_name ? ZSTR_VAL(alias_name) : "");
    
    zend_string_release(cols_ret);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto object Asf_Db_AbstractQueryBuilder::set(string $name, string $value)
*/
PHP_METHOD(asf_db_absqb, set)
{
    zend_string *columns = NULL;
    zval *value = NULL, *self = NULL;
    zval args[4], ret;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &columns, &value) == FAILURE) {
        return;
    }

    self = getThis();

    /* Call the function 'where' */
    ZVAL_STR(&args[0], columns);
    ZVAL_COPY_VALUE(&args[1], value);
    ZVAL_STRINGL(&args[2], "=", 1);
    ZVAL_LONG(&args[3], 2);

    /* reset property where */
    zend_update_property_bool(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_WHERE), 0);

    ASF_CALL_USER_FUNCTION_EX(self, "where", 5, return_value, 4, args);

    /* Important: clear property where */
    zend_update_property_bool(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_SET), 1);
    zend_update_property_bool(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_WHERE), 0);

    zval_ptr_dtor(&args[2]);
}
/* }}} */

/* {{{ proto object Asf_Db_AbstractQueryBuilder::limit(int $start_page, int $end_page)
*/
PHP_METHOD(asf_db_absqb, limit)
{
    zend_string *start = NULL, *end = NULL;
    zval *sql = NULL, *self = NULL;
    smart_str buf = {0};

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &start, &end) == FAILURE) {
        return;
    }

    self = getThis();
    sql = zend_read_property(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_SQL), 1, NULL);

    if (Z_TYPE_P(sql) != IS_STRING) {
        RETURN_FALSE;
    }

    smart_str_appendl(&buf, Z_STRVAL_P(sql), Z_STRLEN_P(sql));
    smart_str_appendl(&buf, " LIMIT ", 7);
    smart_str_appendl(&buf, ZSTR_VAL(start), ZSTR_LEN(start));
    
    /* support limit start, end*/
    if (end) {
        smart_str_appendl(&buf, ", ", 2);
        smart_str_appendl(&buf, ZSTR_VAL(end), ZSTR_LEN(end));
    }
    smart_str_0(&buf);

    zend_update_property_str(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_SQL), buf.s);
    smart_str_free(&buf);

    (void)asf_db_absqb_clear_where_set(self);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto object Asf_Db_AbstractQueryBuilder::like(string $columns, mixed $value)
*/
PHP_METHOD(asf_db_absqb, like)
{
    zend_string *col = NULL, *cols_ret = NULL;
    zval *value = NULL, ztmp;
    char *tmp = NULL;
    size_t tmp_len = 0;
    zval *self = NULL, *sql = NULL, *bind_value = NULL, *where = NULL;
    _Bool is_like = 1;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz|b", &col, &value, &is_like) == FAILURE) {
        return;
    }

    self = getThis();
    (void)asf_db_absqb_start(self, &sql, &bind_value, &where);

    /* operator: like, not like */
    cols_ret = asf_db_backquote_columns(col);
    tmp_len = spprintf(&tmp, 0, "%s %s %s %s ?"
            , Z_STRVAL_P(sql)
            , ((Z_TYPE_P(where) == IS_FALSE) ? ASF_DB_ABSQB_WHERE : ASF_DB_ABSQB_AND)
            , ZSTR_VAL(cols_ret)
            , is_like ?  "LIKE" : "NOT LIKE");

    (void)asf_db_absqb_end(self, value, bind_value, where, tmp, tmp_len);

    zend_string_release(cols_ret);
    efree(tmp); tmp = NULL;

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto object Asf_Db_AbstractQueryBuilder::notLike(string $columns, mixed $value)
*/
PHP_METHOD(asf_db_absqb, notLike)
{
    zend_string *col = NULL;
    zval *value = NULL;
    zval args[3];

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &col, &value) == FAILURE) {
        return;
    }

    ZVAL_STR(&args[0], col);
    ZVAL_COPY_VALUE(&args[1], value);
    ZVAL_BOOL(&args[2], 0);

    ASF_CALL_USER_FUNCTION_EX(getThis(), "like", 4, return_value, 3, args);
}
/* }}} */

/* {{{ proto object Asf_Db_AbstractQueryBuilder::between(string $columns, mixed $a, mixed $b)
*/
PHP_METHOD(asf_db_absqb, between)
{
    zend_string *col = NULL, *cols_ret = NULL;
    zval *a = NULL, *b = NULL;
    char *tmp = NULL;
    size_t tmp_len = 0;
    zval *self = NULL, *sql = NULL, *bind_value = NULL, *where = NULL;
    _Bool is_between = 1;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Szz|b", &col, &a, &b, &is_between) == FAILURE) {
        return;
    }

    self = getThis();
    (void)asf_db_absqb_start(self, &sql, &bind_value, &where);

    /* operator: like, not like */
    cols_ret = asf_db_backquote_columns(col);
    tmp_len = spprintf(&tmp, 0, "%s %s %s %s ? AND ?"
            , Z_STRVAL_P(sql)
            , ((Z_TYPE_P(where) == IS_FALSE) ? ASF_DB_ABSQB_WHERE : ASF_DB_ABSQB_AND)
            , ZSTR_VAL(cols_ret)
            , is_between ?  "BETWEEN" : "NOT BETWEEN");

    (void)asf_db_absqb_end(self, a, bind_value, where, tmp, tmp_len);
    (void)asf_db_absqb_end(self, b, bind_value, where, tmp, tmp_len);

    zend_string_release(cols_ret);
    efree(tmp); tmp = NULL;

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto object Asf_Db_AbstractQueryBuilder::notBetween(string $columns, mixed $a, mixed $b)
*/
PHP_METHOD(asf_db_absqb, notBetween)
{
    zend_string *col = NULL;
    zval *a = NULL, *b = NULL;
    zval args[4];

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Szz|b", &col, &a, &b) == FAILURE) {
        return;
    }

    ZVAL_STR(&args[0], col);
    ZVAL_COPY_VALUE(&args[1], a);
    ZVAL_COPY_VALUE(&args[2], b);
    ZVAL_BOOL(&args[3], 0);

    ASF_CALL_USER_FUNCTION_EX(getThis(), "between", 7, return_value, 4, args);
}
/* }}} */

/* {{{ proto object Asf_Db_AbstractQueryBuilder::cm(string $data)
    (custom => cm) Manually write sql */
PHP_METHOD(asf_db_absqb, cm)
{
    zend_string *cm = NULL;
    zval *sql = NULL, *self = NULL;
    smart_str buf = {0};

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &cm) == FAILURE) {
        return;
    }

    self = getThis();
    sql = zend_read_property(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_SQL), 1, NULL);

    /* The sql value, must to be string */
    if (Z_TYPE_P(sql) == IS_STRING) {
        smart_str_appendl(&buf, Z_STRVAL_P(sql), Z_STRLEN_P(sql));
        smart_str_appendl(&buf, " ", 1);
    }

    smart_str_appendl(&buf, ZSTR_VAL(cm), ZSTR_LEN(cm));
    smart_str_0(&buf);

    zend_update_property_str(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_SQL), buf.s);
    smart_str_free(&buf);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto string Asf_Db_AbstractQueryBuilder::show(void)
*/
PHP_METHOD(asf_db_absqb, show)
{
    zval *sql = NULL, *bind_value = NULL;
    smart_str buf = {0};
    zval *self = NULL, impl_ret;
    zend_string *impl = NULL;

    self = getThis();
    sql = zend_read_property(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_SQL), 1, NULL);
    bind_value = zend_read_property(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_BINDVALUE), 1, NULL);

    if (Z_TYPE_P(sql) != IS_STRING) {
        RETURN_FALSE;
    }

    if (asf_func_isempty(Z_STRVAL_P(sql))) {
        RETURN_FALSE;
    }

    smart_str_appendl(&buf, "[SQL] ", 6);
    smart_str_append(&buf, Z_STR_P(sql));

    if (Z_TYPE_P(bind_value) == IS_ARRAY) {
        impl = zend_string_init(" , ", 3, 0);
        php_implode(impl, bind_value, &impl_ret);

        smart_str_appendl(&buf, " [bindValues] ", 14);
        smart_str_append(&buf, Z_STR_P(&impl_ret));

        zend_string_release(impl);
        zval_ptr_dtor(&impl_ret);
    }

    smart_str_0(&buf);
    RETVAL_STR(buf.s);
}
/* }}} */

/* {{{ proto string Asf_Db_AbstractQueryBuilder::getSql(void)
*/
PHP_METHOD(asf_db_absqb, getSql)
{
    ZVAL_COPY(return_value, zend_read_property(asf_db_absqb_ce
                , getThis(), ZEND_STRL(ASF_DB_ABSQB_PRONAME_SQL), 1, NULL));
}/*}}}*/

/* {{{ proto array Asf_Db_AbstractQueryBuilder::getValues(void)
*/
PHP_METHOD(asf_db_absqb, getValues)
{
    ZVAL_COPY(return_value, zend_read_property(asf_db_absqb_ce
                , getThis(), ZEND_STRL(ASF_DB_ABSQB_PRONAME_BINDVALUE), 1, NULL));
}/*}}}*/

/* {{{ proto bool Asf_Db_AbstractQueryBuilder::exec([bool $flags = 0]) 
*/
PHP_METHOD(asf_db_absqb, exec)
{
    zval *sql = NULL, *bind_value = NULL;
    zval *self = NULL, *cur_link = NULL;
    zval function_name;
    zend_long curd = 0;
    uint params_i = 2;
    zval params[params_i];
    zend_bool flags = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &flags) == FAILURE) {
        return;
    }

    self = getThis();
    sql = zend_read_property(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_SQL), 1, NULL);
    bind_value = zend_read_property(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_BINDVALUE), 1, NULL);
    curd = Z_LVAL_P(zend_read_property(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_CURD), 1, NULL));

    if (Z_TYPE_P(sql) != IS_STRING) {
        RETURN_FALSE;
    }

    if (asf_func_isempty(Z_STRVAL_P(sql))) {
        RETURN_FALSE;
    }

    cur_link = zend_read_static_property(asf_Db_ce, ZEND_STRL(CLINK), 1);

    if (IS_OBJECT != Z_TYPE_P(cur_link)) {
        RETURN_FALSE;
    }

    switch (curd) {
        case ASF_DB_ABSQB_SELECT:
            ZVAL_STRINGL(&function_name, flags ? "findOne" : "findAll", 7);
            break;
        default:
            ZVAL_STRINGL(&function_name, flags ? "exeQuery" : "exeNoQuery", flags ? 8 : 10);
            break;
    }

    if (Z_TYPE_P(bind_value) == IS_NULL) {
        params_i = 1;
    }

    ZVAL_COPY_VALUE(&params[0], sql);
    ZVAL_COPY_VALUE(&params[1], bind_value);

    call_user_function_ex(&Z_OBJCE_P(cur_link)->function_table, cur_link, &function_name, return_value, params_i, params, 1, NULL);
    zval_ptr_dtor(&function_name);

    if (Z_TYPE_P(return_value) == IS_UNDEF) {
        RETURN_FALSE;
    }

    zend_call_method_with_0_params(self, Z_OBJCE_P(self), NULL, "clear", NULL);
}
/* }}} */

/* {{{ proto bool Asf_Db_AbstractQueryBuilder::clear(void)
*/
PHP_METHOD(asf_db_absqb, clear)
{
    zval *self = getThis();

    zend_update_property_null(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_SQL));
    zend_update_property_null(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_BINDVALUE));
    zend_update_property_bool(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_WHERE), 0);
    zend_update_property_bool(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_SET), 0);

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto object Asf_Db_AbstractQueryBuilder::table(string $name [, string $alias_name = ''])
*/
PHP_METHOD(asf_db_absqb, table)
{
    zend_string *name = NULL, *cols_ret = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
        return;
    }

    zval *self = getThis();
    cols_ret = asf_db_backquote_columns(name);

    (void)asf_db_absqb_sql_format(self, ZSTR_VAL(cols_ret));
    zend_string_release(cols_ret);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ asf_db_absqb_methods[]
*/
zend_function_entry asf_db_absqb_methods[] = {
    PHP_ME(asf_db_absqb, where,        asf_db_absqb_where_arginfo,     ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, wheres,       asf_db_absqb_wheres_arginfo,    ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, orWhere,      asf_db_absqb_where_arginfo,     ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, whereIn,      asf_db_absqb_wherein_arginfo,   ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, orWhereIn,    asf_db_absqb_wherein_arginfo,   ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, from,         asf_db_absqb_from_arginfo,      ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, table,        asf_db_absqb_table_arginfo,     ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, set,          asf_db_absqb_set_arginfo,       ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, sets,         asf_db_absqb_set_arginfo,       ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, limit,        asf_db_absqb_limit_arginfo,     ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, like,         asf_db_absqb_set_arginfo,       ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, notLike,      asf_db_absqb_set_arginfo,       ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, between,      asf_db_absqb_between_arginfo,   ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, notBetween,   asf_db_absqb_between_arginfo,   ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, cm,           asf_db_absqb_cm_arginfo,        ZEND_ACC_PUBLIC)

    PHP_ME(asf_db_absqb, show,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, exec,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, clear,     NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, getSql,    NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_absqb, getValues, NULL, ZEND_ACC_PUBLIC)

    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(db_absqb) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_db_absqb, Asf_Db_AbstractQueryBuilder
            , Asf\\Db\\AbstractQueryBuilder, ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);

    zend_declare_property_null(asf_db_absqb_ce, ZEND_STRL(ASF_DB_ABSQB_PRONAME_SQL), ZEND_ACC_PROTECTED);
    zend_declare_property_null(asf_db_absqb_ce, ZEND_STRL(ASF_DB_ABSQB_PRONAME_BINDVALUE), ZEND_ACC_PROTECTED);
    
    zend_declare_property_bool(asf_db_absqb_ce, ZEND_STRL(ASF_DB_ABSQB_PRONAME_WHERE), 0, ZEND_ACC_PROTECTED);
    zend_declare_property_bool(asf_db_absqb_ce, ZEND_STRL(ASF_DB_ABSQB_PRONAME_SET), 0, ZEND_ACC_PROTECTED);

    zend_declare_property_long(asf_db_absqb_ce, ZEND_STRL(ASF_DB_ABSQB_PRONAME_CURD), 0, ZEND_ACC_PROTECTED);

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
