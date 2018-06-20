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
#include "Zend/zend_exceptions.h"

#include "php_asf.h"
#include "kernel/asf_namespace.h"
#include "asf_application.h"
#include "asf_exception.h"

zend_class_entry *asf_exception_ce;

void asf_trigger_error(int code, char *format, ...) /* {{{ */
{
    va_list args;
    char *message = NULL;
    uint msg_len = 0;

    /* if have a exception, then return */
    if (EG(exception)) {
        return;
    }

    va_start(args, format);
    msg_len = vspprintf(&message, 0, format, args);
    va_end(args);

    if (ASF_G(throw_exception)) {
        /* If PHP Fatal error, Check EG(current_execute_data) value */
        if (EG(current_execute_data)) {
            zend_function *active_function = EG(current_execute_data)->func;
            const char *class_name = active_function->common.scope ? ZSTR_VAL(active_function->common.scope->name) : "";
            zend_throw_exception_ex(asf_exception_ce, code, "%s%s%s() %s", class_name, class_name[0] ? "::" : ""
                    , ZSTR_VAL(active_function->common.function_name), message);
        }
    } else {
        php_error_docref(NULL, E_ERROR, "%s", message);
    }

    efree(message);
}
/* }}} */

ASF_INIT_CLASS(exception) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERNAL(asf_exception, Asf_Exception, Asf\\Exception, zend_ce_exception, NULL);

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
