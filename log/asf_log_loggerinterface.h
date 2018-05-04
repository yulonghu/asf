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

#ifndef ASF_LOG_LOGGERINTERFACE_H
#define ASF_LOG_LOGGERINTERFACE_H

/* {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(asf_log_loggerinterface_common_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, message)
    ZEND_ARG_ARRAY_INFO(0, context, 1)
    ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_log_loggerinterface_log_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, level)
    ZEND_ARG_INFO(0, message)
    ZEND_ARG_ARRAY_INFO(0, context, 1)
ZEND_END_ARG_INFO()
/* }}}*/

extern zend_class_entry *asf_log_loggerinterface_ce;

ASF_INIT_CLASS(log_loggerinterface);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
