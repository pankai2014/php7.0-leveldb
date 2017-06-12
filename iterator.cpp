/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: will<pan.kai@icloud.com>                                     |
   | Date: 2017/06/11													  |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_leveldb.h"

void iterator_destroy_handler(zend_resource *rsrc TSRMLS_DC) 
{
	leveldb::Iterator *it = (leveldb::Iterator *) rsrc->ptr;

	delete it; 
}

PHP_METHOD(LevelDBIterator, __construct)
{
	zval *object = NULL;
	zval iterator;

	int iterator_number;

	if ( zend_parse_parameters(ZEND_NUM_ARGS(), "o!", &object) == FAILURE ) {
		 RETURN_NULL();
	}
	
	zval *rv;   
	zval *leveldb      = zend_read_property(leveldb_entry, object, "leveldb",      sizeof("leveldb") - 1,      0, rv); 
	zval *leveldb_type = zend_read_property(leveldb_entry, object, "leveldb_type", sizeof("leveldb_type") - 1, 0, rv);
	zval *read_options = zend_read_property(leveldb_entry, object, "read_options", sizeof("read_options") - 1, 0, rv);

	leveldb::DB *db = (leveldb::DB *)zend_fetch_resource(Z_RES_P(leveldb), LEVELDB_TYPE, zval_get_long(leveldb_type)); 
	if ( db == NULL ) { 
		php_error_docref(NULL, E_WARNING, "getting leveldb resource faild"); 

		RETURN_FALSE; 
	}
	
	int iterator_type = zend_register_list_destructors_ex(iterator_destroy_handler, NULL, LEVELDB_TYPE, iterator_number);
	zend_update_property_long(iterator_entry, getThis(), "iterator_type", sizeof("iterator_type") - 1, iterator_type);

	ReadOptions options = leveldb_read_options(read_options);
	leveldb::Iterator *it = db->NewIterator(options);

	zend_resource *_iterator = zend_register_resource(it, iterator_type);
	ZVAL_RES(&iterator, _iterator);

	zend_update_property(iterator_entry, getThis(), "iterator",  sizeof("iterator") - 1, &iterator);
}

PHP_METHOD(LevelDBIterator, key) 
{
	leveldb_fetch_iterator_ptr(getThis(), iterator_type, iterator);

	Slice slice = it->key();
	string val  = slice.ToString();

	const char *ret = val.data();

	RETURN_STRING(ret);
}

PHP_METHOD(LevelDBIterator, prev) 
{
	leveldb_fetch_iterator_ptr(getThis(), iterator_type, iterator);

	it->Prev();
}

PHP_METHOD(LevelDBIterator, next) 
{
	leveldb_fetch_iterator_ptr(getThis(), iterator_type, iterator);

	it->Next();
}

PHP_METHOD(LevelDBIterator, valid) 
{
	leveldb_fetch_iterator_ptr(getThis(), iterator_type, iterator);

	bool status = it->Valid();
	if ( status ) {
		RETURN_TRUE;
	}
}

PHP_METHOD(LevelDBIterator, rewind) 
{
	leveldb_fetch_iterator_ptr(getThis(), iterator_type, iterator);

	it->SeekToFirst();
}

PHP_METHOD(LevelDBIterator, last) 
{
	leveldb_fetch_iterator_ptr(getThis(), iterator_type, iterator);

	it->SeekToLast();
}

PHP_METHOD(LevelDBIterator, current) 
{
	leveldb_fetch_iterator_ptr(getThis(), iterator_type, iterator);

	Slice slice = it->value();
	string val  = slice.ToString();
	    
	const char *ret = val.data();

	RETURN_STRING(ret);
}

PHP_METHOD(LevelDBIterator, status) 
{
	leveldb_fetch_iterator_ptr(getThis(), iterator_type, iterator);

	Status _status = it->status();
	if ( _status.ok() ) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}

PHP_METHOD(LevelDBIterator, __destruct)
{

}
