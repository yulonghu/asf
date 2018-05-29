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

$handle = new Asf_Application(CONFIG_PATH . '/config.php');

var_dump($handle->getConfig()->toArray());
var_dump(ini_get('asf.cache_config_enable'));
var_dump(ini_get('asf.cache_config_expire'));

shutdown();
?>
--EXPECTF--
array(1) {
  ["asf"]=>
  array(1) {
    ["root_path"]=>
    string(%d) "%s"
  }
}
string(%d) "%d"
string(%d) "%d"

