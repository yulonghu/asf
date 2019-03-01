<?php
class Bootstrap
{
    /* Basic module */
    public function _initAlarm()
    {/*{{{*/
        $app = Asf\Application::getInstance();
        $app->setErrorHandler(array($this, 'error'));
        $app->setTimeoutHandler(array($this, 'timeout'));   
    }/*}}}*/

    public function error($errno, $errstr, $errfile, $errline)
    {/*{{{*/
        //var_dump($errno, $errstr, $errfile, $errline);
    }/*}}}*/

    public function timeout($errno, $errstr)
    {/*{{{*/
        //var_dump($errno, $errstr);
    }/*}}}*/
}

