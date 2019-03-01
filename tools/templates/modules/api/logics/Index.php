<?php
use Asf\Loader;

class IndexLogic
{
    public function getStart()
    {
        return Loader::get('IndexDao')->getStart();
    }
}

