--TEST--
Check for Asf_Application others public function name
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
asf.ctype_id=0
--FILE--
<?php
ini_set('asf.ctype_id', 0);

$configs = array(
    'asf' => array(
        'root_path' => realpath(dirname(__FILE__)),
        'modules' => 'aDmin,indEx',
    )
);

class IndexService
{
    public function indexAction()
    {
    }
}

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
$handle->bootstrap()->run();

echo '------public function name-------', PHP_EOL;

var_dump($handle->getModules());
var_dump($handle->getRootPath());
var_dump($handle->getEnv());
var_dump($handle->getMode());
var_dump($handle->setLogErrFileName("Asf.Err.Log." . date('Y-m-d')));
?>
--EXPECTF--
_initOne
_initTwo
------public function name-------
array(2) {
  [0]=>
  string(5) "aDmin"
  [1]=>
  string(5) "indEx"
}
string(%d) "%s"
string(%d) "%s"
int(%d)
bool(true)

