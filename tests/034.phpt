--TEST--
Check for Asf_Application, get started No.7 for build
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.lowcase_path=1
asf.use_namespace=0
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";
start();

ini_set('asf.lowcase_path', 1);

file_put_contents(APP_PATH . "/services/main.php", <<<PHP
<?php
class MainService
{
    public function userAction()
    {
        \$data = Asf_Loader::get('UserLogic')->getUser();
        Asf_Ensure::notEmpty(\$data, defConstants::ERR_TEST_CODE, 'Defconstants');
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
        var_dump('hello world');
    }
}
PHP
);

file_put_contents(LIBRARY_PATH . "/defconstants.php", <<<PHP
<?php
class defConstants
{
    const ERR_TEST_CODE = 500;

    public static \$ErrDescription = array(
        self::ERR_TEST_CODE => 'This is test default text',
    );
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
--EXPECT--
string(11) "hello world"
{"errno":0,"data":""}

