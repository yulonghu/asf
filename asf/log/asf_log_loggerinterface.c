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
#include "asf_log_loggerinterface.h"
#include "asf_exception.h"

zend_class_entry *asf_log_loggerinterface_ce;

/* {{{ asf_log_loggerinterface_methods[]
*/
zend_function_entry asf_log_loggerinterface_methods[] = {
    PHP_ABSTRACT_ME(asf_log_loggerinterface, emergency,  asf_log_loggerinterface_common_arginfo)
    PHP_ABSTRACT_ME(asf_log_loggerinterface, alert,	     asf_log_loggerinterface_common_arginfo)
    PHP_ABSTRACT_ME(asf_log_loggerinterface, critical,   asf_log_loggerinterface_common_arginfo)
    PHP_ABSTRACT_ME(asf_log_loggerinterface, error,	     asf_log_loggerinterface_common_arginfo)
    PHP_ABSTRACT_ME(asf_log_loggerinterface, warning,	 asf_log_loggerinterface_common_arginfo)
    PHP_ABSTRACT_ME(asf_log_loggerinterface, notice,	 asf_log_loggerinterface_common_arginfo)
    PHP_ABSTRACT_ME(asf_log_loggerinterface, info,		 asf_log_loggerinterface_common_arginfo)
    PHP_ABSTRACT_ME(asf_log_loggerinterface, debug,	     asf_log_loggerinterface_common_arginfo)
    PHP_ABSTRACT_ME(asf_log_loggerinterface, log,		 asf_log_loggerinterface_log_arginfo)

    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(log_loggerinterface) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERFACE(asf_log_loggerinterface, Asf_Log_LoggerInterface, Asf\\Log\\LoggerInterface);

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
