--TEST--
Check for Asf_Config_Ini
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";
start();

file_put_contents(CONFIG_PATH . "/conf.ini", <<<PHP
<?php
[MySQL]
    host = 127.0.0.2
    port = 3306
    password = 123456
[Redis]
    host = 127.0.0.1
    port = 6379
    password = 123456,
    timeout = 1
    persistent = 0
    other[pass] =  654321
PHP
);

$config = new Asf_Config_Ini(CONFIG_PATH . "/conf.ini");

var_dump($config->get('MySQL')->host);
var_dump($config->get('Redis')->other->pass);
var_dump($config->get('Redis1', 'none'));
print_r($config->get('MySQL')->toArray());

shutdown();

?>
--EXPECTF--
string(9) "127.0.0.2"
string(6) "654321"
string(4) "none"
Array
(
    [host] => 127.0.0.2
    [port] => 3306
    [password] => 123456
)

