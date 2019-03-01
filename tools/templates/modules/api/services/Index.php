<?php
use Asf\Loader;

class IndexService
{
    public function indexAction()
    {/*{{{*/
        return Loader::get('IndexLogic')->getStart();
    }/*}}}*/
}

