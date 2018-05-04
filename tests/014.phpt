--TEST--
Check for Asf_Application::bootstrap()
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php
$configs = array(
    'asf' => array(
        'root_path' => realpath(dirname(__FILE__)),
    )
);

class Bootstrap
{
    public function _initOne()
    {
        echo '_initOne', PHP_EOL;
    }


    public function _initTwo()
    {
        echo '_initTwo', PHP_EOL;
    }
}

$handle = new Asf_Application($configs);
$handle->bootstrap();

?>
--EXPECT--
_initOne
_initTwo

