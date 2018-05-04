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

#ifndef ASF_DB_ADAPTERINTERFACE_H
#define ASF_DB_ADAPTERINTERFACE_H

/* {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(asf_db_adapterinterface_construct_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, configs, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(asf_db_adapterinterface_insert_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, data, 0)
    ZEND_ARG_INFO(0, ret_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(asf_db_adapterinterface_update_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, data, 0)
    ZEND_ARG_ARRAY_INFO(0, condition, 1)
    ZEND_ARG_INFO(0, limit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(asf_db_adapterinterface_delete_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, condition, 0)
    ZEND_ARG_INFO(0, limit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(asf_db_adapterinterface_findone_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_ARRAY_INFO(0, bind_value, 1)
    ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(asf_db_adapterinterface_findby_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, pairs, 1)
    ZEND_ARG_ARRAY_INFO(0, fields_name, 0)
    ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(asf_db_adapterinterface_query_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, sql)
    ZEND_ARG_ARRAY_INFO(0, bind_value, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(asf_db_adapterinterface_addbyarray_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, pairs, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(asf_db_adapterinterface_adddupbyarray_arginfo, 0, 0, 2)
    ZEND_ARG_ARRAY_INFO(0, pairs, 1)
    ZEND_ARG_ARRAY_INFO(0, update_cols, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(asf_db_adapterinterface_getcount_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, sql)
    ZEND_ARG_ARRAY_INFO(0, bind_value, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(asf_db_adapterinterface_settable_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
/* }}}*/

extern zend_class_entry *asf_db_adapterinterface_ce;

ASF_INIT_CLASS(db_adapterinterface);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
