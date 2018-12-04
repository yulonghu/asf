--TEST--
Check for Asf_Application load the wrong configuration file
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.ctype_id=1
asf.use_namespace=0
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";

start();

error_reporting(0);

file_put_contents(APP_PATH . "/services/Index.php", <<<PHP
<?php
class IndexService extends Asf_AbstractService
{
    public function indexAction()
    {
    }
}
PHP
);

file_put_contents(CONFIG_PATH . "/config.php", <<<PHP
<?php
return array(
    'asf' => array(
        'root_path'  => MODULE_PATH,
        'xx' => 1
        'yy' => 2
    )
);
PHP
);

$handle = new Asf_Application(CONFIG_PATH . '/config.php');
$handle->run();

shutdown();
?>
--EXPECT--
