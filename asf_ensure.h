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

#ifndef ASF_ENSURE_H
#define ASF_ENSURE_H

#define ASF_ENSURE_FLAG_ISNULL		0
#define ASF_ENSURE_FLAG_NOTNULL		1
#define ASF_ENSURE_FLAG_ISEMPTY		2
#define ASF_ENSURE_FLAG_NOTEMPTY	3
#define ASF_ENSURE_FLAG_ISFALSE		4
#define ASF_ENSURE_FLAG_NOTFALSE	5
#define ASF_ENSURE_FLAG_ISTRUE		6

#define ASF_ENSURE_COMMON_METHOD(ce, x, flag) \
PHP_METHOD(ce, x) \
{ \
	zval *data = NULL; \
	zend_long code = 0; \
	char *class_name = NULL; \
	size_t class_name_len = 0; \
    ZEND_PARSE_PARAMETERS_START(2, 3) \
        Z_PARAM_ZVAL_DEREF(data) \
        Z_PARAM_LONG(code) \
        Z_PARAM_OPTIONAL \
        Z_PARAM_STRING(class_name, class_name_len) \
    ZEND_PARSE_PARAMETERS_END(); \
    switch (flag) { \
        case ASF_ENSURE_FLAG_ISNULL: \
             if (Z_TYPE_P(data) == IS_NULL) { \
                 RETURN_TRUE; \
             } \
        break; \
        case ASF_ENSURE_FLAG_NOTNULL: \
             if (Z_TYPE_P(data) != IS_NULL) { \
                RETURN_TRUE; \
             } \
        break; \
        case ASF_ENSURE_FLAG_ISEMPTY: \
             if (zend_is_true(data) == 0) { \
                RETURN_TRUE; \
             } \
        break; \
        case ASF_ENSURE_FLAG_NOTEMPTY: \
             if (zend_is_true(data) == 1) { \
                RETURN_TRUE; \
             } \
        break; \
        case ASF_ENSURE_FLAG_ISFALSE: \
             if (Z_TYPE_P(data) == IS_FALSE) { \
                RETURN_TRUE; \
             } \
        break; \
        case ASF_ENSURE_FLAG_NOTFALSE: \
              if (Z_TYPE_P(data) != IS_FALSE) { \
                RETURN_TRUE; \
              } \
        break; \
        case ASF_ENSURE_FLAG_ISTRUE: \
             if (Z_TYPE_P(data) == IS_TRUE) { \
                 RETURN_TRUE; \
             } \
        break; \
    } \
	(void)asf_ensure_write_stdout(class_name, class_name_len, code); \
} \

extern zend_class_entry *asf_ensure_ce;

ASF_INIT_CLASS(ensure);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
