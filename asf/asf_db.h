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

#ifndef ASF_DB_H
#define ASF_DB_H

#define LINKS  "_links"
#define CLINK  "_clink"

#define ASF_DB_QB_METHOD(ce, method) \
    PHP_METHOD(asf_Db, method) \
    { \
        zval zmethod; \
        object_init_ex(&zmethod, asf_db_qb_ ##ce## _ce); \
        zend_call_method_with_0_params(&zmethod, asf_db_qb_ ##ce## _ce, NULL, "__construct", return_value); \
        zval_ptr_dtor(&zmethod); \
    }

extern zend_class_entry *asf_Db_ce;

ASF_INIT_CLASS(Db);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
