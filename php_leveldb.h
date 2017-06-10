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
  | Date: 2017/06/10													 |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_LEVELDB_H
#define PHP_LEVELDB_H

#include<leveldb/db.h>
#include<string>

using namespace std;
using namespace leveldb;

static leveldb::DB *db = NULL;
static zend_class_entry  *leveldb_entry  = NULL;
static zend_class_entry  *iterator_entry = NULL;

extern zend_module_entry leveldb_module_entry;
#define phpext_leveldb_ptr &leveldb_module_entry

#define PHP_LEVELDB_VERSION "0.1.3" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_LEVELDB_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_LEVELDB_API __attribute__ ((visibility("default")))
#else
#	define PHP_LEVELDB_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

Options		 leveldb_options(zval *_options);
ReadOptions  leveldb_read_options(zval *_options);
WriteOptions leveldb_write_options(zval *_options);

PHP_METHOD(LevelDB, __construct);
PHP_METHOD(LevelDB, get);
PHP_METHOD(LevelDB, put);
PHP_METHOD(LevelDB, delete);
PHP_METHOD(LevelDB, __destruct);

PHP_METHOD(LevelDBIterator, __construct);
PHP_METHOD(LevelDBIterator, key);
PHP_METHOD(LevelDBIterator, prev);
PHP_METHOD(LevelDBIterator, next);
PHP_METHOD(LevelDBIterator, valid);
PHP_METHOD(LevelDBIterator, rewind);
PHP_METHOD(LevelDBIterator, last);
PHP_METHOD(LevelDBIterator, current);
PHP_METHOD(LevelDBIterator, status);
PHP_METHOD(LevelDBIterator, __destruct);

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(leveldb)
	zend_long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(leveldb)
*/

/* Always refer to the globals in your function as LEVELDB_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define LEVELDB_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(leveldb, v)

#if defined(ZTS) && defined(COMPILE_DL_LEVELDB)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_LEVELDB_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
