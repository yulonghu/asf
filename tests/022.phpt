--TEST--
Check for Asf_Application::getInstance()
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.ctype_id=0
asf.use_namespace=0
--FILE--
<?php
$configs = array(
    'asf' => array(
        'root_path'  => realpath(dirname(__FILE__)),
    )
);

class IndexService
{
    public function indexAction()
    {
    }
}

echo '------uninitialized-------', PHP_EOL;
var_dump(Asf_Application::getInstance());

echo '------initialized-------', PHP_EOL;
$handle = new Asf_Application($configs);
$handle->run();
print_r(Asf_Application::getInstance());
?>
--EXPECTF--
------uninitialized-------
NULL
------initialized-------
Asf_Application Object
(
    [conf:protected] => Asf_Config_Simple Object
        (
            [_conf:protected] => Array
                (
                    [asf] => Array
                        (
                            [root_path] => %s
                        )

                )

        )

    [disp:protected] => Asf_Dispatcher Object
        (
            [_request:protected] => Asf_Http_Request Object
                (
                    [_module:protected] => api
                    [_action:protected] => index
                    [_service:protected] => index
                    [_method:protected] => cli
                    [_params:protected] => Array
                        (
                        )

                    [_uri:protected] => 
                    [_base_uri:protected] => 
                )

            [_router:protected] => Asf_Route_Pathinfo Object
                (
                )

            [_r:protected] => 
        )

    [_env:protected] => %s
    [_m:protected] => %d
    [_ef:protected] => %s
    [_eh:protected] => 
)

