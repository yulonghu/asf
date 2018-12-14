--TEST--
Check for Asf_Application, get started No.5 for build
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.lowcase_path=1
asf.ctype_id=0
asf.use_namespace=0
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";
start();

ini_set('asf.lowcase_path', 1);
ini_set('asf.ctype_id', 0);

file_put_contents(APP_PATH . "/services/main.php", <<<PHP
<?php
class MainService
{
    public function userAction()
    {
        \$obj = new UserLogic();
        var_dump(\$obj->getUser());

        var_dump(Asf_Loader::get('UserLogic')->getUser());

    }
}
PHP
);

file_put_contents(MODULE_PATH . "/bootstrap.php", <<<PHP
<?php
class Bootstrap
{
    public function _initHow()
    {
        var_dump('hello world');
    }
}
PHP
);

file_put_contents(APP_PATH . "/logics/user.php", <<<PHP
<?php
class UserLogic extends ParentUserLogic
{
    public function getUser()
    {
        return 'zhangsan';
    }
}
PHP
);

file_put_contents(APP_PATH . "/logics/parentuser.php", <<<PHP
<?php
class ParentUserLogic
{
    public function __construct()
    {
        var_dump('Parentuser');     
    }
}
PHP
);

$configs = array(
    'asf' => array(
        'root_path'  => MODULE_PATH,
        'dispatcher' =>  array(
            'default_service' => 'main',
            'default_action'  => 'user'
        )
    )
);

$handle = new Asf_Application($configs);
$handle->bootstrap()->run();

shutdown();
?>
--EXPECTF--
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"

