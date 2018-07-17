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
#include "kernel/asf_namespace.h"
#include "asf_exception.h"
#include "kernel/asf_func.h"

#include "db/asf_db_abstractquerybuilder.h"
#include "db/querybuilder/asf_db_querybuilder_insert.h"

zend_class_entry *asf_db_qb_insert_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_db_qb_insert_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, ignore)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_db_qb_insert_ondku_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, col)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Insert::__construct([bool $ignore = false])
 Init the class Asf_Db_QueryBuilder_Insert */
PHP_METHOD(asf_db_qb_insert, __construct)
{
    zend_bool bignore = 0;
    char *name = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &bignore) == FAILURE) {
        return;
    }

    zval *self = getThis();

    if (!bignore) {
        name = "INSERT INTO";
        (void)asf_db_absqb_set_header(self, ASF_DB_ABSQB_INSERT);
        zend_update_property_long(Z_OBJCE_P(self), self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_CURD), ASF_DB_ABSQB_INSERT);
    } else {
        name = "INSERT IGNORE INTO";
        (void)asf_db_absqb_set_header(self, ASF_DB_ABSQB_INSERT_IGNORE);
        zend_update_property_long(Z_OBJCE_P(self), self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_CURD), ASF_DB_ABSQB_INSERT_IGNORE);
    }

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto object Asf_Db_QueryBuilder_Insert::onDku(string $columns, mixed $value)
    onDku(string $columns, mixed $value) */
PHP_METHOD(asf_db_qb_insert, onDku)
{
    zend_string *col = NULL, *s = NULL, *cols_ret = NULL;
    zval *value = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &col, &value) == FAILURE) {
        return;
    }

    zval *self = getThis();
    s = zval_get_string(value);
    cols_ret = asf_db_backquote_columns(col);

    (void)asf_db_absqb_sql_format(self, "ON DUPLICATE KEY UPDATE %s = %s", ZSTR_VAL(cols_ret), ZSTR_VAL(s));

    zend_string_release(cols_ret);
    zend_string_release(s);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ asf_db_qb_insert_methods[]
*/
zend_function_entry asf_db_qb_insert_methods[] = {
    PHP_ME(asf_db_qb_insert, __construct, asf_db_qb_insert_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_MALIAS(asf_db_qb_insert, insert, __construct, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_db_qb_insert, onDku, asf_db_qb_insert_ondku_arginfo, ZEND_ACC_PUBLIC)

    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(db_qb_insert) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERNAL(asf_db_qb_insert, Asf_Db_QueryBuilder_Insert, Asf\\Db\\QueryBuilder\\Insert, asf_db_absqb_ce, asf_db_qb_insert_methods);

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
