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
#include "http/asf_http_responseinterface.h"

zend_class_entry *asf_http_responseinterface_ce;

/* {{{ asf_http_responseinterface_methods[]
 */
zend_function_entry asf_http_responseinterface_methods[] = {
    PHP_ABSTRACT_ME(asf_http_responseinterface, setContentType, asf_http_repinterface_setcontenttype_arginfo)
    PHP_ABSTRACT_ME(asf_http_responseinterface, redirect, asf_http_repinterface_redirect_arginfo)
    PHP_ABSTRACT_ME(asf_http_responseinterface, setContent, asf_http_repinterface_setcontent_arginfo)
    PHP_ABSTRACT_ME(asf_http_responseinterface, appendContent, asf_http_repinterface_appendcontent_arginfo)
    PHP_ABSTRACT_ME(asf_http_responseinterface, getContent, NULL)
    PHP_ABSTRACT_ME(asf_http_responseinterface, send, NULL)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(responseinterface) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERFACE(asf_http_responseinterface, Asf_Http_ResponseInterface, Asf\\Http\\ResponseInterface);

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
