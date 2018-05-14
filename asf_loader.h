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

#ifndef ASF_LOADER_H
#define ASF_LOADER_H

#define ASF_LOADER_PROPERTY_NAME_INSTANCE	"_ins"

#define ASF_LOADER_SERVICE_DIRECTORY_NAME	"services"
#define ASF_LOADER_LOGIC_DIRECTORY_NAME		"logics"
#define ASF_LOADER_DAO_DIRECTORY_NAME		"daos"
#define ASF_LOADER_MODULE_DIRECTORY_NAME    "modules"
#define ASF_LOADER_LIBRARY_DIRECTORY_NAME	"library"

#define ASF_AUTOLOAD_FUNC_NAME				"autoload"
#define ASF_AUTOLOAD_FUNC_NAME_LEN          8

#define ASF_LOADER_SERVER			"Service"
#define ASF_LOADER_SERVER_LEN		7
#define ASF_LOADER_LOGIC			"Logic"
#define ASF_LOADER_LOGIC_LEN		5
#define ASF_LOADER_DAO				"Dao"
#define ASF_LOADER_DAO_LEN			3

extern zend_class_entry *asf_loader_ce;

zval *asf_loader_instance(zval *this_ptr, zend_string *library_path);
_Bool asf_loader_import(zend_string *path, zval *return_value_ptr);
_Bool asf_internal_autoload(char *file_name, size_t name_len, char **root_path);

ASF_INIT_CLASS(loader);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
