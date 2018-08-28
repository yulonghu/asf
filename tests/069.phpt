--TEST--
For Application, Check for set_error_handler, set_exception_handler on PHP functions
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
asf.ctype_id=0
--FILE--
<?php
ini_set('asf.ctype_id', 0);
ini_set('error_reporting', 0);
ini_set('display_errors', 0);

define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";
start();

class IndexService
{
    public function indexAction()
    {   
        echo $a; 
        echo $b; 

        var_dump(error_get_last());

        echo cs, PHP_EOL;

        try {
            new a();
        } catch (Exception $e) {
            var_dump($e->getMessage());
        }   
    }   
}

$configs = array(
    'asf' => array(
        'root_path'  => MODULE_PATH,
        'log_path'   => LOG_PATH,
        'dispatcher' => array(
            'log' => array(
                'err' => 1
            )
        )
    )
);

$handle = new Asf_Application($configs);
$handle->run();

shutdown();
?>
--EXPECTF--
NULL
cs
string(%d) "%s"

