<?php
/**
 * 各日志函数的基准测试
 *
 * @Author: fanjiapeng@126.com
 * @Date 2018/11/21 10:30:00
 *
 * LOGINFO: 20-Nov-2018 11:27:54 Asia/Shanghai [INFO] Hello World
 */

$gTotal = 10000;
$gMsg = 'Hello World';
$gFullPath0 = './asf_logger.0.log';
$gFullPath1 = './asf_logger.1.log';
$gFullPath2 = './asf_logger.2.log';
$gFullPath3 = './asf_logger.3.log';
$gFullPath4 = './asf_logger.4.log';
$gCounter = 0;
$gConsume = 0;
$gRepeat = 40;

date_default_timezone_set('Asia/Shanghai');

function testAsfLogger()
{
    global $gTotal, $gMsg, $gFullPath0;
    $i = 0;

    $logger = new Asf\Log\Adapter\File('asf_logger.0.log', __DIR__);

    while($i++ < $gTotal) {
        $logger->info($gMsg);
    }
}

function testFilePutContents()
{
    global $gTotal, $gMsg, $gFullPath1;
    $i = 0;

    while($i++ < $gTotal) {
        $info = date('d-M-Y H:i:s e') . ' [INFO] ' . $gMsg . PHP_EOL;
        file_put_contents($gFullPath1, $info, FILE_APPEND);
    }
}

function testFopenInstance()
{
    global $gTotal, $gMsg, $gFullPath2;
    $i = 0;

    $handler = fopen($gFullPath2, 'a');

    while($i++ < $gTotal) {
        $info = date('d-M-Y H:i:s e') . ' [INFO] ' . $gMsg . PHP_EOL;
        fwrite($handler, $info);
    }

    fclose($handler);
}

function testFopen()
{
    global $gTotal, $gMsg, $gFullPath3;
    $i = 0;

    while($i++ < $gTotal) {
        $handler = fopen($gFullPath3, 'a');
        $info = date('d-M-Y H:i:s e') . ' [INFO] ' . $gMsg . PHP_EOL;
        fwrite($handler, $info);
        fclose($handler);
    }
}

function testSyslog()
{
    global $gTotal, $gMsg;
    $i = 0;

    while($i++ < $gTotal) {
        $info = date('d-M-Y H:i:s e') . ' [INFO] ' . $gMsg . PHP_EOL;
        syslog(LOG_INFO, $info);
    }
}

function testErrorLog()
{
    global $gTotal, $gMsg, $gFullPath4;
    $i = 0;

    while($i++ < $gTotal) {
        $info = date('d-M-Y H:i:s e') . ' [INFO] ' . $gMsg . PHP_EOL;
        error_log($info, 3, $gFullPath4);
    }
}

function startTime()
{
    global $gCounter;
    $gCounter = microtime(true);   
}

function resultTime($name)
{
    global $gCounter, $gConsume, $gRepeat;

    $endTime = microtime(true);
    $counter = number_format(($endTime - $gCounter), 3);
    $pad = str_repeat(' ', $gRepeat - strlen($name) - strlen($counter));

    $gConsume += $counter;

    echo $name, $pad, $counter, PHP_EOL;
}

function cleanLogFile()
{/*{{{*/
    global $gFullPath0, $gFullPath1, $gFullPath2, $gFullPath3, $gFullPath4;

    if (file_exists($gFullPath0)) {
        unlink($gFullPath0);
    }
    if (file_exists($gFullPath1)) {
        unlink($gFullPath1);
    }
    if (file_exists($gFullPath2)) {
        unlink($gFullPath2);
    }
    if (file_exists($gFullPath3)) {
        unlink($gFullPath3);
    }
    if (file_exists($gFullPath4)) {
        unlink($gFullPath4);
    }
}/*}}}*/

function total()
{
    global $gConsume, $gRepeat;

    echo str_repeat('-', $gRepeat), PHP_EOL;
    $pad = str_repeat(' ', $gRepeat - strlen('Total') - strlen($gConsume));
    echo 'Total', $pad, $gConsume, PHP_EOL;
}

//-------------------------------------Asf\Logger
startTime();
testAsfLogger();
resultTime('testAsfLogger');
//-------------------------------------file_put_contents
startTime();
testFilePutContents();
resultTime('testFilePutContents');
//-------------------------------------fopen instance
startTime();
testFopenInstance();
resultTime('testFopenInstance');
//-------------------------------------fopen
startTime();
testFopen();
resultTime('testFopen');
//-------------------------------------syslog
startTime();
testSyslog();
resultTime('testSyslog');
//-------------------------------------error_log
startTime();
testErrorLog();
resultTime('testErrorLog');
//-------------------------------------clean/counter
cleanLogFile();
total();

