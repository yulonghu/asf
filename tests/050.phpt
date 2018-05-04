--TEST--
Check for Asf_Application::init/getInstance
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.ctype_id=0
asf.use_namespace=0
--FILE--
<?php
ini_set('asf.ctype_id', 0);
ini_set('asf.use_namespace', 0);

$configs = array(
    'asf' => array(
        'root_path' => realpath(dirname(__FILE__)),
    )
);

class IndexService extends Asf_AbstractService
{
    public function indexAction()
    {
        var_dump('hi, asf');
    }
}

$obj = Asf_Application::init($configs)->run();
Asf_Application::getInstance();

/* Error */
try {
    $obj->task();
} catch (Error $e) {
    var_dump($e->getMessage());
}

/* Error */
try {
    $obj->run();
} catch (Error $e) {
    var_dump($e->getMessage());
}

/* Error */
try {
    $app = new Asf_Application($configs);
    $app->init($configs);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

/* Error */
try {
    $app = new Asf_Application($configs);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

?>
--EXPECTF--
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"

