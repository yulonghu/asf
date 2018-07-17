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

#ifndef ASF_ABSQB_H
#define ASF_ABSQB_H

#define ASF_DB_ABSQB_PRONAME_SQL        "s"
#define ASF_DB_ABSQB_PRONAME_BINDVALUE  "bv"
#define ASF_DB_ABSQB_PRONAME_WHERE      "w"
#define ASF_DB_ABSQB_PRONAME_SET        "se"
#define ASF_DB_ABSQB_PRONAME_CURD       "curd"

#define ASF_DB_ABSQB_SPACE      " "
#define ASF_DB_ABSQB_WHERE      "WHERE"
#define ASF_DB_ABSQB_AND        "AND"
#define ASF_DB_ABSQB_OR         "OR"
#define ASF_DB_ABSQB_EQUAL      "="
#define ASF_DB_ABSQB_SET        "SET"
#define ASF_DB_ABSQB_HAVING     "HAVING"

enum sql_method {
    ASF_DB_ABSQB_INSERT,
    ASF_DB_ABSQB_UPDATE,
    ASF_DB_ABSQB_SELECT,
    ASF_DB_ABSQB_DELETE,
    ASF_DB_ABSQB_INSERT_IGNORE,
};

void asf_db_absqb_clear_where_set(asf_db_t *db);
void asf_db_absqb_sql_format(asf_db_t *db, char *format, ...);
zend_string *asf_db_backquote_columns(zend_string *cols);
void asf_db_absqb_set_header(zval *self, zend_long header_id);

#define ASF_DB_ABSQB_SETS_WHERES(method, cm, cm_len) \
    PHP_METHOD(asf_db_absqb, method) \
    { \
        zval *cols = NULL; \
        if (zend_parse_parameters(ZEND_NUM_ARGS(), "|a", &cols) == FAILURE) { \
            return; \
        } \
        zval *self = getThis(); \
        if (cols && zend_hash_num_elements(Z_ARRVAL_P(cols)) > 0) { \
            zend_string *key = NULL; \
            zval *entry = NULL; \
            zval args[2], ret; \
            ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(cols), key, entry) { \
                if (!key) { \
                    continue; \
                } \
                ZVAL_STR(&args[0], key); \
                ZVAL_COPY_VALUE(&args[1], entry); \
                ASF_CALL_USER_FUNCTION_EX(self, cm, cm_len, &ret, 2, args); \
                zval_ptr_dtor(&ret); \
                ZVAL_UNDEF(&args[0]); \
                ZVAL_UNDEF(&args[1]); \
            } ZEND_HASH_FOREACH_END(); \
        } \
        RETURN_ZVAL(self, 1, 0); \
    }

extern zend_class_entry *asf_db_absqb_ce;

ASF_INIT_CLASS(db_absqb);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
