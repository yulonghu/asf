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
#include "Zend/zend_interfaces.h" /* zend_call_method* */

#include "php_asf.h"
#include "kernel/asf_namespace.h"
#include "asf_exception.h"

#include "http/asf_http_cookieinterface.h"
#include "http/asf_http_cookie.h"

zend_class_entry *asf_http_cookie_ce;

/* {{{ proto object Asf_Http_Cookie::__construct(void)
*/
PHP_METHOD(asf_http_cookie, __construct)
{
}
/* }}} */

/* {{{ proto bool Asf_Http_Cookie::init(array $configs)
*/
PHP_METHOD(asf_http_cookie, init)
{
}
/* }}} */

/* {{{ proto bool Asf_Http_Cookie::prefix([string $prefix = ''])
*/
PHP_METHOD(asf_http_cookie, prefix)
{
}
/* }}} */

/* {{{ proto bool Asf_Http_Cookie::set(string $name [, string $value = ''])
*/
PHP_METHOD(asf_http_cookie, set)
{
}
/* }}} */

/* {{{ proto bool Asf_Http_Cookie::forever(string $name, string $value)
*/
PHP_METHOD(asf_http_cookie, forever)
{
}
/* }}} */

/* {{{ proto bool Asf_Http_Cookie::has(string $name)
*/
PHP_METHOD(asf_http_cookie, has)
{
}
/* }}} */

/* {{{ proto mixed Asf_Http_Cookie::get(string $name)
*/
PHP_METHOD(asf_http_cookie, get)
{
}
/* }}} */

/* {{{ proto bool Asf_Http_Cookie::del(string $name)
*/
PHP_METHOD(asf_http_cookie, del)
{
}
/* }}} */

/* {{{ proto bool Asf_Http_Cookie::clear()
*/
PHP_METHOD(asf_http_cookie, clear)
{
}
/* }}} */

/* {{{ asf_http_cookie_methods[]
*/
zend_function_entry asf_http_cookie_methods[] = {
    PHP_ME(asf_http_cookie, __construct,    asf_http_cookieinterface_init_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(asf_http_cookie, init,           asf_http_cookieinterface_init_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_cookie, prefix,         asf_http_cookieinterface_prefix_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_cookie, set,            asf_http_cookieinterface_set_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_cookie, forever,        asf_http_cookieinterface_forever_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_cookie, has,            asf_http_cookieinterface_has_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_cookie, get,            asf_http_cookieinterface_get_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_cookie, del,            asf_http_cookieinterface_has_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_cookie, clear,          NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(http_cookie) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_http_cookie, Asf_Http_Cookie, Asf\\HTTP\\Cookie, ZEND_ACC_FINAL);

    zend_declare_property_null(asf_http_cookie_ce, ZEND_STRL(ASF_COOKIE_PRONAME_CONFIG), ZEND_ACC_PROTECTED);

    ASF_INIT(cookieinterface);

    zend_class_implements(asf_http_cookie_ce, 1, asf_http_cookieinterface_ce);

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
