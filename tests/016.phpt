--TEST--
Check for Asf configs to dispatcher.default_service/default_action
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.lowcase_path=0
asf.ctype_id=1
asf.use_namespace=0
--FILE--
<?php
$configs = array(
    'asf' => array(
        'root_path' => realpath(dirname(__FILE__)),
        'dispatcher' => array(
            'default_service' => 'main',
            'default_action'  => 'main'
        ),
    )
);

class MainService
{
    public function mainAction()
    {
    }
}

$handle = new Asf_Application($configs);
$handle->run();

?>
--EXPECT--
{"errno":0,"errmsg":"","data":""}

