--TEST--
Check for Asf_Application, No found IndexService
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";

$configs = array(
    'asf' => array(
        'root_path' => MODULE_PATH,
    )
);

try {
    $handle = new Asf_Application($configs);
    $handle->run();
} catch (Asf_Exception $e) {
    var_dump($e->getMessage());
}
?>
--EXPECTF--
string(%d) "%s"

