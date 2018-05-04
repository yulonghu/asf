--TEST--
Check for Asf_Router_Dot
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
asf.ctype_id=0
--FILE--
<?php
$configs = array(
    'asf' => array(
        'root_path' => realpath(dirname(__FILE__)),
        'dispatcher' => array(
            'route' => array(
                'type' => 'dot',
                'dot_name' => 'method',
            )
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

?>
--EXPECT--
Asf_Route_Dot Object
(
    [_var_name:protected] => method
)

