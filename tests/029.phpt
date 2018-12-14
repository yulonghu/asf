--TEST--
Check for Asf_Application && Test Asf_Loader::clean()
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.lowcase_path=1
asf.ctype_id=0
asf.use_namespace=0
--FILE--
<?php
ini_set('asf.lowcase_path', 1);
ini_set('asf.ctype_id', 0);

define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";
start();

file_put_contents(APP_PATH . "/services/main.php", <<<PHP
<?php
class MainService
{
    public \$data = null;
    public function userAction()
    {
        \$this->data = Asf_Loader::get('UserLogic')->getUser();
        var_dump(Asf_Loader::get('Api_UserLogic')->getUser(\$this->data));

        Asf_Loader::clean('UserLogic');
        Asf_Loader::clean('Api_UserLogic');

        \$this->data = Asf_Loader::get('UserLogic')->getUser();
        var_dump(Asf_Loader::get('Api_UserLogic')->getUser(\$this->data));
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
        print_r(\$this);
    }
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

if (!file_exists(APP_PATH . '/logics/api')) {
    mkdir(APP_PATH . '/logics/api', 0755, true);
    chmod(APP_PATH . '/logics/api', 0755);
}

file_put_contents(APP_PATH . "/logics/api/user.php", <<<PHP
<?php
class Api_UserLogic
{
    public \$data = null;
    public function __construct()
    {
        \$this->data = 'This is ';
    }
    public function getUser(\$data)
    {
        return \$this->data . \$data;
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
Bootstrap Object
(
)
string(16) "This is zhangsan"
string(16) "This is zhangsan"

