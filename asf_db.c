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
#include "asf_namespace.h"
#include "asf_exception.h"

#include "zend_smart_str.h" /* for smart_str */
#include "ext/standard/php_var.h" /* php_serialize_data_t */
#include "ext/standard/md5.h"
#include "Zend/zend_interfaces.h" /* for zend_call_method_with_* */
#include "Zend/zend_exceptions.h"

#include "asf_db.h"
#include "kernel/asf_func.h"
#include "db/asf_db_adapterinterface.h"
#include "db/asf_db_abstractadapter.h"
#include "db/adapter/asf_db_adapter_mysql.h"
#include "db/adapter/asf_db_adapter_sqlite.h"
#include "db/adapter/asf_db_adapter_pgsql.h"
#include "db/asf_db_abstractquerybuilder.h"
#include "db/querybuilder/asf_db_querybuilder_select.h"
#include "db/querybuilder/asf_db_querybuilder_insert.h"
#include "db/querybuilder/asf_db_querybuilder_update.h"
#include "db/querybuilder/asf_db_querybuilder_delete.h"

zend_class_entry *asf_Db_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_Db_init_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, configs, 1)
    ZEND_ARG_INFO(0, adapter_id)
    ZEND_ARG_INFO(0, reset)
    ZEND_ARG_INFO(0, self)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_Db_initmysql_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, configs, 1)
    ZEND_ARG_INFO(0, reset)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_db_qb_insert_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, ignore)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ proto object Asf_Db::init(array $configs [, int $adapter_id = 0 [, bool $reset = false]])
*/
PHP_METHOD(asf_Db, init)
{
    zval *configs = NULL;
    zend_long adapter_id = 0;
    zend_bool reset = 0;
    zval *self = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 4)
        Z_PARAM_ARRAY(configs)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(adapter_id)
        Z_PARAM_BOOL(reset)
        Z_PARAM_OBJECT(self)
    ZEND_PARSE_PARAMETERS_END();

    zval pdo, *cpdo = NULL;
    smart_str ins = {0};
    php_serialize_data_t var_hash;
    char md5str[33];
    unsigned char digest[16];
    PHP_MD5_CTX context;

    /* get instance */
    md5str[0] = '\0'; digest[0] = '\0';
    smart_str_append_long(&ins, adapter_id);
    smart_str_appendc(&ins, '_');

    PHP_VAR_SERIALIZE_INIT(var_hash);
    php_var_serialize(&ins, configs, &var_hash);
    PHP_VAR_SERIALIZE_DESTROY(var_hash);
    smart_str_0(&ins);

    PHP_MD5Init(&context);
    PHP_MD5Update(&context, ZSTR_VAL(ins.s), ZSTR_LEN(ins.s));
    PHP_MD5Final(digest, &context);
    make_digest_ex(md5str, digest, 16);
    smart_str_free(&ins);

    zval *zlinks = zend_read_static_property(asf_Db_ce, ZEND_STRL(LINKS), 1);
    if (!reset && !Z_ISNULL_P(zlinks) && IS_ARRAY == Z_TYPE_P(zlinks)) {
        if ((cpdo = zend_hash_str_find(Z_ARRVAL_P(zlinks), md5str, 32)) != NULL) {
            zval attr_args[1], *fpdo = NULL;

            /* PDO_ATTR_SERVER_INFO */
            ZVAL_LONG(&attr_args[0], 6);
            fpdo = zend_read_property(Z_OBJCE_P(cpdo), cpdo, ZEND_STRL("_dbh"), 1, NULL);
            zend_call_method_with_1_params(fpdo, Z_OBJCE_P(fpdo), NULL, "getattribute", NULL, attr_args);

            if (!EG(exception)) {
                zend_update_static_property(asf_Db_ce, ZEND_STRL(CLINK), cpdo);
                RETURN_TRUE;
            } else {
                zend_hash_str_del(Z_ARRVAL_P(zlinks), md5str, 32);
                zend_clear_exception();
            }
        }
    }

    ZVAL_UNDEF(&pdo);

    /* Connect/ReConnect DB */
    char *type = NULL;
    switch (adapter_id) {
        case 0: (void)asf_db_adapter_mysql_instance(&pdo); type = ASF_DB_TYPE_MYSQL; break;
        case 1: (void)asf_db_adapter_sqlite_instance(&pdo); type = ASF_DB_TYPE_SQLTILE; break;
        case 2: (void)asf_db_adapter_pgsql_instance(&pdo); type = ASF_DB_TYPE_PGSQL; break;
        default: (void)asf_db_adapter_mysql_instance(&pdo); type = ASF_DB_TYPE_MYSQL; break;
    }

    if (!asf_db_absadapter_instance(&pdo, type, configs)) {
        zval_ptr_dtor(&pdo);
        RETURN_FALSE;
    }

    zend_update_static_property(asf_Db_ce, ZEND_STRL(CLINK), &pdo);

    if (Z_ISNULL_P(zlinks)) {
        zval executor;
        array_init(&executor);

        add_assoc_zval_ex(&executor, md5str, 32, &pdo);
        zend_update_static_property(asf_Db_ce, ZEND_STRL(LINKS), &executor);
        zval_ptr_dtor(&executor);

        /* Free Resource, Fix leak in PHP 7.2.0 (cli)(NTS DEBUG) */
        if (self && Z_TYPE_P(self) == IS_OBJECT) {
            ASF_FUNC_REGISTER_SHUTDOWN_FUNCTION_CLOSE(self, 1);
        } else {
            zval this_ptr;
            object_init_ex(&this_ptr, asf_Db_ce);
            ASF_FUNC_REGISTER_SHUTDOWN_FUNCTION_CLOSE(&this_ptr, 0);
        }
    } else {
        zend_hash_str_add(Z_ARRVAL_P(zlinks), md5str, 32, &pdo);
        zend_update_static_property(asf_Db_ce, ZEND_STRL(LINKS), zlinks);
    }

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto object Asf_Db::Mysql(array $configs [, bool $reset = false])
*/
PHP_METHOD(asf_Db, initMysql)
{
    zval *configs = NULL;
    zend_bool reset = 0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ARRAY(configs)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(reset)
    ZEND_PARSE_PARAMETERS_END();

    zval args[4], zmn_1;
    asf_db_t this_ptr;

    object_init_ex(&this_ptr, asf_Db_ce);

    ZVAL_STRINGL(&zmn_1, "init", 4);
    ZVAL_COPY_VALUE(&args[0], configs);
    ZVAL_LONG(&args[1], 0);
    ZVAL_BOOL(&args[2], reset);
    ZVAL_OBJ(&args[3], Z_OBJ(this_ptr));

    call_user_function_ex(&asf_Db_ce->function_table, &this_ptr, &zmn_1, return_value, 4, args, 1, NULL);
    if (UNEXPECTED(EG(exception) && strcasecmp(ZSTR_VAL(EG(exception)->ce->name), "PDOException") == 0)) {
        zval exception_object;
        ZVAL_OBJ(&exception_object, EG(exception));

        zend_long code = zval_get_long(zend_read_property(EG(exception)->ce, &exception_object, ZEND_STRL("code"), 1, NULL));
        if (code && (code == 2002 || code == 2003)) {
            zend_clear_exception();
            ZVAL_BOOL(&args[2], 1);
            call_user_function_ex(&asf_Db_ce->function_table, &this_ptr, &zmn_1, return_value, 3, args, 1, NULL);
        }
    }

    ASF_FAST_STRING_PTR_DTOR(zmn_1);
    zval_ptr_dtor(&this_ptr);
    
    RETURN_TRUE;
}
/* }}} */

/* {{{ proto object Asf_Db::qbs(void)
*/
ASF_DB_QB_METHOD(select, qbs)
/* }}} */

/* {{{ proto object Asf_Db::qbi([bool $ignore = false])
*/
PHP_METHOD(asf_Db, qbi)
{
    zend_bool bignore = 0;
    zval zmethod, zignore;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &bignore) == FAILURE) {
        return;
    }

    ZVAL_BOOL(&zignore, bignore);

    object_init_ex(&zmethod, asf_db_qb_insert_ce);
    zend_call_method_with_1_params(&zmethod, asf_db_qb_insert_ce, NULL, "__construct", return_value, &zignore);
    zval_ptr_dtor(&zmethod);
}
/* }}} */

/* {{{ proto object Asf_Db::qbu(void)
*/
ASF_DB_QB_METHOD(update, qbu)
/* }}} */

/* {{{ proto object Asf_Db::qbd(void)
*/
ASF_DB_QB_METHOD(delete, qbd)
/* }}} */

/* {{{ proto mixed Asf_Db::__callStatic(string $function_name, array $args)
*/
PHP_METHOD(asf_Db, __callStatic)
{
    zval *function_name = NULL;
    zval *args = NULL;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL_DEREF(function_name)
        Z_PARAM_ZVAL_DEREF(args)
    ZEND_PARSE_PARAMETERS_END();

    zval *cur_link = zend_read_static_property(asf_Db_ce, ZEND_STRL(CLINK), 1);

    if (!cur_link || IS_OBJECT != Z_TYPE_P(cur_link)) {
        RETURN_FALSE;
    }

    size_t arg_count = zend_hash_num_elements(Z_ARRVAL_P(args));
    zval params[arg_count];

    if (arg_count) {
        zval *entry = NULL;
        size_t i = 0;
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(args), entry) {
            ZVAL_DEREF(entry);
            ZVAL_COPY_VALUE(&params[i++], entry);
        } ZEND_HASH_FOREACH_END();
    }

    call_user_function_ex(&Z_OBJCE_P(cur_link)->function_table, cur_link, function_name, return_value, arg_count, params, 1, NULL);

    /* If the method not found in class */
    if (!return_value || Z_TYPE_P(return_value) == IS_UNDEF) {
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto array Asf_Db::getLinks(void)
*/
PHP_METHOD(asf_Db, getLinks)
{
    ZVAL_COPY(return_value, zend_read_static_property(asf_Db_ce,
                ZEND_STRL(LINKS), 1));
}
/* }}} */

/* {{{ proto void Asf_Db::close(void)
*/
PHP_METHOD(asf_Db, close)
{
    zend_update_static_property_null(asf_Db_ce, ZEND_STRL(CLINK));
    zend_update_static_property_null(asf_Db_ce, ZEND_STRL(LINKS));
    RETURN_TRUE;
}
/* }}} */

/* {{{ asf_Db_methods[]
*/
zend_function_entry asf_Db_methods[] = {
    PHP_ME(asf_Db, init,            asf_Db_init_arginfo,            ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_Db, initMysql,       asf_Db_initmysql_arginfo,       ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_Db, __callStatic,    asf_absadapter_call_arginfo,    ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_Db, getLinks,        NULL,                           ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_Db, close,           NULL,                           ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_Db, qbs,             NULL,                           ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_Db, qbi,             asf_db_qb_insert_arginfo,       ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_Db, qbu,             NULL,                           ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_Db, qbd,             NULL,                           ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(Db) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_Db, Asf_Db, Asf\\Db, ZEND_ACC_FINAL);

    zend_declare_property_null(asf_Db_ce, ZEND_STRL(LINKS),     ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
    zend_declare_property_null(asf_Db_ce, ZEND_STRL(CLINK),     ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);

    ASF_INIT(db_adapterinterface);
    ASF_INIT(db_abstractadapter);
    ASF_INIT(db_adapter_mysql);
    ASF_INIT(db_adapter_sqlite);
    ASF_INIT(db_adapter_pgsql);

    ASF_INIT(db_absqb);
    ASF_INIT(db_qb_select);
    ASF_INIT(db_qb_insert);
    ASF_INIT(db_qb_update);
    ASF_INIT(db_qb_delete);

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
