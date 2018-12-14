--TEST--
Check for Asf_Sg 
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
        var_dump(Asf_Sg::\$inputs);
        var_dump(Asf_Sg::\$inputs['get']);
        var_dump(Asf_Sg::\$inputs['post']);
        var_dump(Asf_Sg::\$inputs['cookie']);
    }
}
PHP
);

$configs = array(
    'asf' => array(
        'root_path'  => MODULE_PATH,
    )
);

$handle = new Asf_Application($configs);
$handle->run();

shutdown();
?>
--EXPECT--
array(3) {
  ["get"]=>
  array(0) {
  }
  ["post"]=>
  array(0) {
  }
  ["cookie"]=>
  array(0) {
  }
}
array(0) {
}
array(0) {
}
array(0) {
}

