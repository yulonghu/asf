--TEST--
Check for Asf_Router for method name addRoute,getRoutes
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";

start();

$configs = array(
    'asf' => array(
        'root_path'  => MODULE_PATH,
        'modules' => 'admin',
    )
);

file_put_contents(APP_PATH . "/services/Index.php", <<<PHP
<?php
class IndexService
{
    public function indexAction()
    {
        return 'Index::index, Hello World';
    }

    public function loginAction()
    {
        return 'Index::login';
    }
}
PHP
);

file_put_contents(APP_PATH_ADMIN . "/services/User.php", <<<PHP
<?php
class UserService
{
    public function topAction()
    {
        return 'Admin::top, Hello World';
    }
}
PHP
);

class Bootstrap
{
    public function _initStart($dispatcher)
    {
        //$dispatcher->setDefaultAction('login');
    }

    public function _initOne($dispatcher)
    {
        $route = array(
            'module'  => 'admin',
            'service' => 'user',
            'action'  => 'top',
        );

        $route_index = array(
            'action'  => 'login',
        );

        $route_login = array(
            'module'  => 'api',
            'action'  => 'index'
        );

        var_dump($dispatcher->getRouter()->getRoutes());
        var_dump($dispatcher->getRouter()->addRoute('index:*', $route));
        var_dump($dispatcher->getRouter()->addRoute('index:index', $route_index));
        var_dump($dispatcher->getRouter()->addRoute('index:login', $route_login));

        print_r($dispatcher->getRouter()->getRoutes());
    }
}

$handle = new Asf_Application($configs);
$handle->bootstrap()->run();

shutdown();
?>
--EXPECTF--
NULL
bool(true)
bool(true)
bool(true)
Array
(
    [index:*] => Array
        (
            [module] => admin
            [service] => user
            [action] => top
        )

    [index:index] => Array
        (
            [action] => login
        )

    [index:login] => Array
        (
            [module] => api
            [action] => index
        )

)
{"errno":0,"data":"%s"}

