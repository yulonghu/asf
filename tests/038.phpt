--TEST--
Check for Asf_Config_Simple::get/set
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php
$config_mysql = array(
    'host' => '127.0.0.1',
    'user' => 'mysql',
    'password' => '654321',
    'port' => 3306
);

$config = new Asf_Config_Simple($config_mysql);
var_dump($config->get('host'));
var_dump($config->get('auth1', 'none'));
?>
--EXPECT--
string(9) "127.0.0.1"
string(4) "none"

