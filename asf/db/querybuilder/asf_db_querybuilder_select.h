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

/* QueryBuilder => QB */
#ifndef ASF_QB_SELECT_H
#define ASF_QB_SELECT_H

#define ASF_DB_QB_SELECT_DESC "DESC"
#define ASF_DB_QB_SELECT_ASC  "ASC"

extern zend_class_entry *asf_db_qb_select_ce;

#define ASF_DB_QB_SELECT_METHOD(method, keyword, type) \
    PHP_METHOD(asf_db_qb_select, method) \
    { \
        zend_string *col = NULL, *alias_col = NULL, *cols_ret = NULL; \
        zval *self = NULL, *sql = NULL; \
        char *tmp = NULL; \
        size_t tmp_len = 0; \
        if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &col, &alias_col) == FAILURE) { \
            return; \
        } \
        self = getThis(); \
        cols_ret = asf_db_backquote_columns(col); \
        switch(type) { \
            case 0: \
                (void)asf_db_absqb_sql_format(self, ""keyword" %s %s", ZSTR_VAL(cols_ret), alias_col ? ZSTR_VAL(alias_col) : ""); \
                break; \
            case 1: \
                (void)asf_db_absqb_sql_format(self, "%s(%s) %s", #method, ZSTR_VAL(cols_ret), alias_col ? ZSTR_VAL(alias_col) : ""); \
                break; \
            case 2: \
                (void)asf_db_absqb_sql_format(self, ""keyword" %s = %s", ZSTR_VAL(cols_ret), alias_col ? ZSTR_VAL(alias_col) : ""); \
                break; \
        } \
        zend_update_property_bool(asf_db_absqb_ce, self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_WHERE), 0); \
        zend_string_release(cols_ret); \
        RETURN_ZVAL(self, 1, 0); \
    }

ASF_INIT_CLASS(db_qb_select);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
