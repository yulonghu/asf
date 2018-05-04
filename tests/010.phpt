--TEST--
Check for Asf_Response to var_dump
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.ctype_id=3
asf.use_namespace=0
--FILE--
<?php
$configs = array(
    'asf' => array(
        'root_path' => realpath(dirname(__FILE__)),
    )
);

class IndexService
{
    public function indexAction()
    {
        return array();
    }
}

$handle = new Asf_Application($configs);
$handle->run();

?>
--EXPECT--
array(0) {
}

