--TEST--
Check for Asf_AbstractService/getRequest()/getResponse()
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.ctype_id=0
asf.use_namespace=0
--FILE--
<?php
ini_set('asf.ctype_id', 0);

class IndexService extends Asf_AbstractService
{
    public function IndexAction($user = 'test', $pass)
    {
        var_dump($user);
        print_r($this->getRequest());
        print_r($this->getResponse());
    }
}

$configs = array(
    'asf' => array(
        'root_path'  => __DIR__,
    )
);

$handle = new Asf_Application($configs);
$handle->run();

?>
--EXPECTF--
string(%d) "%s"
Asf_Http_Request Object
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
Asf_Http_Response Object
(
    [_v:protected] => 
    [_h:protected] => 
)

