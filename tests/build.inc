<?php
define("PROJECT_PATH",      ROOT_PATH    . '/project');
define("MODULE_PATH",       PROJECT_PATH . '/modules');
define("LIBRARY_PATH",      PROJECT_PATH . '/library');
define("LOG_PATH",          PROJECT_PATH . '/logs');
define("APP_PATH",          MODULE_PATH  . '/api');
define("APP_PATH_ADMIN",    MODULE_PATH  . '/admin');
define("CONFIG_PATH",       PROJECT_PATH . '/config');
define("BOOTSTRAP_PATH",    MODULE_PATH  . '/');
define("CONSTANTS_PATH",    MODULE_PATH  . '/');

function shutdown($root = PROJECT_PATH)
{/*{{{*/
    $dp = opendir($root);
    while (($dir = readdir($dp))) {
        if (in_array($dir, array('.', '..'))) {
            continue;
        }
        $path = $root . DIRECTORY_SEPARATOR . $dir;

        if (is_dir($path)) {
            shutdown($path);
        } else if (is_file($path)) {
            unlink($path);
        }
    }
    rmdir($root);
}/*}}}*/

function start()
{/*{{{*/
    startRoot();
    startModules();
    startApps();
}/*}}}*/

function _mkdir($dirs, $path)
{/*{{{*/
    foreach ($dirs as $dir) {
        $dir = $path . DIRECTORY_SEPARATOR . $dir;
        if (!file_exists($dir)) {
            mkdir($dir, 0755, true);
            chmod($dir, 0755);
        }
    }  
}/*}}}*/

function startModules()
{/*{{{*/
    $dirs = array();

    $dirs[] = 'constants';
    $dirs[] = 'bootstrap';

    _mkdir($dirs, MODULE_PATH);
}/*}}}*/

function startApps()
{/*{{{*/
    $dirs = array();

    $dirs[] = 'services';
    $dirs[] = 'logics';
    $dirs[] = 'daos';
    $dirs[] = 'views';

    _mkdir($dirs, APP_PATH);
    _mkdir($dirs, APP_PATH_ADMIN);
}/*}}}*/

function startRoot()
{/*{{{*/
    $dirs = array();

    $dirs[] = 'config';
    $dirs[] = 'public';
    $dirs[] = 'library';
    $dirs[] = 'logs';

    _mkdir($dirs, PROJECT_PATH); 
}/*}}}*/

