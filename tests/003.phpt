--TEST--
Check all the methods of the Asf_Dispatcher class
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
        'root_path' => MODULE_PATH,
        'modules' => 'admin,test,111',
    )
);

file_put_contents(MODULE_PATH . "/Bootstrap.php", <<<PHP
<?php
class Bootstrap
{
    public function _initHow(\$dispatcher)
    {
        \$dispatcher->setDefaultModule('api')->setDefaultService('index')
            ->setDefaultAction('check')->throwException(1);

        \$dispatcher->setDefaultModule(111);

        try {
            \$dispatcher->setDefaultModule('xx');
        } catch (Exception \$e) {
            var_dump(\$e->getMessage());
        }

        try {
            \$dispatcher->setDefaultService('');
        } catch (Exception \$e) {
            var_dump(\$e->getMessage());
        }

        try {
            \$dispatcher->setDefaultAction('');
        } catch (Exception \$e) {
            var_dump(\$e->getMessage());
        }

        print_r(\$dispatcher->getRequest());
    }
}
PHP
);

$handle = new Asf_Application($configs);
$handle->bootstrap();

shutdown();
?>
--EXPECTF--
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
Asf_Http_Request Object
(
    [_module:protected] => 111
    [_action:protected] => check
    [_service:protected] => index
    [_method:protected] => cli
    [_params:protected] => Array
        (
        )

    [_uri:protected] => 
    [_base_uri:protected] => 
)

