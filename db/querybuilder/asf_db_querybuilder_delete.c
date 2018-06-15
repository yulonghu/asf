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
#include "asf_exception.h"
#include "kernel/asf_func.h"

#include "db/asf_db_abstractquerybuilder.h"
#include "db/querybuilder/asf_db_querybuilder_delete.h"

zend_class_entry *asf_db_qb_delete_ce;

/* {{{ proto object Asf_Db_QueryBuilder_Delete::__construct(void)
 */
PHP_METHOD(asf_db_qb_delete, __construct)
{
    zval *self = getThis();

    (void)asf_db_absqb_sql_format(self, "DELETE");

    zend_update_property_long(Z_OBJCE_P(self), self, ZEND_STRL(ASF_DB_ABSQB_PRONAME_CURD), ASF_DB_ABSQB_DELETE);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ asf_db_qb_delete_methods[]
*/
zend_function_entry asf_db_qb_delete_methods[] = {
    PHP_ME(asf_db_qb_delete, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_MALIAS(asf_db_qb_delete, delete, __construct, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(db_qb_delete) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERNAL(asf_db_qb_delete, Asf_Db_QueryBuilder_Delete, Asf\\Db\\QueryBuilder\\Delete, asf_db_absqb_ce, asf_db_qb_delete_methods);

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
