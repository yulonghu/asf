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
#include "zend_smart_str.h" /* for smart_str */
#include "Zend/zend_interfaces.h"
#include "main/php_main.h" /* php_stream_* */

#include "php_asf.h"
#include "asf_namespace.h"
#include "asf_loader.h"
#include "asf_exception.h"
#include "http/asf_http_request.h" /* default module name */
#include "asf_func.h"

zend_class_entry *asf_loader_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_loader_get_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, class_name)
    ZEND_ARG_INFO(0, module_name)
    ZEND_ARG_INFO(0, nc_cache)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_loader_clean_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, class_name)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_loader_autoloader_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, class_name)
    ZEND_ARG_INFO(0, module_name)
ZEND_END_ARG_INFO()
/* }}} */

asf_loader_t *asf_loader_instance(zval *this_ptr, zend_string *library_path) /* {{{ */
{
    zval *instance = NULL;
    zval ret;

    instance = zend_read_static_property(asf_loader_ce, ZEND_STRL(ASF_LOADER_PROPERTY_NAME_INSTANCE), 1);
    if (Z_TYPE_P(instance) == IS_OBJECT && 
            instanceof_function(Z_OBJCE_P(instance), asf_loader_ce)) {
        return instance;
    }

    if (Z_ISUNDEF_P(this_ptr)) {
        object_init_ex(this_ptr, asf_loader_ce);
    }

    if (library_path) {
        zend_update_property_str(asf_loader_ce, this_ptr, ZEND_STRL(ASF_LOADER_LIBRARY_DIRECTORY_NAME), library_path);
    }

    ASF_FUNC_CALL_PHP_FUNC(this_ptr, "spl_autoload_register", ASF_AUTOLOAD_FUNC_NAME, ASF_AUTOLOAD_FUNC_NAME_LEN, &ret);

    if (UNEXPECTED(Z_TYPE(ret) == IS_FALSE)) {
        asf_trigger_error(ASF_ERR_LOADER_FAILED, "Failed to register autoload function");
        return NULL;
    }

    zend_update_static_property(asf_loader_ce, ZEND_STRL(ASF_LOADER_PROPERTY_NAME_INSTANCE), this_ptr);

    return this_ptr;
}
/* }}} */

_Bool asf_internal_autoload(char *file_name, size_t name_len, char **root_path) /* {{{ */
{
    _Bool status = 0;
    char *q = NULL, *p = NULL;
    size_t root_path_len = 0;
    smart_str buf = {0};

    /* library loading */
    if (NULL == *root_path) {
        zend_string *library_path;
        asf_loader_t *loader, rv = {{0}};

        loader = asf_loader_instance(&rv, NULL);

        if (loader == NULL) {
            asf_trigger_error(ASF_ERR_STARTUP_FAILED, "Load loader failed");
            return 0;
        }

        zval *library_dir = zend_read_property(asf_loader_ce, loader, ZEND_STRL(ASF_LOADER_LIBRARY_DIRECTORY_NAME), 1, NULL);
        library_path = Z_STR_P(library_dir);

        smart_str_appendl(&buf, ZSTR_VAL(library_path), ZSTR_LEN(library_path));

        zval_ptr_dtor(&rv);
    } else {
        /* service, logic, dao, constants */
        smart_str_appendl(&buf, *root_path, strlen(*root_path));
    }

    root_path_len = ZSTR_LEN(buf.s);

    /* aussume all the path is not end in slash */
    smart_str_appendc(&buf, DEFAULT_SLASH);

    p = file_name;
    q = p;

    while (1) {
        do {
            q++;
        } while (*q != '_' && *q != '\0');
        if (*q != '\0') {
            smart_str_appendl(&buf, p, q - p);
            smart_str_appendc(&buf, DEFAULT_SLASH);
            p = q + 1;
        } else {
            break;
        }
    }

    smart_str_appendl(&buf, p, strlen(p));
    smart_str_appendl(&buf, ".php", 4);
    smart_str_0(&buf);

    /* all path changeto lowercase */
    if (ASF_G(lowcase_path)) {
        zend_str_tolower(ZSTR_VAL(buf.s) + root_path_len, ZSTR_LEN(buf.s) - root_path_len);
    }

    status = asf_loader_import(buf.s, NULL); 
    smart_str_free(&buf);

    return status;
}
/* }}} */

static size_t asf_loader_is_user_autoload(const char *class_name, size_t class_name_len, const char *path_name, size_t path_name_len) /* {{{ */
{
    if (UNEXPECTED(class_name_len <= path_name_len)) {
        return 0;
    }

    /* The class name is indexlogic and IndexLogic */
    if (strncasecmp(class_name + class_name_len - path_name_len, path_name, path_name_len) == 0) {
        return 1;
    }

    return 0;
}
/* }}} */

/* {{{ proto bool Asf_Loader::autoload(string $class_name [, string $module_name = ''])
*/
PHP_METHOD(asf_loader, autoload)
{
    char *class_name = NULL, *origin_classname = NULL, *root_path_rel = NULL, *root_path_route = NULL, *file_name = NULL;
    size_t file_name_len = 0, class_name_len = 0;
    _Bool ret = 1, is_path_efree = 0;
    zend_string *module_name = NULL;

    // class_name = indexlogic or IndexLogic
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|S", &class_name, &class_name_len, &module_name) == FAILURE) {
        return;
    }

    /* specified module name */
    if (UNEXPECTED(module_name && !asf_func_isempty(ZSTR_VAL(module_name)))) {
        spprintf(&root_path_route, 0,
                "%s%c%s", ZSTR_VAL(ASF_G(root_path)), DEFAULT_SLASH, ZSTR_VAL(module_name));
        is_path_efree = 1;
    } else {
    /* usually */
        if (ASF_G(root_path_route)) {
            root_path_route = ZSTR_VAL(ASF_G(root_path_route));
        } else if (ASF_G(root_path)) {
            is_path_efree = 1;
            spprintf(&root_path_route, 0,
                    "%s%c%s", ZSTR_VAL(ASF_G(root_path)), DEFAULT_SLASH, ASF_API_NAME);
        }
    }

    if (UNEXPECTED(!root_path_route)) {
        asf_trigger_error(ASF_ERR_LOADER_FAILED, "Root path '%s' not found", root_path_route);
        return;
    }

    if (UNEXPECTED(strncmp(class_name, "Asf", 3) == 0)) {
        asf_trigger_error(ASF_ERR_LOADER_FAILED, "You cant not use 'Asf_' as class name prefix");
        return;
    }

    origin_classname = class_name;

    char *pos = NULL, *dup_lcname = NULL;
    if ((pos = strchr(class_name, '\\')) != NULL) {
        dup_lcname = estrndup(class_name, class_name_len);
        pos = dup_lcname + (pos - class_name);
        *pos = '_';
        while (*(++pos) != '\0') {
            if (*pos == '\\') {
                *pos = '_';
            }
        }
        class_name = dup_lcname;
    }

    /* {{{ */
    do {
        if (asf_loader_is_user_autoload(class_name, class_name_len, ASF_LOADER_LOGIC, ASF_LOADER_LOGIC_LEN)) {
            /* root_path_rel = /project/modules/xxx/logics/ */
            spprintf(&root_path_rel, 0, "%s%c%s", root_path_route, DEFAULT_SLASH, ASF_LOADER_LOGIC_DIRECTORY_NAME);

            if (!ASF_G(file_suffix)) {
                /* Index.php */
                file_name_len = class_name_len - ASF_LOADER_LOGIC_LEN;
            } else {
                /* IndexLogic.php */
                file_name_len = class_name_len;
            }
            file_name = estrndup(class_name, file_name_len);
            break;
        }

        if (asf_loader_is_user_autoload(class_name, class_name_len, ASF_LOADER_DAO, ASF_LOADER_DAO_LEN)) {
            /* root_path_rel = /project/modules/xxx/daos/ */
            spprintf(&root_path_rel, 0, "%s%c%s", root_path_route, DEFAULT_SLASH, ASF_LOADER_DAO_DIRECTORY_NAME);

            if (!ASF_G(file_suffix)) {
                file_name_len = class_name_len - ASF_LOADER_DAO_LEN;
            } else {
                file_name_len = class_name_len;
            }
            file_name = estrndup(class_name, file_name_len);

            break;
        }

        if (asf_loader_is_user_autoload(class_name, class_name_len, ASF_LOADER_SERVER, ASF_LOADER_SERVER_LEN)) {
            /* root_path_rel= /project/modules/xxx/services/ */
            spprintf(&root_path_rel, 0, "%s%c%s", root_path_route, DEFAULT_SLASH, ASF_LOADER_SERVICE_DIRECTORY_NAME);

            if (!ASF_G(file_suffix)) {
                file_name_len = class_name_len - ASF_LOADER_SERVER_LEN;
            } else {
                file_name_len = class_name_len;
            }
            file_name = estrndup(class_name, file_name_len);

            break;
        }

        file_name_len = class_name_len;
        file_name     = class_name;

    } while (0);
    /* }}} */

    char *lower_case_name = zend_str_tolower_dup(origin_classname, class_name_len);

    if (asf_internal_autoload(file_name, file_name_len, &root_path_rel) &&
            zend_hash_str_exists(EG(class_table), lower_case_name, class_name_len)) {
        efree(lower_case_name);
        goto output;
    }

    ret = 0;
    efree(lower_case_name);

output:
    if (root_path_rel) {
        efree(root_path_rel);
    }

    if (dup_lcname) {
        efree(dup_lcname);
    }

    if (file_name != class_name) {
        efree(file_name);
    }

    if (is_path_efree) {
        efree(root_path_route);
    }

    RETURN_BOOL(ret);
}
/* }}} */

_Bool asf_loader_import(zend_string *path, zval *return_value_ptr) /* {{{ */
{
    zend_file_handle file_handle;
    zend_op_array 	*op_array = NULL;
    _Bool ret = 1;

    if (UNEXPECTED(php_stream_open_for_zend_ex(ZSTR_VAL(path), &file_handle, USE_PATH|STREAM_OPEN_FOR_INCLUDE) != SUCCESS)) {
        return 0;
    }

    if (!file_handle.opened_path) {
        file_handle.opened_path = zend_string_init(ZSTR_VAL(path), ZSTR_LEN(path), 0);
    }

    zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path);
    op_array = zend_compile_file(&file_handle, ZEND_INCLUDE);
    zend_destroy_file_handle(&file_handle);

    if (op_array) {
        zval result;
        ZVAL_UNDEF(&result);

        zend_try {
            zend_execute(op_array, &result);
            if (return_value_ptr) {
                ZVAL_DUP(return_value_ptr, &result);
            }
            zval_ptr_dtor(&result);
        } zend_catch {
            ret = 0;
        } zend_end_try();

        destroy_op_array(op_array);
        efree(op_array);
    }

    return ret;
}
/* }}} */

/* {{{ proto object Asf_Loader::get(string $class_name [, string $module_name = '', [, class_table_cache = 1]])
*/
PHP_METHOD(asf_loader, get)
{
    zend_string *class_name = NULL, *module_name = NULL;
    zend_long ct_cache = 1;

    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_STR(class_name)
        Z_PARAM_OPTIONAL
        Z_PARAM_STR(module_name)
        Z_PARAM_LONG(ct_cache)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (UNEXPECTED(asf_func_isempty(ZSTR_VAL(class_name)))) {
        asf_trigger_error(ASF_ERR_LOADER_CLASSNAME, "Parameter 'class_name' must be a string");
        return;
    }

    zend_class_entry *ce = NULL;
    char *lc_class_name = zend_str_tolower_dup(ZSTR_VAL(class_name), ZSTR_LEN(class_name));

    if (!ct_cache || (ce = zend_hash_str_find_ptr(EG(class_table), lc_class_name, ZSTR_LEN(class_name))) == NULL) {
        /* If the class 'xx' not found */
        zval *instance = zend_read_static_property(asf_loader_ce, ZEND_STRL(ASF_LOADER_PROPERTY_NAME_INSTANCE), 1);
        if (Z_TYPE_P(instance) != IS_OBJECT) {
            efree(lc_class_name);
            asf_trigger_error(ASF_ERR_LOADER_FAILED, "Loader used failed, Please the Application First init");
            return;
        }

        zval zclass_name, zmodule_name, ret;

        ZVAL_STR_COPY(&zclass_name, class_name);
        ZVAL_NULL(&zmodule_name);

        if (UNEXPECTED(module_name && !asf_func_isempty(ZSTR_VAL(module_name)))) {
            ZVAL_STR_COPY(&zmodule_name, module_name);
        }

        zend_call_method_with_2_params(instance, Z_OBJCE_P(instance), NULL, "autoload", &ret, &zclass_name, &zmodule_name);

        zval_ptr_dtor(&zclass_name);
        zval_ptr_dtor(&zmodule_name);

        if (Z_TYPE(ret) == IS_TRUE) {
            ce = zend_hash_str_find_ptr(EG(class_table), lc_class_name, ZSTR_LEN(class_name));
        }
    }

    efree(lc_class_name);

    if (!ce) {
        asf_trigger_error(ASF_ERR_LOADER_FAILED, "Class '%s' not found", ZSTR_VAL(class_name));
        return;
    }

    if (UNEXPECTED(object_init_ex(return_value, ce) != SUCCESS)) {
        RETURN_FALSE;
    }

    if (ce->constructor && ce->constructor->common.fn_flags & ZEND_ACC_PUBLIC) {
        if (EXPECTED(ce->constructor->common.num_args == 0)) {
            zend_call_method_with_0_params(return_value, ce, NULL, ZEND_CONSTRUCTOR_FUNC_NAME, NULL);
        }
    }
}
/* }}} */

/* {{{ proto bool Asf_Loader::clean(string $class_name)
*/
PHP_METHOD(asf_loader, clean)
{
    zend_string *class_name = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &class_name) == FAILURE) {
        return;
    }

    if (UNEXPECTED(asf_func_isempty(ZSTR_VAL(class_name)))) {
        asf_trigger_error(ASF_ERR_LOADER_CLASSNAME, "Parameter 'class_name' must be a string");
        return;
    }

    zend_string *lc_class_name = zend_string_tolower(class_name);

    if (zend_hash_exists(EG(class_table), lc_class_name) &&
            zend_hash_del(EG(class_table), lc_class_name) == SUCCESS) {
        zend_string_release(lc_class_name);
        RETURN_TRUE;
    }

    zend_string_release(lc_class_name);
    RETURN_FALSE;
}
/* }}} */

PHP_METHOD(asf_loader, __sleep) /* {{{ */
{
}
/* }}} */

PHP_METHOD(asf_loader, __wakeup) /* {{{ */
{
}
/* }}} */

PHP_METHOD(asf_loader, __clone) /* {{{ */
{
}
/* }}} */

/* {{{ asf_loader_methods[]
*/
zend_function_entry asf_loader_methods[] = {
    PHP_ME(asf_loader, get,      asf_loader_get_arginfo,        ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_loader, clean,    asf_loader_clean_arginfo,      ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_loader, autoload, asf_loader_autoloader_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(loader) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_loader, Asf_Loader, Asf\\Loader, ZEND_ACC_FINAL);

    zend_declare_property_null(asf_loader_ce, ZEND_STRL(ASF_LOADER_PROPERTY_NAME_INSTANCE), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
    zend_declare_property_null(asf_loader_ce, ZEND_STRL(ASF_LOADER_LIBRARY_DIRECTORY_NAME), ZEND_ACC_PROTECTED);

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
