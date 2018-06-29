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
#include "asf_exception.h"
#include "http/asf_http_cookieinterface.h"

zend_class_entry *asf_http_cookieinterface_ce;

/* {{{ asf_http_cookieinterface_methods[]
*/
zend_function_entry asf_http_cookieinterface_methods[] = {
    PHP_ABSTRACT_ME(asf_http_cookieinterface, init, asf_http_cookieinterface_init_arginfo)
    PHP_ABSTRACT_ME(asf_http_cookieinterface, prefix, asf_http_cookieinterface_prefix_arginfo)
    PHP_ABSTRACT_ME(asf_http_cookieinterface, set, asf_http_cookieinterface_set_arginfo)
    PHP_ABSTRACT_ME(asf_http_cookieinterface, forever, asf_http_cookieinterface_set_arginfo)
    PHP_ABSTRACT_ME(asf_http_cookieinterface, has, asf_http_cookieinterface_has_arginfo)
    PHP_ABSTRACT_ME(asf_http_cookieinterface, get, asf_http_cookieinterface_get_arginfo)
    PHP_ABSTRACT_ME(asf_http_cookieinterface, del, asf_http_cookieinterface_del_arginfo)
    PHP_ABSTRACT_ME(asf_http_cookieinterface, clear, NULL)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(http_cookieinterface) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERFACE(asf_http_cookieinterface, Asf_Http_CookieInterface, Asf\\Http\\CookieInterface);

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
