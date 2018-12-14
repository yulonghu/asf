--TEST--
Check for Asf_Application::getConfig() 
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.ctype_id=0
asf.use_namespace=0
--FILE--
<?php
$configs = array(
    'asf' => array(
        'root_path' => realpath(dirname(__FILE__)),
        'boostrap' => realpath(dirname(__FILE__)) . '/Bootstrap.php',
        'base_uri' => 'aaa/bbb',
        'modules'  => 'admin,index,api,rpc,main',
        'dispatcher' => array(
            'default_module'  => 'main',
            'default_service' => 'main',
            'default_action'  => 'main',
        )
    )
);

class MainService
{
    public function mainAction()
    {
    }
}

$handle = new Asf_Application($configs);
$handle->run();
var_dump($handle->getConfig());
print_r($handle->getConfig()->get('asf')->get('dispatcher'));
print_r($handle->getConfig()->asf->dispatcher);

var_dump($handle->getConfig()->get('asf')->get('root_path'));
var_dump($handle->getConfig()->asf->root_path);
?>
--EXPECTF--
object(Asf_Config_Simple)#%d (%d) {
  ["_conf":protected]=>
  array(1) {
    ["asf"]=>
    array(5) {
      ["root_path"]=>
      string(%d) "%s"
      ["boostrap"]=>
      string(%d) "%s"
      ["base_uri"]=>
      string(7) "aaa/bbb"
      ["modules"]=>
      string(24) "admin,index,api,rpc,main"
      ["dispatcher"]=>
      array(3) {
        ["default_module"]=>
        string(4) "main"
        ["default_service"]=>
        string(4) "main"
        ["default_action"]=>
        string(4) "main"
      }
    }
  }
}
Asf_Config_Simple Object
(
    [_conf:protected] => Array
        (
            [default_module] => main
            [default_service] => main
            [default_action] => main
        )

)
Asf_Config_Simple Object
(
    [_conf:protected] => Array
        (
            [default_module] => main
            [default_service] => main
            [default_action] => main
        )

)
string(%d) "%s"
string(%d) "%s"

