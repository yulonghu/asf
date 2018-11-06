PHP_ARG_ENABLE(asf, whether to enable asf support,
Make sure that the comment is aligned:
[  --enable-asf            Enable asf support])

AC_ARG_ENABLE(asf-debug,
[  --enable-asf-debug      compile with debugging symbols, default=no],
[PHP_ASF_DEBUG=$enableval],
[PHP_ASF_DEBUG=no])

if test "$PHP_ASF" != "no"; then

  if test "$PHP_ASF_DEBUG" != "no"; then
    AC_DEFINE(PHP_ASF_DEBUG, 1, [Include debugging support in example])
  else
    AC_DEFINE(PHP_ASF_DEBUG, 0, [Include debugging support in example])
  fi

  AC_MSG_CHECKING([php-config])

  if test -z "$PHP_CONFIG"; then
    AC_MSG_ERROR([php-config not found])
  else
   AC_MSG_RESULT([$PHP_CONFIG, ok]) 
  fi

  php_version=`$PHP_CONFIG --version 2>/dev/null|head -n 1|awk -F '.' '{print $1}'`

  AC_MSG_CHECKING([PHP Version])
 
  if test -z "$php_version"; then
   AC_MSG_ERROR([php version invalid])
  fi

  if test "$php_version" -lt "7"; then
   AC_MSG_ERROR([Version of the minimum requirements can not be lower than PHP 7.0])
  else
   AC_MSG_RESULT([$php_version, ok]) 
  fi

  PHP_NEW_EXTENSION(asf, \
      asf.c \
      kernel/asf_func.c \
      asf_application.c \
      asf_config.c \
      config/asf_config_abstractconfig.c \
      config/asf_config_simple.c \
      config/asf_config_php.c \
      config/asf_config_ini.c \
      asf_dispatcher.c \
      asf_loader.c \
      asf_router.c \
      routes/asf_route_pathinfo.c \
      routes/asf_route_query.c \
      routes/asf_route_dot.c \
      http/asf_http_request.c \
      http/asf_http_requestinterface.c \
      http/asf_http_response.c \
      http/asf_http_responseinterface.c \
      http/asf_http_cookie.c \
      http/asf_http_cookieinterface.c \
      asf_service.c \
      asf_ensure.c \
      asf_exception.c \
      asf_sg.c \
      asf_logger.c \
      log/asf_log_loggerinterface.c \
      log/asf_log_level.c \
      log/asf_log_adapter.c \
      log/adapter/asf_log_adapter_file.c \
      log/adapter/asf_log_adapter_syslog.c \
      log/asf_log_formatterinterface.c \
      log/formatter/asf_log_formatter_file.c \
      log/formatter/asf_log_formatter_syslog.c \
      asf_db.c \
      db/asf_db_adapterinterface.c \
      db/asf_db_abstractadapter.c \
      db/adapter/asf_db_adapter_mysql.c \
      db/adapter/asf_db_adapter_sqlite.c \
      db/adapter/asf_db_adapter_pgsql.c \
      db/asf_db_abstractquerybuilder.c \
      db/querybuilder/asf_db_querybuilder_select.c \
      db/querybuilder/asf_db_querybuilder_insert.c \
      db/querybuilder/asf_db_querybuilder_update.c \
      db/querybuilder/asf_db_querybuilder_delete.c \
      asf_util.c \
      debug/asf_debug_dump.c \
      debug/dump/asf_debug_dump_abstractdump.c \
      debug/dump/asf_debug_dump_cli.c \
      debug/dump/asf_debug_dump_html.c \
      asf_cache.c \
      , $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
