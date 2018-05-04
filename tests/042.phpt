--TEST--
Check for Asf_Config_Php
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
asf.ctype_id=0
--FILE--
<?php
define('ROOT_PATH', __DIR__ . '/configs/');

if (!file_exists(ROOT_PATH)) {
    mkdir(ROOT_PATH, 0755, true);
}

file_put_contents(ROOT_PATH . "/conf.php", <<<PHP
<?php
return array(
    'host' => '127.0.0.1',
    'port' => 6379,
    'password' => '123456',
    'timeout' => 1,
    'persistent' => 0,
    'other' => array(
        'pass' => '654321'
    )
);
PHP
);

$config = new Asf_Config_Php(ROOT_PATH . "/conf.php");

var_dump($config->get('host1', 'none'));
var_dump($config->get('host'));
var_dump($config->get('other')->pass);
var_dump($config->host);
print_r($config->other->toArray());

unlink(ROOT_PATH . '/conf.php');
if (is_dir(ROOT_PATH)) {
    @rmdir(ROOT_PATH);
}
?>
--EXPECTF--
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
Array
(
    [pass] => 654321
)

