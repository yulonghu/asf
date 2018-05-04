--TEST--
Check for Asf_Response return none
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.ctype_id=0
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
        echo 123456;
        return array();
    }
}

$handle = new Asf_Application($configs);
$handle->run();

?>
--EXPECT--
123456

