--TEST--
Check for Asf_Response to serialize
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.ctype_id=2
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
        return 'hello world';
    }
}

$handle = new Asf_Application($configs);
$handle->run();

?>
--EXPECT--
i:0;s:11:"hello world";

