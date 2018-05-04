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

#ifndef ASF_CONFIG_INI_H
#define ASF_CONFIG_INI_H

#define ASF_CONFIG_INI_PARSE_NAME		"parse_ini_file"
#define ASF_CONFIG_INI_PARSE_NAME_LEN	14	

extern zend_class_entry *asf_config_ini_ce;

zval *asf_config_ini_instance(zval *this_ptr, zval *file_path, zval *section);

ASF_INIT_CLASS(config_ini);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
