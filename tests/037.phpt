--TEST--
Check for Asf_Config_Simple
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php
$config_default = array(
    'asf' => array(
        'root_path'  => __DIR__,
    )
);

$config_mysql = array(
    'host' => '127.0.0.1',
    'user' => 'mysql',
    'password' => '654321',
    'port' => 3306
);

$conf_a = new Asf_Config_Simple($config_default);
print_r($conf_a);

$conf_b = new Asf_Config_Simple($config_mysql);
print_r($conf_b);

try {
    $conf_b = new Asf_Config_Simple('ss');
} catch (Asf_Exception $e) {
    var_dump($e->getMessage());
}

?>
--EXPECTF--
Asf_Config_Simple Object
(
    [_conf:protected] => Array
        (
            [asf] => Array
                (
                    [root_path] => %s
                )

        )

)
Asf_Config_Simple Object
(
    [_conf:protected] => Array
        (
            [host] => 127.0.0.1
            [user] => mysql
            [password] => 654321
            [port] => 3306
        )

)

Warning: %s

