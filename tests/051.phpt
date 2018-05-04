--TEST--
Check for Asf_Application::init(require xxx.ini) 
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.ctype_id=0
asf.use_namespace=0
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";
start();

file_put_contents(APP_PATH . "/services/Index.php", <<<PHP
<?php
class IndexService
{
    public function indexAction()
    {
    }
}
PHP
);

file_put_contents(MODULE_PATH . "/constants/Constants.php", <<<PHP
<?php
class Constants
{
    const ERR_INDEX_SUCCESS = 'constants-bootstrap';
}
PHP
);

file_put_contents(MODULE_PATH . "/bootstrap/Bootstrap.php", <<<PHP
<?php
class Bootstrap
{
    public function _initFirst()
    {
        var_dump('bootstrap_1');
    }

    public function _initSecond()
    {
        var_dump('bootstrap-2');
    }

    public function _initThree()
    {
        var_dump(Constants::ERR_INDEX_SUCCESS);
    }

    public function _initFour()
    {
        trigger_error("Notice: This is Test.");
    }
}
PHP
);

$configs = array(
    'asf' => array(
        'root_path'  => MODULE_PATH,
        'log_path'   => LOG_PATH,
        'bootstrap'  => MODULE_PATH . '/bootstrap/Bootstrap.php',
        'constants'  => MODULE_PATH . '/constants/Constants.php',
        'dispatcher' => array(
            'log' => array(
                'err' => 1
            )
        )
    )
);

$handle = new Asf_Application($configs);
$handle->constants()->bootstrap()->run();

shutdown();
?>
--EXPECTF--
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"

