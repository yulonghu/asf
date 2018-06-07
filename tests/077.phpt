--TEST--
Check for Asf_Debug
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php
define('VER_STR', 'VERSION_ID_100');

ob_start();

$b_check = false;
$b_check_1 = true;
$n_check = null;
$i_check = 100;
$d_check = 100.05;
$s_check = 'This is Asf';

$a_check = array(
    'aaa',
    'bbb' => array(
        'aaa_1' => 100,
        'bbb_1' => array(
            'ccc_2',
            'ccc_3' => 'world',
            'ccc_4' => VER_STR
        )
    ),
    'a' => false,
    'b' => true,
    'c' => null,
    'd' => 100.05,
    '"dd' => '100.01'
);

$r_check = fopen(__DIR__ . '/077.phpt', 'r');

Asf_Debug_Dump::vars($b_check, $b_check_1, $n_check, $i_check, $d_check, $s_check, $a_check, $r_check);

fclose($r_check);

class Test
{
    public $a = NULL;
    protected $b = array();
    private $c = 1;

    public function user()
    {
    
    }
}

$nt = new Test();

$a_check['o'] = $nt;
$o_check = (object)$a_check;

Asf_Debug_Dump::vars($nt, $o_check);
ob_clean();

$h_cli = new Asf_Debug_Dump_Cli();
var_dump($h_cli, $h_cli);
$h_cli = null;

$h_html = new Asf_Debug_Dump_Html();
var_dump($h_html, $h_html);
$h_html = null;

?>
--EXPECTF--
object(Asf_Debug_Dump_Cli)#%d (%d) {
  ["%s":protected]=>
  int(0)
}
object(Asf_Debug_Dump_Cli)#%d (%d) {
  ["%s":protected]=>
  int(0)
}
object(Asf_Debug_Dump_Html)#%d (%d) {
  ["%s":protected]=>
  int(1)
}
object(Asf_Debug_Dump_Html)#%d (%d) {
  ["%s":protected]=>
  int(1)
}

