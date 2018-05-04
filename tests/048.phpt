--TEST--
Check for Asf_Config_Ini for section arg 
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

$config = new Asf_Config_Ini(CONFIG_PATH . "/conf.ini", "MySQL");

print_r($config->toArray());
var_dump($config->host);

shutdown();
?>
--EXPECTF--
Array
(
    [host] => 127.0.0.2
    [port] => 3306
    [password] => 123456
)
string(9) "127.0.0.2"

