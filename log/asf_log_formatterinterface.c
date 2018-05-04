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
#include "log/asf_log_formatterinterface.h"

zend_class_entry *asf_log_formatterinterface_ce;

/* {{{ asf_log_formatterinterface_methods[]
*/
zend_function_entry asf_log_formatterinterface_methods[] = {
    PHP_ABSTRACT_ME(asf_log_formatterinterface, format, asf_log_formatterinterface_f_arginfo)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(log_formatterinterface) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERFACE(asf_log_formatterinterface, Asf_Log_FormatterInterface, Asf\\Log\\FormatterInterface);

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
