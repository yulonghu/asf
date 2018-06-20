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

#ifndef ASF_HTTP_REP_H
#define ASF_HTTP_REP_H

#define ASF_HTTP_REP_PRONAME_VALUE		"_v"
#define ASF_HTTP_REP_PRONAME_HEADER		"_h"

#define ASF_HTTP_REP_CTYPE_NONE			0
#define ASF_HTTP_REP_CTYPE_JSON_ERRMSG  1
#define ASF_HTTP_REP_CTYPE_SERIALIZE	2
#define ASF_HTTP_REP_CTYPE_VAR_DUMP		3
#define ASF_HTTP_REP_CTYPE_JSON_DATA    5

#define ASF_HTTP_REP_JSON_FORMAT_TXT(t1, code, t2, body, flag) { \
    if (EXPECTED(flag)) { \
        php_printf("%s"t1"%s"t2"%s", "{\"errno\":", code ,",\"data\":\"", body ,"\"}"); \
    } else { \
        php_printf("%s"t1"%s"t2"%s", "{\"errno\":", code ,",\"errmsg\":\"\",\"data\":\"", body ,"\"}"); \
    } \
}

extern zend_class_entry *asf_http_response_ce;

void asf_http_rep_instance(asf_http_rep_t *this_ptr);
void asf_http_rep_display_success(asf_http_rep_t *response_ptr);
void asf_http_rep_display_error(size_t code, zval *errmsg);

ASF_INIT_CLASS(http_response);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
