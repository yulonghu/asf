--TEST--
Check for configuration file resident memory
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
asf.cache_config_enable=1
asf.cache_config_expire=60
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";

start();

file_put_contents(CONFIG_PATH . "/config.php", <<<PHP
<?php
return array(
    'asf' => array(
        'root_path'  => MODULE_PATH,
    )
);
PHP
);

file_put_contents(CONFIG_PATH . "/config.db.php", <<<PHP
<?php
return array(
    'db'  => array('host' => '127.0.0.1', 'user' => 'test', 'pass' => '', 'port' => 3306),
);
PHP
);

file_put_contents(CONFIG_PATH . "/config.redis.ini", <<<PHP
[db]
host=127.0.0.1:1228
pass=
PHP
);

var_dump(ini_get('asf.cache_config_enable'));
var_dump(ini_get('asf.cache_config_expire'));

$app = new Asf_Application(CONFIG_PATH . '/config.php');
var_dump($app->getConfig()->toArray());

$conf_php =  new Asf_Config_Php(CONFIG_PATH . '/config.db.php');
var_dump($conf_php->toArray());

$conf_ini =  new Asf_Config_Ini(CONFIG_PATH . '/config.redis.ini');
var_dump($conf_ini->toArray());

shutdown();
?>
--EXPECTF--
string(1) "1"
string(2) "60"
array(1) {
  ["asf"]=>
  array(1) {
    ["root_path"]=>
    string(%d) "%s"
  }
}
array(1) {
  ["db"]=>
  array(4) {
    ["host"]=>
    string(9) "127.0.0.1"
    ["user"]=>
    string(4) "test"
    ["pass"]=>
    string(0) ""
    ["port"]=>
    int(3306)
  }
}
array(1) {
  ["db"]=>
  array(2) {
    ["host"]=>
    string(14) "127.0.0.1:1228"
    ["pass"]=>
    string(0) ""
  }
}
