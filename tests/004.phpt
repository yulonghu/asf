--TEST--
Check for Asf_Http_Request
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php
try {
    $request = new Asf_Http_Request('requestUri', 'baseUri', false);
    print_r($request);
} catch (TypeError $e) {
    var_dump($e->getMessage());
}

$request = new Asf_Http_Request('xxxxxxxxxxxxxxxx');
print_r($request);

echo '------is value-------', PHP_EOL;
var_dump($request->isGet());
var_dump($request->isPost());
var_dump($request->isPut());
var_dump($request->isDelete());
var_dump($request->isPatch());
var_dump($request->isHead());
var_dump($request->isOptions());
var_dump($request->isCli());
var_dump($request->isTrace());
var_dump($request->isConnect());
var_dump($request->isFlashRequest());
var_dump($request->isPropFind());

echo '------get value-------', PHP_EOL;
var_dump($request->getServer("xxx"));
var_dump($request->getEnv("xxx"));
var_dump($request->getModuleName());
var_dump($request->getServiceName());
var_dump($request->getActionName());
var_dump($request->getMethod());
var_dump($request->getBaseUri());
var_dump($request->getRequestUri());

echo '------default value-------', PHP_EOL;
var_dump($request->getQuery("xxx", 'defaultQuery'));
var_dump($request->getServer("xxx", 'defaultServer'));
var_dump($request->getPost("xxx", 'defaultPost'));
var_dump($request->getCookie("xxx", 'defaultCookie'));
var_dump($request->getEnv("xxx", 'defaultEnv'));

echo '------params-------', PHP_EOL;
var_dump($request->getParam('xxxx'));
var_dump($request->getParams());

echo '------has-------', PHP_EOL;
var_dump($request->hasPost('xxx'));
var_dump($request->hasQuery('xxx'));
var_dump($request->hasServer('xxx'));
var_dump($request->hasFiles('xxx'));
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

    [_uri:protected] => xxxxxxxxxxxxxxxx
    [_base_uri:protected] => 
)
------is value-------
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
------get value-------
NULL
NULL
string(%d) "api"
string(%d) "index"
string(%d) "index"
string(%d) "cli"
string(%d) ""
string(%d) "xxxxxxxxxxxxxxxx"
------default value-------
string(%d) "defaultQuery"
string(%d) "defaultServer"
string(%d) "defaultPost"
string(%d) "defaultCookie"
string(%d) "defaultEnv"
------params-------
NULL
array(0) {
}
------has-------
bool(false)
bool(false)
bool(false)
bool(false)

