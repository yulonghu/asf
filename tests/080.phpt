--TEST--
Check for Asf_Loader::import load the wrong configuration file
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.ctype_id=1
asf.use_namespace=0
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";

start();

error_reporting(0);

file_put_contents(CONFIG_PATH . "/config.php", <<<PHP
<?php
return array(
    'asf' => array(
        'root_path'  => MODULE_PATH,
        'xx' => 1
        'yy' => 2
    )
);
PHP
);

var_dump(Asf_Loader::import(CONFIG_PATH . "/config.php"));

shutdown();
?>
--EXPECT--
