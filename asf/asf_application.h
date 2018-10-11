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

#ifndef ASF_APPLICATION_H
#define ASF_APPLICATION_H

/* app.config */
#define ASF_APP_PRONAME_CONFIG		"conf"
/* app.dispatcher */
#define ASF_APP_PRONAME_DISPATCHER	"disp"
/* app.instance */
#define ASF_APP_PRONAME_INSTANCE	"_ins"
/* app.env */
#define ASF_APP_PRONAME_ENV			"_env"
/* app.mode */
#define ASF_APP_PRONAME_MODE        "_m"
/* app.run */
#define ASF_APP_PROPNAME_RUN        "_run"
/* app.errfname */
#define ASF_APP_PRONAME_ERR_FNAME   "_ef"
/* app.errhandler */
#define ASF_APP_PRONAME_ERR_HANDLER "_eh"

#define ASF_APPLICATION_CONS_BOOT(asfg_name, err_name, find_name_uf, find_name_lc, zret) \
    zend_string *path = NULL; \
    zend_class_entry *ce; \
    if (UNEXPECTED(ZVAL_IS_NULL(zend_read_static_property(asf_application_ce, ZEND_STRL(ASF_APP_PRONAME_INSTANCE), 1)))) { \
        asf_trigger_error(ASF_ERR_STARTUP_FAILED, "Initialized application failed"); \
        return; \
    } \
    if (!(ce = zend_hash_str_find_ptr(EG(class_table), ZEND_STRL(find_name_lc)))) { \
        char *p = ASF_G(lowcase_path) ? find_name_lc : find_name_uf; \
        zend_string *path = ASF_G(asfg_name) ? zend_string_copy(ASF_G(asfg_name)) : \
        strpprintf(0, "%s%c%s.php", ZSTR_VAL(ASF_G(root_path)), DEFAULT_SLASH, p); \
        if (UNEXPECTED(!asf_loader_import(path, NULL))) { \
            asf_trigger_error(ASF_ERR_## err_name ##_FAILED, "No such file %s", ZSTR_VAL(path)); \
            zend_string_release(path); \
            RETURN_FALSE; \
        } \
        if (UNEXPECTED((ce = zend_hash_str_find_ptr(EG(class_table), ZEND_STRL(find_name_lc))) == NULL)) { \
            zend_string_release(path); \
            asf_trigger_error(ASF_ERR_## err_name ##_FAILED, "Class '%s' not found in %s", p, ZSTR_VAL(path)); \
            RETURN_FALSE; \
        } \
        zend_string_release(path); \
    } \
    if (zret) { \
        RETURN_ZVAL(getThis(), 1, 0); \
    } \

extern zend_class_entry *asf_application_ce;

ASF_INIT_CLASS(application);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
