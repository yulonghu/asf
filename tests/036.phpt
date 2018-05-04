--TEST--
Check for Asf_Application INI configs
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php
define('ROOT_PATH', __DIR__);

class IndexService
{
}

$configs = array(
    'asf' => array(
        'root_path'  => __DIR__,
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

