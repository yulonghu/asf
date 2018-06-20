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
#include "db/asf_db_adapterinterface.h"

zend_class_entry *asf_db_adapterinterface_ce;

/* {{{ asf_db_methods
*/
zend_function_entry asf_db_adapterinterface_methods[] = {
    PHP_ABSTRACT_ME(asf_db_adapterinterface, insert, asf_db_adapterinterface_insert_arginfo)
    PHP_ABSTRACT_ME(asf_db_adapterinterface, insertIgnore, asf_db_adapterinterface_insert_arginfo)
    PHP_ABSTRACT_ME(asf_db_adapterinterface, update, asf_db_adapterinterface_update_arginfo)
    PHP_ABSTRACT_ME(asf_db_adapterinterface, delete, asf_db_adapterinterface_delete_arginfo)

    PHP_ABSTRACT_ME(asf_db_adapterinterface, findOne, asf_db_adapterinterface_findone_arginfo)
    PHP_ABSTRACT_ME(asf_db_adapterinterface, findAll, asf_db_adapterinterface_findone_arginfo)

    PHP_ABSTRACT_ME(asf_db_adapterinterface, findOneBy, asf_db_adapterinterface_findby_arginfo)
    PHP_ABSTRACT_ME(asf_db_adapterinterface, findAllBy, asf_db_adapterinterface_findby_arginfo)

    PHP_ABSTRACT_ME(asf_db_adapterinterface, exeNoQuery, asf_db_adapterinterface_query_arginfo)
    PHP_ABSTRACT_ME(asf_db_adapterinterface, exeQuery, asf_db_adapterinterface_query_arginfo)

    PHP_ABSTRACT_ME(asf_db_adapterinterface, addByArray, asf_db_adapterinterface_addbyarray_arginfo)
    PHP_ABSTRACT_ME(asf_db_adapterinterface, addDupByArray, asf_db_adapterinterface_adddupbyarray_arginfo)

    PHP_ABSTRACT_ME(asf_db_adapterinterface, getCount, asf_db_adapterinterface_getcount_arginfo)

    PHP_ABSTRACT_ME(asf_db_adapterinterface, setTable, asf_db_adapterinterface_settable_arginfo)
    PHP_ABSTRACT_ME(asf_db_adapterinterface, getTable, NULL)

    PHP_ABSTRACT_ME(asf_db_adapterinterface, close, NULL)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(db_adapterinterface) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERFACE(asf_db_adapterinterface, Asf_Db_AdapterInterface, Asf\\Db\\AdapterInterface);

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
