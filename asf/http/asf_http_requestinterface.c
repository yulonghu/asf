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
#include "http/asf_http_requestinterface.h"

zend_class_entry *asf_http_requestinterface_ce;

/* {{{ asf_http_requestinterface_methods
 */
zend_function_entry asf_http_requestinterface_methods[] = {
    /* The prefix is the has method for HTTP REQUEST */
    PHP_ABSTRACT_ME(asf_http_requestinterface, isGet,			NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, isPost,			NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, isPut,			NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, isDelete,		NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, isPatch,			NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, isHead,			NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, isOptions,		NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, isCli,			NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, isXmlHttpRequest,NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, isTrace,			NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, isConnect,		NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, isFlashRequest,	NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, isPropFind,		NULL)

    /* HTTP REQUEST */
    PHP_ABSTRACT_ME(asf_http_requestinterface, getParam,		asf_http_requestinterface_get_param_arginfo)
    PHP_ABSTRACT_ME(asf_http_requestinterface, getParams,		NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, getModuleName,	NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, getServiceName,	NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, getActionName,	NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, getMethod,		NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, getBaseUri,		NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, getRequestUri,	NULL)

    PHP_ABSTRACT_ME(asf_http_requestinterface, getQuery,		NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, getPost,			NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, getRequest,		NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, getFiles,		NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, getCookie,		NULL)
    PHP_ABSTRACT_ME(asf_http_requestinterface, getServer,		asf_http_requestinterface_getserver_arginfo)
    PHP_ABSTRACT_ME(asf_http_requestinterface, getEnv,			asf_http_requestinterface_getenv_arginfo)

    /* The prefix is the has method for $_REQUEST */
    PHP_ABSTRACT_ME(asf_http_requestinterface, hasPost,			asf_http_requestinterface_hasname_arginfo)
    PHP_ABSTRACT_ME(asf_http_requestinterface, hasQuery,		asf_http_requestinterface_hasname_arginfo)
    PHP_ABSTRACT_ME(asf_http_requestinterface, hasServer,		asf_http_requestinterface_hasname_arginfo)
    PHP_ABSTRACT_ME(asf_http_requestinterface, hasFiles,		asf_http_requestinterface_hasname_arginfo)

    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(http_requestinterface) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERFACE(asf_http_requestinterface, asf_Http_RequestInterface, Asf\\Http\\RequestInterface);

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
