/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_page.h"

/* If you declare any globals in php_page.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(page)
*/

/* True global resources - no need for thread safety here */
static int le_page;

zend_class_entry *Page_ce;

/* {{{ page_functions[]
 *
 * Every user visible function must have an entry in page_functions[].
 */
const zend_function_entry PageMethod[] = {
	//PHP_FE(confirm_page_compiled,	NULL)		/* For testing, remove later. */
	ZEND_ME(Page, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	ZEND_ME(Page, __destruct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
	ZEND_ME(Page, startUp, NULL, ZEND_ACC_PUBLIC)
	ZEND_ME(Page, getPagesInfo, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END	/* Must be the last line in page_functions[] */
};
/* }}} */

/* {{{ page_module_entry
 */
zend_module_entry page_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"page",
	PageMethod,
	PHP_MINIT(page),
	PHP_MSHUTDOWN(page),
	PHP_RINIT(page),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(page),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(page),
#if ZEND_MODULE_API_NO >= 20010901
	"1.0.0",
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PAGE
ZEND_GET_MODULE(page)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("page.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_page_globals, page_globals)
    STD_PHP_INI_ENTRY("page.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_page_globals, page_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_page_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_page_init_globals(zend_page_globals *page_globals)
{
	page_globals->global_value = 0;
	page_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(page)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Page", PageMethod);
	Page_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_null(Page_ce, "pageInfo", strlen("pageInfo"), ZEND_ACC_PRIVATE TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(page)
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
PHP_RINIT_FUNCTION(page)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(page)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(page)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "page support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

ZEND_METHOD(Page, __construct)
{
	//创建 成员属性 pageInfo
	zval *pageInfo, *groups;
	MAKE_STD_ZVAL(pageInfo);
	MAKE_STD_ZVAL(groups);
	array_init(pageInfo);
	array_init(groups);
	add_assoc_long(pageInfo, "prevPage", 0);
	add_assoc_long(pageInfo, "nextPage", 0);
	add_assoc_long(pageInfo, "realPage", 0);
	add_assoc_long(pageInfo, "groupPageNum", 0);
	add_assoc_long(pageInfo, "start", 0);
	add_assoc_long(pageInfo, "pageCount", 0);
	add_assoc_long(pageInfo, "totalPage", 0);
	add_assoc_long(pageInfo, "totalGroup", 0);
	add_assoc_zval(pageInfo, "groups", groups);
	zend_update_property(Page_ce, getThis(), "pageInfo", strlen("pageInfo"), pageInfo TSRMLS_CC);
}

ZEND_METHOD(Page, __destruct)
{
	zval *pageInfo, *groups;
	pageInfo = zend_read_property(Page_ce, getThis(), "pageInfo", strlen("pageInfo"), 0 TSRMLS_CC);
	zval_ptr_dtor(&pageInfo);
}

ZEND_METHOD(Page, startUp)
{
	long page, dataCount, pageCount = 5, groupPageNum = 5;
	unsigned int i = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll|ll", &page, &dataCount, &pageCount, &groupPageNum) == FAILURE) {
		RETURN_FALSE;
	}
	long totalPage = dataCount % pageCount == 0 ? dataCount / pageCount : dataCount / pageCount + 1; // 实现php ceil
	if (totalPage < page)
		RETURN_FALSE;
	if (1 > page)
		RETURN_FALSE;
	long totalGroup = totalPage % groupPageNum == 0 ? totalPage / groupPageNum : totalPage / groupPageNum + 1;
	long realGroupPageNum = totalPage > groupPageNum ? groupPageNum : totalPage; // 实际组页数
	long *pGroups = (long*)emalloc(groupPageNum * sizeof(long));
	zval *pageInfo;

	pageInfo = zend_read_property(Page_ce, getThis(), "pageInfo", strlen("pageInfo"), 0 TSRMLS_CC);
	// 对成员属性pageInfo 数组赋值操作
	add_assoc_long(pageInfo, "start", (page - 1) * pageCount);
	add_assoc_long(pageInfo, "realPage", page);
	add_assoc_long(pageInfo, "groupPageNum", groupPageNum);
	add_assoc_long(pageInfo, "pageCount", pageCount);
	add_assoc_long(pageInfo, "totalPage", totalPage);
	add_assoc_long(pageInfo, "totalGroup", totalGroup);
	add_assoc_long(pageInfo, "prevPage", page <= 1 ? 0 : page - 1);
	add_assoc_long(pageInfo, "nextPage", page >= totalPage ? 0 : page + 1);
	zval *groups;
	getGroup(groupPageNum, totalPage, page, totalGroup, &pGroups);
    MAKE_STD_ZVAL(groups);
    array_init(groups);
	for (i = 0; i < realGroupPageNum; ++i)
		add_index_long(groups, (ulong)i, *(pGroups + i));
    add_assoc_zval(pageInfo, "groups", groups);
	efree(pGroups);
	RETURN_TRUE;
}

void getGroup(long groupPageNum, long totalPage, long realPage, long totalGroup, long **ppGroups)
{
	long pageSite = groupPageNum / 2; // php floor
	int i, j, site, p, k, l, q;
	i = j = site = p = k = l = q = 0;
	pageSite = pageSite > totalPage ? totalPage : pageSite;
	if (realPage <= pageSite)
	{
		for (i = 0; i < pageSite; ++i)
			*(*ppGroups + i) = i + 1;
		for (; i < totalPage && i < groupPageNum; ++i)
			*(*ppGroups + i) = i + 1;
		for (; i < groupPageNum; ++i)
			*(*ppGroups + i) = 0;
	}
	else if (realPage >= totalPage - pageSite)
	{
		long realGroupPageNum = totalPage > groupPageNum ? groupPageNum : totalPage; // 实际组页数
		for (j = totalPage, site = realGroupPageNum - 1; j > totalPage - realGroupPageNum && site >= 0 && j >= 0; --j, --site)
			*(*ppGroups + site) = j;
	}
	else
	{
		for (p = groupPageNum % 2 == 0 ? pageSite - 1 : pageSite, k = 0; p >= 0; --p, ++k)
			*(*ppGroups + p) = realPage - k;
		for (q = groupPageNum % 2 == 0 ? pageSite : pageSite + 1, l = 1; q < groupPageNum; ++q, ++l)
			*(*ppGroups + q) = realPage + l;
	}
}

ZEND_METHOD(Page, getPagesInfo)
{
    zval *pageInfo;
    pageInfo = zend_read_property(Page_ce, getThis(), "pageInfo", strlen("pageInfo"), 0 TSRMLS_CC);
	RETURN_ZVAL(pageInfo, 1, 0);
    return;
}

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_page_compiled(string arg)
   Return a string to confirm that the module is compiled in */
/*PHP_FUNCTION(confirm_page_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "page", arg);
	RETURN_STRINGL(strg, len, 0);
}*/
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
