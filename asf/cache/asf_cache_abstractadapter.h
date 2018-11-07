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

#ifndef ASF_CACHE_ABSADAPTER_H
#define ASF_CACHE_ABSADAPTER_H

//#define ASF_DB_TYPE_MYSQL	 "mysql"  // adapter_id=0
//#define ASF_DB_TYPE_SQLTILE  "sqlite" // adapter_id=1
//#define ASF_DB_TYPE_PGSQL	 "pgsql"  // adapter_id=2

/* {{{ ARG_INFO
 */
/*
ZEND_BEGIN_ARG_INFO_EX(asf_absadapter_call_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, function_name)
    ZEND_ARG_ARRAY_INFO(0, arguments, 1)
ZEND_END_ARG_INFO()
*/
/* }}} */

//_Bool asf_db_absadapter_instance(asf_db_t *this_ptr, const char *type, zval *configs);

extern zend_class_entry *asf_cache_absadapter_ce;

ASF_INIT_CLASS(cache_absadapter);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
