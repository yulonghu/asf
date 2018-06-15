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

#ifndef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/php_var.h"
#include "Zend/zend_interfaces.h" /* for zend_call_method_with_* */
#include "ext/standard/php_string.h"
#include "Zend/zend_exceptions.h"

#include "php_asf.h"
#include "kernel/asf_namespace.h"
#include "asf_application.h"
#include "asf_dispatcher.h"
#include "http/asf_http_request.h"
#include "http/asf_http_response.h"

#include "asf_router.h"
#include "asf_loader.h"
#include "asf_exception.h"
#include "asf_service.h"
#include "kernel/asf_func.h"

#include "routes/asf_route_pathinfo.h"
#include "routes/asf_route_query.h"
#include "routes/asf_route_dot.h"

zend_class_entry * asf_dispatcher_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_dispatcher_name_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_dispatcher_exception_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, start)
ZEND_END_ARG_INFO()
/* }}}*/

static _Bool asf_dispatcher_route_match(zval *dispatcher, asf_http_req_t *request) /* {{{ */
{
    asf_router_t router = {{0}}; 
    _Bool ret = 0;

    (void)asf_router_instance(&router);

    switch(ZSTR_LEN(Z_OBJCE(router)->name)) {
        case ASF_DISPATCHER_ROUTER_A:
            ret = asf_route_pathinfo_run(&router, request);
            break;
        case ASF_DISPATCHER_ROUTER_B:
            ret = asf_route_query_run(&router, request);
            break;
        case ASF_DISPATCHER_ROUTER_C:
            ret = asf_route_dot_run(&router, request);
            break;
    }

    if (UNEXPECTED(!ZVAL_IS_NULL(&router))) {
        zend_update_property(Z_OBJCE_P(dispatcher), dispatcher, ZEND_STRL(ASF_DISP_PRONAME_ROUTER), &router);
        zval_ptr_dtor(&router);
    }

    return ret;
}
/* }}} */

void asf_dispatcher_instance(asf_disp_t *this_ptr, asf_http_req_t *request) /* {{{ */
{
    object_init_ex(this_ptr, asf_dispatcher_ce);

    asf_dispatcher_route_match(this_ptr, request);

    zend_update_property(asf_dispatcher_ce, this_ptr, ZEND_STRL(ASF_DISP_PRONAME_REQ), request);
    zend_update_static_property(asf_dispatcher_ce, ZEND_STRL(ASF_DISP_PRONAME_INS), this_ptr);
}
/* }}} */

/* {{{ _get_recv_opcode */
static zval *_get_recv_op_zval(zend_function *fptr, uint32_t offset)
{
    zend_op *op = ((zend_op_array*)fptr)->opcodes;
    zend_op *end = op + ((zend_op_array*)fptr)->last;

    ++offset;
    while (op < end) {
        if ((op->opcode == ZEND_RECV || op->opcode == ZEND_RECV_INIT
                    || op->opcode == ZEND_RECV_VARIADIC) && op->op1.num == offset)
        {
            break; 
        }
        ++op;
    }

    if (!op || op->opcode != ZEND_RECV_INIT || op->op2_type == IS_UNUSED) {
        return NULL;
    }

    return RT_CONSTANT(&fptr->op_array, op->op2);
}
/* }}} */

static void asf_dispatcher_gpc_parse_parameters(asf_http_req_t *request, zend_function *fptr, zval **params) /* {{{ */
{
    zval            *arg = NULL, *g_args = NULL, *p_args = NULL, *c_args = NULL;
    zend_string     *sarg = NULL;
    uint            current = 0;
    zend_arg_info   *arg_info;
    HashTable       *params_ht = NULL;
    char *cinfo = NULL, *cinfo2 = NULL;
    int cinfo2_len = 0;
    _Bool is_gpc = 0;

    g_args = zend_read_property(Z_OBJCE_P(request), request, ZEND_STRL(ASF_HTTP_REQ_PRONAME_PARAMS), 1, NULL);

    params_ht = Z_ARRVAL_P(g_args);
    arg_info  = fptr->common.arg_info;
    *params   = safe_emalloc(sizeof(zval), fptr->common.num_args, 0);

    for (current = 0; current < fptr->common.num_args; current++, arg_info++) {

        cinfo = ZSTR_VAL(arg_info->name); cinfo2 = cinfo + 2;
        cinfo2_len = ZSTR_LEN(arg_info->name) - 2;
        arg = NULL;

        /* Integer to meet the search criteria */
        /* Exclude invalid $_g, $_p, $_c */
        is_gpc = 0;

        do {
            /* Search key in $_POST */
            if (php_memnstr(cinfo, (char *)"p_", 2, cinfo2)) {
                arg = asf_http_req_pg_find_len(TRACK_VARS_POST, cinfo2, cinfo2_len);
                is_gpc = 1;
                break;
            }

            /* Search key in $_GET */
            if (php_memnstr(cinfo, (char *)"g_", 2, cinfo2)) {
                arg = zend_hash_str_find(params_ht, cinfo2, cinfo2_len);
                is_gpc = 1;
                break;
            }

            /* Search key in $_COOKIE */
            if (php_memnstr(cinfo, (char *)"c_", 2, cinfo2)) {
                arg = asf_http_req_pg_find_len(TRACK_VARS_COOKIE, cinfo2, cinfo2_len);
                is_gpc = 1;
                break;
            }

            /* If the paramters prefix not found, Default is $_GET, exclude gpc the way */
            if (!is_gpc) {
                arg = zend_hash_find(params_ht, arg_info->name);
            }
        } while(0);

        /* if not find key, search default value  */
        if (!arg) {
            arg = _get_recv_op_zval(fptr, current);
        }

        if (arg) {
            /* Filter the space used by php_trim */
            if (Z_TYPE_P(arg) == IS_STRING) {
                sarg = php_trim(Z_STR_P(arg), NULL, 0, 3);
                /* Fix sarg value is empty string, and cancel length judgment */
                if (Z_STRLEN_P(arg) != ZSTR_LEN(sarg)) {
                    ZVAL_STR_COPY(arg, sarg);
                }
            }

            ZVAL_COPY_VALUE(&((*params)[current]), arg);
        } else {
            ZVAL_NULL(&((*params)[current]));
        }

        if (sarg) {
            zend_string_release(sarg);
            sarg = NULL;
        }
    }
}
/* }}} */

static inline zend_class_entry *asf_dispatcher_load_service(zend_string *service) /* {{{ */
{
    char *path = NULL;
    size_t path_len = 0;

    /* /project/modules/xxxx/services */
    path_len = spprintf(&path, 0,
            "%s%s", ZSTR_VAL(ASF_G(root_path_route)), ASF_LOADER_SERVICE_DIRECTORY_NAME);

    if (UNEXPECTED(!path_len)) {
        return NULL;
    }

    zend_class_entry *ce = NULL;
    char *filename = NULL;
    uint ret = 0;

    zend_string *lc_class = strpprintf(0, "%s%s", ZSTR_VAL(service), "service"); 

    if (!ASF_G(file_suffix)) {
        /* index.php */
        filename = estrndup(ZSTR_VAL(service), ZSTR_LEN(service));
    } else {
        /* indexservice.php */
        filename = estrndup(ZSTR_VAL(lc_class), ZSTR_LEN(lc_class));
    }

    /* Indexservice.php OR Index.php */
    if (!ASF_G(lowcase_path)) {
        *filename = toupper(*filename);
    }

    if ((ce = zend_hash_find_ptr(EG(class_table), lc_class)) == NULL) {
        ret = asf_internal_autoload(filename, strlen(filename), &path);
        if (UNEXPECTED(!ret)) {
            asf_trigger_error(ASF_ERR_AUTOLOAD_FAILED, "No such file %s/%s.php", path, filename);
            goto free_res;
        } else if ((ce = zend_hash_find_ptr(EG(class_table), lc_class)) == NULL) {
            asf_trigger_error(ASF_ERR_AUTOLOAD_FAILED, "Class '%s' not found in %s/%s.php", ZSTR_VAL(lc_class), path, filename);
            goto free_res;
        }
    }

free_res:
    zend_string_release(lc_class);
    efree(path);
    efree(filename);

    return ce;
}
/* }}} */

_Bool asf_dispatcher_is_right_module(char *module_name, size_t module_name_len) /* {{{ */
{
    zval *pzval = NULL;

    if (0 == strncasecmp(module_name, ASF_API_NAME, ASF_API_LEN)) {
        return 1;
    }

    if (UNEXPECTED(Z_TYPE(ASF_G(modules)) != IS_ARRAY)) {
        return 0;
    }

    ZEND_HASH_FOREACH_VAL(Z_ARRVAL(ASF_G(modules)), pzval) {
        if (UNEXPECTED(Z_TYPE_P(pzval) != IS_STRING)) {
            continue;
        }
        if (strncasecmp(module_name, Z_STRVAL_P(pzval), module_name_len) == 0) {
            return 1;
        }
    } ZEND_HASH_FOREACH_END();

    return 0;
}
/* }}} */

static inline void asf_dispatcher_load_route_match(zval *routes, zval **module, zval **service, zval **action) /*{{{*/
{
    zend_string *key = NULL;
    zval *entry = NULL, *ztmp = NULL;
    HashTable *ht = NULL;
    const char *ctmp = NULL;
    uint ctmp_len = 0;

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(routes), key, entry) {
        if (UNEXPECTED(!key)) {
            continue;
        }

        if (strncmp(Z_STRVAL_P(*service), ZSTR_VAL(key), Z_STRLEN_P(*service))) {
            continue;
        }

        if (ZSTR_LEN(key) != Z_STRLEN_P(*service)) {
            ctmp = ZSTR_VAL(key) + Z_STRLEN_P(*service) + 1;
            ctmp_len = ZSTR_LEN(key) - Z_STRLEN_P(*service) - 1;

            /* Exclude a match '*' */
            if (!ctmp_len || (ctmp[0] != '*' && strncmp(Z_STRVAL_P(*action), ctmp, ctmp_len))) {
                continue;               
            }
        }

        ht = Z_ARRVAL_P(entry);

        if ((ztmp = zend_hash_str_find(ht, "module", 6))) {
            *module = ztmp;
        }
        if ((ztmp = zend_hash_str_find(ht, "service", 7))) {
            *service = ztmp;
        }
        if ((ztmp = zend_hash_str_find(ht, "action", 6))) {
            *action = ztmp;
        }

        break;
    } ZEND_HASH_FOREACH_END();
}
/*}}}*/

static inline _Bool asf_dispatcher_run(asf_disp_t *dispatcher, asf_http_req_t *request, asf_http_rep_t *response_ptr) /* {{{ */
{
    zend_class_entry *ce = NULL;

    zval *module = zend_read_property(Z_OBJCE_P(request),
            request, ZEND_STRL(ASF_HTTP_REQ_PRONAME_MODULE), 1, NULL);
    zval *service = zend_read_property(Z_OBJCE_P(request),
            request, ZEND_STRL(ASF_HTTP_REQ_PRONAME_SERVICE), 1, NULL);
    zval *action = zend_read_property(Z_OBJCE_P(request),
            request, ZEND_STRL(ASF_HTTP_REQ_PRONAME_ACTION), 1, NULL);

    /* Loading route match */
    zval *routes = zend_read_property(Z_OBJCE_P(dispatcher), dispatcher,
            ZEND_STRL(ASF_DISP_PRONAME_ROUTES), 1, NULL);
    if (routes && Z_TYPE_P(routes) == IS_ARRAY) {
        (void)asf_dispatcher_load_route_match(routes, &module, &service, &action);
    }

    /* Set root_path_route */
    (void)asf_func_set_cur_module(Z_STRVAL_P(module));

    /* Loading a service */
    if (!(ce = asf_dispatcher_load_service(Z_STR_P(service)))) {
        return 0;		
    }

    zval ret, zservice;
    zend_string *func_name = NULL;
    zend_function *fptr;

    object_init_ex(&zservice, ce);

    /* first service init */
    (void)asf_http_rep_instance(response_ptr);
    (void)asf_service_construct(ce, &zservice, request, response_ptr);

    /* If find function __constructor */
    if (ce->constructor && ce->constructor->common.fn_flags & ZEND_ACC_PUBLIC) {
        zend_call_method_with_0_params(&zservice, ce, NULL, ZEND_CONSTRUCTOR_FUNC_NAME, NULL);
    }

    // xxxAction or xxx
    func_name = ASF_G(action_suffix) ? strpprintf(0, "%s%s", Z_STRVAL_P(action), "action")
        : zend_string_init(Z_STRVAL_P(action), Z_STRLEN_P(action), 0);

    if ((fptr = zend_hash_find_ptr(&((ce)->function_table), func_name)) != NULL) {
        zval *call_args = NULL;

        if (fptr->common.num_args) {
            zval method_name;
            ZVAL_STR(&method_name, func_name);

            (void)asf_dispatcher_gpc_parse_parameters(request, fptr, &call_args);
            call_user_function_ex(&(ce)->function_table,
                    &zservice, &method_name, &ret, fptr->common.num_args, call_args, 1, NULL);
            efree(call_args);
        } else {
            zend_call_method(&zservice, ce, NULL, ZSTR_VAL(func_name), ZSTR_LEN(func_name), &ret, 0, NULL, NULL);
        }
    }

    if (EG(exception)) {
        zend_string_release(func_name);
        zval_ptr_dtor(&zservice);
        zval_ptr_dtor(response_ptr);
        return 0;
    }

    if (!fptr) {
        asf_trigger_error(ASF_ERR_NOTFOUND_ACTION,
                "There is no method %s in %s", ZSTR_VAL(func_name), ZSTR_VAL(Z_OBJCE_P(&zservice)->name));
        zend_string_release(func_name);
        zval_ptr_dtor(&zservice);
        zval_ptr_dtor(response_ptr);
        return 0;
    }

    zend_update_property(Z_OBJCE_P(response_ptr), response_ptr, ZEND_STRL(ASF_HTTP_REP_PRONAME_VALUE), &ret);

    zend_string_release(func_name);
    zval_ptr_dtor(&zservice);

    if (!Z_ISUNDEF(ret)) {
        zval_ptr_dtor(&ret);
    }

    return 1;
}
/* }}} */

_Bool asf_dispatcher_ondispatch(asf_disp_t *dispatcher, asf_http_rep_t *response_ptr) /* {{{ */
{
    zval *request = zend_read_property(Z_OBJCE_P(dispatcher), dispatcher,
            ZEND_STRL(ASF_DISP_PRONAME_REQ), 1, NULL);

    if (UNEXPECTED(!zend_read_property(Z_OBJCE_P(dispatcher), dispatcher,
                    ZEND_STRL(ASF_DISP_PRONAME_ROUTER), 1, NULL))) {
        ASF_EG_EXCEPTION(ASF_ERR_ROUTE_FAILED, "Routing match failed");
        return 0;
    }

    if (UNEXPECTED(!(asf_dispatcher_run(dispatcher, request, response_ptr)))) {
        ASF_EG_EXCEPTION(ASF_ERR_DISPATCH_FAILED, "Dispatcher run failed");
        return 0;
    }

    return 1;
}
/* }}} */

/* {{{ proto object Asf_Dispatcher::getInstance(void)
*/
PHP_METHOD(asf_dispatcher, getInstance)
{
    ZVAL_COPY(return_value, zend_read_static_property(asf_dispatcher_ce,
                ZEND_STRL(ASF_DISP_PRONAME_INS), 1));
}
/* }}} */

/* {{{ proto object Asf_Dispatcher::setDefaultModule(string $name)
*/
PHP_METHOD(asf_dispatcher, setDefaultModule)
{
    zend_string *module = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &module) == FAILURE) {
        return;
    }
    
    if (UNEXPECTED(!asf_dispatcher_is_right_module(ZSTR_VAL(module), ZSTR_LEN(module)))) {
        asf_trigger_error(ASF_ERR_NOTFOUND_MODULE, "Module name '%s' invalid", ZSTR_VAL(module));
        return;
    }

    zval *self = getThis();
    zval *request = zend_read_property(Z_OBJCE_P(self), self, ZEND_STRL(ASF_DISP_PRONAME_REQ), 1, NULL);

    zend_string *module_lowercase = zend_string_tolower(module);

    zend_update_property_str(Z_OBJCE_P(request), request, ZEND_STRL(ASF_HTTP_REQ_PRONAME_MODULE), module_lowercase);
    zend_string_release(module_lowercase);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto object Asf_Dispatcher::setDefaultService(string $name)
*/
PHP_METHOD(asf_dispatcher, setDefaultService)
{
    zend_string *service = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &service) == FAILURE) {
        return;
    }

    if (UNEXPECTED(asf_func_isempty(ZSTR_VAL(service)))) {
        asf_trigger_error(ASF_ERR_DIS_SERVICE, "Parameter 'service' must be a string");
        return;
    }

    zval *self = getThis();
    zval *request = zend_read_property(Z_OBJCE_P(self), self, ZEND_STRL(ASF_DISP_PRONAME_REQ), 1, NULL);

    zend_string *service_lowercase = zend_string_tolower(service);

    zend_update_property_str(Z_OBJCE_P(request), request, ZEND_STRL(ASF_HTTP_REQ_PRONAME_SERVICE), service_lowercase);
    zend_string_release(service_lowercase);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto object Asf_Dispatcher::setDefaultAction(string $name)
*/
PHP_METHOD(asf_dispatcher, setDefaultAction)
{
    zend_string *action = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &action) == FAILURE) {
        return;
    }

    if (asf_func_isempty(ZSTR_VAL(action))) {
        asf_trigger_error(ASF_ERR_DIS_ACTION, "Parameter 'action' must be a string");
        return;
    }

    zval *self = getThis();
    zval *request = zend_read_property(Z_OBJCE_P(self), self, ZEND_STRL(ASF_DISP_PRONAME_REQ), 1, NULL);

    zend_string *action_lowercase = zend_string_tolower(action);

    zend_update_property_str(Z_OBJCE_P(request), request, ZEND_STRL(ASF_HTTP_REQ_PRONAME_ACTION), action_lowercase);
    zend_string_release(action_lowercase);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/* {{{ proto object Asf_Dispatcher::throwException(bool $start)
*/
PHP_METHOD(asf_dispatcher, throwException)
{
    zend_bool start = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "b", &start) == FAILURE) {
        return;
    }

    ASF_G(throw_exception) = start;

    RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/* {{{ proto object Asf_Dispatcher::getRouter(void)
*/
PHP_METHOD(asf_dispatcher, getRouter)
{
    ZVAL_COPY(return_value, zend_read_property(asf_dispatcher_ce, getThis(),
                ZEND_STRL(ASF_DISP_PRONAME_ROUTER), 1, NULL));
}
/* }}} */

/* {{{ proto object Asf_Dispatcher::getRequest(void)
*/
PHP_METHOD(asf_dispatcher, getRequest)
{
    ZVAL_COPY(return_value, zend_read_property(asf_dispatcher_ce, getThis(),
                ZEND_STRL(ASF_DISP_PRONAME_REQ), 1, NULL));
}
/* }}} */

/* {{{ asf_dispatcher_methods[]
*/
zend_function_entry asf_dispatcher_methods[] = {
    PHP_ME(asf_dispatcher, getInstance,		  NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_dispatcher, setDefaultModule,  asf_dispatcher_name_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_dispatcher, setDefaultService, asf_dispatcher_name_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_dispatcher, setDefaultAction,  asf_dispatcher_name_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_dispatcher, throwException,    asf_dispatcher_exception_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_dispatcher, getRouter,  NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_dispatcher, getRequest, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(dispatcher) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_dispatcher, Asf_Dispatcher, Asf\\Dispatcher, ZEND_ACC_FINAL);

    zend_declare_property_null(asf_dispatcher_ce, ZEND_STRL(ASF_DISP_PRONAME_INS), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
    zend_declare_property_null(asf_dispatcher_ce, ZEND_STRL(ASF_DISP_PRONAME_REQ),  ZEND_ACC_PROTECTED);
    zend_declare_property_null(asf_dispatcher_ce, ZEND_STRL(ASF_DISP_PRONAME_ROUTER),  ZEND_ACC_PROTECTED);
    zend_declare_property_null(asf_dispatcher_ce, ZEND_STRL(ASF_DISP_PRONAME_ROUTES), ZEND_ACC_PROTECTED);

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
