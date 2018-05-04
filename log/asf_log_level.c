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
#include "asf_log_level.h"

zend_class_entry *asf_log_level_ce;

/* {{{ asf_log_level_methods[]
*/
zend_function_entry asf_log_level_methods[] = {
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(log_level) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_log_level, Asf_Log_Level, Asf\\Log\\Level, ZEND_ACC_FINAL);

    zend_declare_class_constant_stringl(asf_log_level_ce, ZEND_STRL("SPECIAL"), "special", 7);

    zend_declare_class_constant_stringl(asf_log_level_ce, ZEND_STRL("CUSTOM"), "custom", 6);

    zend_declare_class_constant_stringl(asf_log_level_ce, ZEND_STRL("DEBUG"), "debug", 5);

    zend_declare_class_constant_stringl(asf_log_level_ce, ZEND_STRL("INFO"), "info", 4);

    zend_declare_class_constant_stringl(asf_log_level_ce, ZEND_STRL("NOTICE"), "notice", 6);

    zend_declare_class_constant_stringl(asf_log_level_ce, ZEND_STRL("WARNING"), "warning", 7);

    zend_declare_class_constant_stringl(asf_log_level_ce, ZEND_STRL("ERROR"), "error", 5);

    zend_declare_class_constant_stringl(asf_log_level_ce, ZEND_STRL("CRITICAL"), "critical", 8);

    zend_declare_class_constant_stringl(asf_log_level_ce, ZEND_STRL("ALERT"), "alert", 4);

    zend_declare_class_constant_stringl(asf_log_level_ce, ZEND_STRL("EMERGENCY"), "emergency", 9);

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
