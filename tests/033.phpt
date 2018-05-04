--TEST--
Check for Asf_Application, get started No.7 for build
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";
start();

file_put_contents(APP_PATH . "/services/Main.php", <<<PHP
<?php
class MainService
{
    public function userAction()
    {
        \$data = Asf_Loader::get('UserLogic')->getUser();
        var_dump(\$data);
        var_dump(Asf_Loader::get('Found') ? 'Is Found' : 'Not Found');
        Asf_Ensure::notEmpty(\$data, Defconstants::ERR_TEST_CODE, 'Defconstants');
    }
}
PHP
);

file_put_contents(BOOTSTRAP_PATH . "/Bootstrap.php", <<<PHP
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

file_put_contents(LIBRARY_PATH . "/Defconstants.php", <<<PHP
<?php
class DefConstants
{
    const ERR_TEST_CODE = 500;

    public static \$ErrDescription = array(
        self::ERR_TEST_CODE => 'This is test default text',
    );
}
PHP
);

file_put_contents(APP_PATH . "/logics/User.php", <<<PHP
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

file_put_contents(LIBRARY_PATH . "/Found.php", <<<PHP
<?php
class Found
{
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
{"errno":0,"data":""}

