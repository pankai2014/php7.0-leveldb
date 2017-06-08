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

/* True global resources - no need for thread safety here */
//static int le_leveldb;
static leveldb::DB *db = NULL;

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

PHP_FUNCTION(leveldb_open) 
{
	char *name = NULL;
	size_t name_len;
	
	if ( db != NULL ) {
		php_error_docref(NULL, E_WARNING, "the leveldb is not closed");

		RETURN_FALSE;
	}

	if ( zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE ) {
		RETURN_FALSE;
	}

	Options options;
	options.create_if_missing = true;

	Status status = DB::Open(options, name, &db);
	if ( ! status.ok() ) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

PHP_FUNCTION(leveldb_put) 
{
	char *key, *val;
	size_t key_len, val_len;

	if ( zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &key, &key_len, &val, &val_len ) == FAILURE ) {
		RETURN_FALSE;
	}

	if ( db == NULL ) {
		php_error_docref(NULL, E_WARNING, "the leveldb has closed");

		RETURN_FALSE;
	}

	Status status = db->Put(WriteOptions(), key, val);
	if ( ! status.ok() ) { 
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}

PHP_FUNCTION(leveldb_get) 
{
	char *key;
	string val;
	size_t key_len;

	if ( zend_parse_parameters(ZEND_NUM_ARGS(), "s", &key, &key_len) == FAILURE ) {
		RETURN_FALSE;
	}
	
	Status status = db->Get(ReadOptions(), key, &val);
	if ( ! status.ok() ) {
		RETURN_FALSE;
	}
	
	const char *ret = val.data();

	RETURN_STRING(ret);
}

PHP_FUNCTION(leveldb_close) 
{
	if ( db != NULL ) delete db;

	RETURN_TRUE;
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

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(leveldb)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/
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
	if ( db != NULL ) delete db;

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

/* {{{ leveldb_functions[]
 *
 * Every user visible function must have an entry in leveldb_functions[].
 */
const zend_function_entry leveldb_functions[] = {
	PHP_FE(confirm_leveldb_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(leveldb_open, NULL)			
	PHP_FE(leveldb_put, NULL)			
	PHP_FE(leveldb_get, NULL)			
	PHP_FE(leveldb_close, NULL)			
	PHP_FE_END	/* Must be the last line in leveldb_functions[] */
};
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
