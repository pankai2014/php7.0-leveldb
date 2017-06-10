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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_leveldb.h"

/* If you declare any globals in php_leveldb.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(leveldb)
*/
leveldb::Iterator *it;

/* True global resources - no need for thread safety here */

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("leveldb.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_leveldb_globals, leveldb_globals)
    STD_PHP_INI_ENTRY("leveldb.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_leveldb_globals, leveldb_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_leveldb_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_leveldb_compiled)
{
	char *arg = NULL;
	size_t arg_len, len;
	zend_string *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	strg = strpprintf(0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "leveldb", arg);

	RETURN_STR(strg);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_leveldb_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_leveldb_init_globals(zend_leveldb_globals *leveldb_globals)
{
	leveldb_globals->global_value = 0;
	leveldb_globals->global_string = NULL;
}
*/
/* }}} */
ZEND_BEGIN_ARG_INFO_EX(arginfo_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, _options)
	ZEND_ARG_INFO(0, _read_options)
	ZEND_ARG_INFO(0, _write_options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_put, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, val)
	ZEND_ARG_INFO(0, _write_options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, _read_options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_delete, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, _write_options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_iterator_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, leveldb)
ZEND_END_ARG_INFO()

/* {{{ leveldb_functions[]
 *
 * Every user visible function must have an entry in leveldb_functions[].
 */
const zend_function_entry leveldb_functions[] = {
	PHP_FE(confirm_leveldb_compiled, NULL)		/* For testing, remove later. */
	PHP_ME(LevelDB, __construct, arginfo_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(LevelDB, put, arginfo_put, ZEND_ACC_PUBLIC)
	PHP_ME(LevelDB, get, arginfo_get, ZEND_ACC_PUBLIC)
	PHP_ME(LevelDB, delete, arginfo_delete, ZEND_ACC_PUBLIC)
	PHP_ME(LevelDB, __destruct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
	PHP_FE_END	/* Must be the last line in leveldb_functions[] */
};

const zend_function_entry leveldb_iterator_functions[] = {
	PHP_ME(LevelDBIterator, __construct, arginfo_iterator_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(LevelDBIterator, key, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(LevelDBIterator, prev, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(LevelDBIterator, next, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(LevelDBIterator, valid, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(LevelDBIterator, rewind, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(LevelDBIterator, last, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(LevelDBIterator, current, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(LevelDBIterator, status, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(LevelDBIterator, __destruct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
	PHP_FE_END	/* Must be the last line in leveldb_functions[] */
};
/* }}} */

Options leveldb_options(zval *_options) 
{
	Options options;

	zval *value;
	zend_ulong num_idx;
	zend_string *str_idx;

	if ( _options != NULL ) {
		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(_options), num_idx, str_idx, value) {
			if ( str_idx ) {
				switch ( Z_TYPE_P(value) ) {
					case IS_STRING:
					case IS_TRUE:
						if ( zend_string_equals_literal(str_idx, "error_if_exists") ) {
							options.error_if_exists = true;
						}

						if ( zend_string_equals_literal(str_idx, "create_if_missing") ) {
							options.create_if_missing = true;
						}

						break;
					case IS_FALSE:
					case IS_LONG:
						if ( zend_string_equals_literal(str_idx, "block_size") ) {
							options.write_buffer_size = zval_get_long(value);
						}

						if ( zend_string_equals_literal(str_idx, "max_open_files") ) {
							options.max_open_files = (int)zval_get_long(value);
						}

						if ( zend_string_equals_literal(str_idx, "write_buffer_size") ) {
							options.write_buffer_size = zval_get_long(value);
						}

						if ( zend_string_equals_literal(str_idx, "block_restart_interval") ) {
							options.block_restart_interval = (int)zval_get_long(value);
						}

						break;
					default:
						php_error_docref(NULL, E_WARNING, "Option(%s) value must be string, boolean or long", ZSTR_VAL(str_idx));

						break;
				}
			}

			(void) num_idx;
		} ZEND_HASH_FOREACH_END();
	}

	return options;
}

ReadOptions leveldb_read_options(zval *_options) 
{
	ReadOptions options;

	zval *value;
	zend_ulong num_idx;
	zend_string *str_idx;

	if ( _options != NULL ) {
		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(_options), num_idx, str_idx, value) {
			if ( str_idx ) {
				switch ( Z_TYPE_P(value) ) {
					case IS_STRING:
					case IS_TRUE:
						if ( zend_string_equals_literal(str_idx, "verify_checksums") ) {
							options.verify_checksums = true;
						}

						break;
					case IS_FALSE:
						if ( zend_string_equals_literal(str_idx, "fill_cache") ) {
							options.fill_cache = false;
						}

						break;
					case IS_LONG:
					default:
						php_error_docref(NULL, E_WARNING, "Option(%s) value must be string, boolean or long", ZSTR_VAL(str_idx));

						break;
				}
			}

			(void) num_idx;
		} ZEND_HASH_FOREACH_END();
	}

	return options;
}

WriteOptions leveldb_write_options(zval *_options) 
{
	WriteOptions options;

	zval *value;
	zend_ulong num_idx;
	zend_string *str_idx;

	if ( _options != NULL ) {
		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(_options), num_idx, str_idx, value) {
			if ( str_idx ) {
				switch ( Z_TYPE_P(value) ) {
					case IS_STRING:
					case IS_TRUE:
						if ( zend_string_equals_literal(str_idx, "sync") ) {
							options.sync = true;
						}

						break;
					case IS_FALSE:
					case IS_LONG:
					default:
						php_error_docref(NULL, E_WARNING, "Option(%s) value must be string, boolean or long", ZSTR_VAL(str_idx));

						break;
				}
			}

			(void) num_idx;
		} ZEND_HASH_FOREACH_END();
	}

	return options;
}

PHP_METHOD(LevelDB, __construct) 
{
	char *name = NULL;
	size_t name_len;
	
	zval *_options	     = NULL;
	zval *_read_options  = NULL;
	zval *_write_options = NULL;

	if ( db != NULL ) {
		php_error_docref(NULL, E_WARNING, "the leveldb is not closed");

		RETURN_NULL();
	}

	if ( zend_parse_parameters(ZEND_NUM_ARGS(), "s|a!a!a!", &name, &name_len, &_options, &_read_options, &_write_options) == FAILURE ) {
		RETURN_NULL();
	}

	Options options = leveldb_options(_options);

	Status status = DB::Open(options, name, &db);
	if ( ! status.ok() ) {
		RETURN_NULL();
	}

	ReadOptions read_options = leveldb_read_options(_read_options);
	it = db->NewIterator(leveldb::ReadOptions());
}

PHP_METHOD(LevelDB, get) 
{
	char *key;
	string val;
	size_t key_len;

	zval *_read_options = NULL;

	if ( zend_parse_parameters(ZEND_NUM_ARGS(), "s|a!", &key, &key_len, &_read_options) == FAILURE ) {
		RETURN_FALSE;
	}

	ReadOptions options = leveldb_read_options(_read_options);

	Status status = db->Get(options, key, &val);
	if ( ! status.ok() ) {
		RETURN_FALSE;
	}

	const char *ret = val.data();

	RETURN_STRING(ret);
}

PHP_METHOD(LevelDB, put) 
{
	char *key, *val;
	size_t key_len, val_len;

	zval *_write_options = NULL;

	if ( zend_parse_parameters(ZEND_NUM_ARGS(), "ss|a!", &key, &key_len, &val, &val_len, &_write_options) == FAILURE ) {
		RETURN_FALSE;
	}

	if ( db == NULL ) {
		php_error_docref(NULL, E_WARNING, "the leveldb has closed");

		RETURN_FALSE;
	}
	
	WriteOptions options = leveldb_write_options(_write_options);
	
	Status status = db->Put(options, key, val);
	if ( ! status.ok() ) { 
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

PHP_METHOD(LevelDB, delete) 
{
	char *key;
	size_t key_len;

	zval *_options = NULL;

	if ( zend_parse_parameters(ZEND_NUM_ARGS(), "s|a!", &key, &key_len, &_options) == FAILURE ) {
		RETURN_FALSE;
	}

	WriteOptions options = leveldb_write_options(_options);

	Status status = db->Delete(options, key);
	if ( ! status.ok() ) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

PHP_METHOD(LevelDB, __destruct) 
{
	if ( db != NULL ) delete db;
	if ( leveldb_entry  != NULL ) free(leveldb_entry);
	if ( iterator_entry != NULL ) free(iterator_entry);
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(leveldb)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/
	zend_class_entry _leveldb_entry;
	zend_class_entry _iterator_entry;

	INIT_CLASS_ENTRY(_leveldb_entry,  "LevelDB",		 leveldb_functions);
	INIT_CLASS_ENTRY(_iterator_entry, "LevelDBIterator", leveldb_iterator_functions);

	leveldb_entry  = zend_register_internal_class(&_leveldb_entry);
	iterator_entry = zend_register_internal_class(&_iterator_entry);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(leveldb)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(leveldb)
{
#if defined(COMPILE_DL_LEVELDB) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(leveldb)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(leveldb)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "leveldb support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ leveldb_module_entry
 */
zend_module_entry leveldb_module_entry = {
	STANDARD_MODULE_HEADER,
	"leveldb",
	leveldb_functions,
	PHP_MINIT(leveldb),
	PHP_MSHUTDOWN(leveldb),
	PHP_RINIT(leveldb),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(leveldb),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(leveldb),
	PHP_LEVELDB_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_LEVELDB
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(leveldb)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
