--TEST--
Check for Asf_Util
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php

echo '---------------------ip', PHP_EOL;
var_dump(Asf_Util::ip());

echo '---------------------guid', PHP_EOL;
/* (compatibility) Some versions have no value */
Asf_Util::guid();

echo '---------------------bytes2string', PHP_EOL;
var_dump(Asf_Util::bytes2string(2048.56));
var_dump(Asf_Util::bytes2string(1073741824, 0));
var_dump(Asf_Util::bytes2string(3645.44, 0));
var_dump(Asf_Util::bytes2string(0));

echo '---------------------isEmail', PHP_EOL;
var_dump(Asf_Util::isEmail('fanjiapeng@box3.cn'));
var_dump(Asf_Util::isEmail('@126.com'));
var_dump(Asf_Util::isEmail('1111@.com'));
var_dump(Asf_Util::isEmail('11111-aaaa@163.com'));
var_dump(Asf_Util::isEmail('222222aaabb@yahoo'));
var_dump(Asf_Util::isEmail('222222aaabb@yahoo.'));

echo '---------------------fileExt', PHP_EOL;
var_dump(Asf_Util::fileExt("test.Doc"));

echo '---------------------cpass', PHP_EOL;
var_dump(Asf_Util::cpass(123456));

echo '---------------------iniSets', PHP_EOL;
$arr = array('error_reporting' => E_ALL, 'memory_limit' => '2M');
var_dump(Asf_Util::iniSets($arr));
var_dump(ini_get('error_reporting'));
var_dump(ini_get('memory_limit'));

echo '---------------------atrim', PHP_EOL;
$ta_y = array(' y-01', ' y-02 ');
$ta = array('1 ', ' 2', 'id' => ' 333 ', 'cat_id' => 4, '6' => ' 5000', ' hello ', array(' this is hello', 10000001, 0.1, 'y' => & $ta_y));
$result = Asf_Util::atrim($ta); 
var_dump($result);

var_dump(Asf_Util::atrim('')); 
var_dump(Asf_Util::atrim('%20')); 

echo '---------------------filterXss', PHP_EOL;
var_dump(Asf_Util::filterXss("<input type='text' value='<>'>"));
var_dump(Asf_Util::filterXss('<img src="x" onerror="&#97;&#108;&#101;&#114;&#116;&#40;&#49;&#41;">'));
var_dump(Asf_Util::filterXss('<a href="javasc&NewLine;ript&colon;alert(1)">click</a>'));
var_dump(Asf_Util::filterXss('<a href="data:text/html;base64, PGltZyBzcmM9eCBvbmVycm9yPWFsZXJ0KDEpPg==">test</a>'));

echo '---------------------getUrl', PHP_EOL;
/* (compatibility) Some versions have no value */

gettype(Asf_Util::getUrl("http://www.box3.cn", array('a', 'b' => 'a')));
gettype(Asf_Util::postUrl("http://www.box3.cn", array('user' => 'zhangsan', 'code' => 999999)));
gettype(Asf_Util::getUrl("https://10111.11.11.11/index.php"));
gettype(Asf_Util::postUrl("https://10111.11.11.11/index.php"));


?>
--EXPECTF--
---------------------ip
bool(false)
---------------------guid
---------------------bytes2string
string(10) "2.00055 KB"
string(3) "1GB"
string(6) "3.56KB"
string(7) "0 Bytes"
---------------------isEmail
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
---------------------fileExt
string(3) "doc"
---------------------cpass
array(2) {
  [0]=>
  string(%d) "%s"
  [1]=>
  string(%d) "%s"
}
---------------------iniSets
bool(true)
string(%d) "%s"
string(%d) "%s"
---------------------atrim
array(7) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
  ["id"]=>
  string(3) "333"
  ["cat_id"]=>
  int(4)
  [6]=>
  string(4) "5000"
  [7]=>
  string(5) "hello"
  [8]=>
  array(4) {
    [0]=>
    string(13) "this is hello"
    [1]=>
    int(10000001)
    [2]=>
    float(0.1)
    ["y"]=>
    &array(2) {
      [0]=>
      string(4) "y-01"
      [1]=>
      string(4) "y-02"
    }
  }
}
string(0) ""
string(3) "%20"
---------------------filterXss
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
---------------------getUrl

