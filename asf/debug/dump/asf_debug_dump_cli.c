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

#include "asf_debug_dump_abstractdump.h"
#include "asf_debug_dump_cli.h"

zend_class_entry *asf_debug_dump_cli_ce;

zval *asf_debug_dump_cli_instance(zval *this_ptr) /* {{{ */
{
    if (Z_ISUNDEF_P(this_ptr)) {
        object_init_ex(this_ptr, asf_debug_dump_cli_ce);
    }

    zend_update_property_long(asf_debug_dump_abstractdump_ce, this_ptr, ZEND_STRL(ASF_DEBUG_DUMP_PRONAME_MODE), 0);

    return this_ptr;
}
/* }}} */

/* {{{ proto object Asf_Debug_Dump_Cli::__construct()
*/
PHP_METHOD(asf_debug_dump_cli, __construct)
{
    (void)asf_debug_dump_cli_instance(getThis());
}
/** }}} */

/* {{{ asf_debug_dump_cli_methods[]
*/
zend_function_entry asf_debug_dump_cli_methods[] = {
    PHP_ME(asf_debug_dump_cli, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(debug_dump_cli) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERNAL(asf_debug_dump_cli, Asf_Debug_Dump_Cli, Asf\\Debug\\Dump\\Cli, asf_debug_dump_abstractdump_ce, asf_debug_dump_cli_methods);

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
