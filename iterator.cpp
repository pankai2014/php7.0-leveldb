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

extern leveldb::Iterator *it;

static bool _assert();

static bool _assert() 
{
	if ( it == NULL ) { 
		php_error_docref(NULL, E_WARNING, "the interator ptr is null");

		return true;
	}

	return false;
}

PHP_METHOD(LevelDBIterator, __construct)
{
	zval *leveldb = NULL;

	if ( zend_parse_parameters(ZEND_NUM_ARGS(), "o!", &leveldb) == FAILURE ) {
		 RETURN_NULL();
	}
}

PHP_METHOD(LevelDBIterator, key) 
{
	if ( _assert() ) RETURN_FALSE;

	Slice slice = it->key();
	string val  = slice.ToString();

	const char *ret = val.data();

	RETURN_STRING(ret);
}

PHP_METHOD(LevelDBIterator, prev) 
{
	if ( _assert() ) RETURN_FALSE;

	it->Prev();
}

PHP_METHOD(LevelDBIterator, next) 
{
	if ( _assert() ) RETURN_FALSE;

	it->Next();
}

PHP_METHOD(LevelDBIterator, valid) 
{
	if ( _assert() ) RETURN_FALSE;

	bool status = it->Valid();
	if ( status ) {
		RETURN_TRUE;
	}
}

PHP_METHOD(LevelDBIterator, rewind) 
{
	if ( _assert() ) RETURN_FALSE;

	it->SeekToFirst();
}

PHP_METHOD(LevelDBIterator, last) 
{
	if ( _assert() ) RETURN_FALSE;

	it->SeekToLast();
}

PHP_METHOD(LevelDBIterator, current) 
{
	if ( _assert() ) RETURN_FALSE;

	Slice slice = it->value();
	string val  = slice.ToString();
	    
	const char *ret = val.data();

	RETURN_STRING(ret);
}

PHP_METHOD(LevelDBIterator, status) 
{
	if ( _assert() ) RETURN_FALSE;

	Status _status = it->status();
	if ( _status.ok() ) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}

PHP_METHOD(LevelDBIterator, __destruct)
{
	if ( it != NULL ) delete it;
}
