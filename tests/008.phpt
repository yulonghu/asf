--TEST--
Check for Asf_Response to json
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
asf.ctype_id=5
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
    }
}

$handle = new Asf_Application($configs);
$handle->run();

?>
--EXPECT--
{"errno":0,"data":""}

