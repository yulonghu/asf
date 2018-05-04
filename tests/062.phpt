--TEST--
Check for Asf_Router_Query
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
asf.ctype_id=0
--FILE--
<?php
$a = array(
    'type' => 'query',
    'module_name'  => 'm',
    'service_name' => 'c',
    'action_name'  => 'a',
);

$configs = array(
    'asf' => array(
        'root_path' => realpath(dirname(__FILE__)),
        'dispatcher' => array(
            'route' =>  & $a
        )
    )
);

class IndexService
{
    public function indexAction()
    {
        print_r(Asf_Dispatcher::getInstance()->getRouter());
    }
}

$handle = new Asf_Application($configs);
$handle->run();

print_r($handle->getConfig()->get('asf')->get('dispatcher')->get('route'));

?>
--EXPECT--
Asf_Route_Pathinfo Object
(
)
Array
(
    [type] => query
    [module_name] => m
    [service_name] => c
    [action_name] => a
)

