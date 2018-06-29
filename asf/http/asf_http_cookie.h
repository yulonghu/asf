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

#ifndef ASF_HTTP_COOKIE_H
#define ASF_HTTP_COOKIE_H

#define ASF_COOKIE_PRONAME_CONFIG  "_conf"
#define ASF_COOKIE_PRONAME_PREFIX  "_prefix"

#define ASF_COOKIE_PATH "path"
#define ASF_COOKIE_PATH_LEN 4
#define ASF_COOKIE_DOMAIN "domain"
#define ASF_COOKIE_DOMAIN_LEN 6
#define ASF_COOKIE_EXPIRE "expire"
#define ASF_COOKIE_EXPIRE_LEN 6
#define ASF_COOKIE_SECURE "secure"
#define ASF_COOKIE_SECURE_LEN 6
#define ASF_COOKIE_HTTPONLY "httponly"
#define ASF_COOKIE_HTTPONLY_LEN 8
#define ASF_COOKIE_PREFIX "prefix"
#define ASF_COOKIE_PREFIX_LEN 6

extern zend_class_entry *asf_http_cookie_ce;

ASF_INIT_CLASS(http_cookie);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
