--TEST--
Check for Asf configs module name invalid
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.lowcase_path=0
asf.ctype_id=0
asf.action_suffix=1
asf.use_namespace=0
--FILE--
<?php
$configs = array(
    'asf' => array(
        'root_path'  => realpath(dirname(__FILE__)),
        'modules'    => 'admin,index,api',
        'dispatcher' => array(
            'default_module' => 'main'   
        )
    )
);

class IndexService
{
    public function indexAction()
    {
    }
}

try {
    $handle = new Asf_Application($configs);
    $handle->run();
} catch (Asf_Exception $e) {
    var_dump($e->getMessage());
}
?>
--EXPECTF--

