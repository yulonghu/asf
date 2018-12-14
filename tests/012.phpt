--TEST--
Check for Lowercase method
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

class indexservice
{
    public function indexaction()
    {
        return array('user' => 'zhangsan');
    }
}

$handle = new Asf_Application($configs);
$handle->run();

?>
--EXPECT--
{"errno":0,"data":{"user":"zhangsan"}}

