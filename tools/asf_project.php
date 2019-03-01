<?php
define('APP_PATH', __DIR__);

$template_project = APP_PATH . '/templates';

$pname = isset($_SERVER['argv'][1]) ? trim($_SERVER['argv'][1]) : '';
if (empty($pname)) {
    $pname = APP_PATH . '/example'; 
}

$pdir = realpath(dirname($pname));
if (empty($pdir)) {
    $pname = APP_PATH . '/example'; 
}

if (is_dir($pdir)) {
    $create_project   = $pname;
} else {
    $create_project   = APP_PATH . '/' . $pname;
}

if (is_dir($create_project)) {
    echo PHP_EOL, "{$create_project} => Already Exists, Create Failed ...", PHP_EOL, PHP_EOL;
    exit(0);
}

system("cp -r {$template_project} {$create_project}", $ret);
if ($ret == 0) {
    echo PHP_EOL, "{$pname} => Project Create Success", PHP_EOL, PHP_EOL;
} else {
    echo PHP_EOL, "{$pname} => Create Failed ...", PHP_EOL, PHP_EOL;
}

