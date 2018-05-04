--TEST--
Check for Asf_Application (bootstrap/constants/run)
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.lowcase_path=1
asf.ctype_id=1
asf.use_namespace=0
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";
start();

ini_set('asf.lowcase_path', 1);
ini_set('asf.ctype_id', 1);

file_put_contents(APP_PATH . "/services/index.php", <<<PHP
<?php
class IndexService
{
    public function indexAction()
    {
        return Constants::ERR_INDEX_SUCCESS;
    }
}
PHP
);

file_put_contents(BOOTSTRAP_PATH . "/bootstrap.php", <<<PHP
<?php
class Bootstrap
{
    public function _initHow()
    {
        var_dump(Asf_Loader::get('UserLogic')->getUser());
    }
}
PHP
);

file_put_contents(CONSTANTS_PATH . "/constants/constants.php", <<<PHP
<?php
class Constants
{
    const ERR_INDEX_SUCCESS = 'hello world';
}
PHP
);

file_put_contents(APP_PATH . "/logics/user.php", <<<PHP
<?php
class UserLogic
{
    public function getUser()
    {
        return 'zhangsan';
    }
}
PHP
);

$configs = array(
    'asf' => array(
        'root_path'  => MODULE_PATH,
        'constants'  => MODULE_PATH . '/constants/constants.php',
    )
);

$handle = new Asf_Application($configs);
$handle->constants()->bootstrap()->run();

shutdown();
?>
--EXPECT--
string(8) "zhangsan"
{"errno":0,"errmsg":"","data":"hello world"}

