<?php
use Asf\Db;
use Asf\Config;

class IndexDao
{
    const STATUS_OK = 0;
    const STATUE_DEL = 1;

    public function init($type =  Constants::SLAVE)
    {/*{{{*/
        /*
        $configs = array(
            'dsn' => array('host' => '127.0.0.1', 'dbname' => 'asf', 'port' => 6666),
            'username' => 'test',
            'password' => 'AbcdefRDvgedf',
        );
         */
        // Db::init(Config::get('config')->toArray());
        // Db::setTable('test');
    }/*}}}*/

    public function add(array $data)
    {/*{{{*/
        $this->init(Constants::MASTER);
        return Db::insert($data, true);
    }/*}}}*/

    public function update(array $data, $id)
    {/*{{{*/
        $this->init(Constants::MASTER);
        return Db::update($data, ['id' => $id]);
    }/*}}}*/

    public function delete($id)
    {/*{{{*/
        $this->init(Constants::MASTER);
        return Db::update(['status' => self::STATUE_DEL], ['id' => $id]);
    }/*}}}*/

    public function find($id)
    {/*{{{*/
        $this->init();

        $sql = "SELECT * FROM ? WHERE id = ?";
        $value = [Db::getTable(), '36'];

        return $mysql->findOne($sql, $value);
    }/*}}}*/

    public function getStart()
    {/*{{{*/
        return Library::start();
    }/*}}}*/
}

