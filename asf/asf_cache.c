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
#include "Zend/zend_interfaces.h" /* for zend_call_method_with_* */

#include "php_asf.h"
#include "kernel/asf_namespace.h"
#include "asf_cache.h"
#include "asf_exception.h"

zend_class_entry *asf_cache_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_cache_init_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, options, 0)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, force)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_setconf_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, configs, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_store_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_cache_call_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, function_name)
    ZEND_ARG_ARRAY_INFO(0, arguments, 1)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ proto object Asf_Cache::init(array $options [, string $name = 'default' [, $force = 1]])
*/
PHP_METHOD(asf_cache, init)
{
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto array Asf_Cache::getConfig(void)
*/
PHP_METHOD(asf_cache, getConfig)
{
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool Asf_Cache::setConfig(array $config)
*/
PHP_METHOD(asf_cache, setConfig)
{
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool Asf_Cache::getLinks(void)
*/
PHP_METHOD(asf_cache, getLinks)
{
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto object Asf_Cache::store(string $name)
*/
PHP_METHOD(asf_cache, store)
{
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto object Asf_Cache::close(void)
*/
PHP_METHOD(asf_cache, close)
{
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto object Asf_Cache::closeAll(void)
*/
PHP_METHOD(asf_cache, closeAll)
{
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto mixed Asf_Cache::__callStatic(string $function_name, array $args)
*/
PHP_METHOD(asf_cache, __callStatic)
{
    RETURN_FALSE;
}
/* }}} */

/* {{{ asf_cache_methods[]
*/
zend_function_entry asf_cache_methods[] = {
    PHP_ME(asf_cache, init,         asf_cache_init_arginfo,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_cache, getConfig,    NULL,                       ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_cache, setConfig,    asf_cache_setconf_arginfo,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_cache, getLinks,     NULL,                       ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_cache, store,        asf_cache_store_arginfo,    ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_cache, close,        NULL,                       ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_cache, closeAll,     NULL,                       ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_cache, __callStatic, asf_cache_call_arginfo,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(cache) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_cache, Asf_Cache, Asf\\Cache, ZEND_ACC_FINAL);

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
