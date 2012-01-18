/*
 * The MIT License
 *
 * Copyright (c) 2010 - 2012 Shuhei Tanuma
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "php_git2.h"

PHPAPI zend_class_entry *git2_repository_class_entry;
void php_git2_repository_init(TSRMLS_D);

static void php_git2_repository_free_storage(php_git2_repository *object TSRMLS_DC)
{
	if (object->repository != NULL) {
		git_repository_free(object->repository);
		object->repository = NULL;
	}
	zend_object_std_dtor(&object->zo TSRMLS_CC);
	efree(object);
}

zend_object_value php_git2_repository_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;

	PHP_GIT2_STD_CREATE_OBJECT(php_git2_repository);
	return retval;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_git2_repository___construct, 0,0,1)
	ZEND_ARG_INFO(0, repository_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_git2_repository_init, 0,0,1)
	ZEND_ARG_INFO(0, isBare)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_git2_repository_discover, 0,0,3)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, across_fs)
	ZEND_ARG_INFO(0, ceiling_dirs)
ZEND_END_ARG_INFO()

/*
{{{ proto: Git2\Repsotiroy::__construct(string $path)
*/
PHP_METHOD(git2_repository, __construct)
{
	const char *repository_path = NULL;
	int repository_path_len, ret = 0;
	git_repository *repository;
	php_git2_repository *m_repository;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"|s", &repository_path, &repository_path_len) == FAILURE) {
		return;
	}

	m_repository = PHP_GIT2_GET_OBJECT(php_git2_repository, getThis());
	if (repository_path_len > 0) {
		ret = git_repository_open(&repository, repository_path);
		
		if (ret != GIT_SUCCESS) {
			zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, git_strerror(ret));
			RETURN_FALSE;
		}
		
		m_repository->repository = repository;
		php_git_add_protected_property_string_ex(getThis(),
			"path",sizeof("path"),
			git_repository_path(repository),
		1 TSRMLS_CC);
	} else {
		m_repository->repository = NULL;
	}
}
/* }}} */

/*
{{{ proto: Git2\Repsotiroy::isEmpty()
*/
PHP_METHOD(git2_repository, isEmpty)
{
	git_repository *repository;
	php_git2_repository *m_repository;

	m_repository = PHP_GIT2_GET_OBJECT(php_git2_repository, getThis());
	if (m_repository->repository != NULL) {
		if (git_repository_is_empty(m_repository->repository)) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
	} else {
		/* @todo: throws an exectpion */
	}
}
/* }}} */

/*
{{{ proto: Git2\Repsotiroy::isBare()
*/
PHP_METHOD(git2_repository, isBare)
{
	git_repository *repository;
	php_git2_repository *m_repository;

	m_repository = PHP_GIT2_GET_OBJECT(php_git2_repository, getThis());
	if (m_repository->repository != NULL) {
		if (git_repository_is_bare(m_repository->repository) == 1) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
	} else {
		/* @todo: throws an exectpion */
	}
}
/* }}} */

/*
{{{ proto: Git2\Repsotiroy::getPath()
*/
PHP_METHOD(git2_repository, getPath)
{
	git_repository *repository;
	php_git2_repository *m_repository;
	const char *path = NULL;
	zval *m_path = NULL;

	m_repository = PHP_GIT2_GET_OBJECT(php_git2_repository, getThis());
	if (m_repository->repository != NULL) {
		path = git_repository_path(m_repository->repository);
		if (path != NULL) {
			MAKE_STD_ZVAL(m_path);
			ZVAL_STRING(m_path, path, 1);
			RETVAL_ZVAL(m_path,0,1);
		}
	} else {
		/* @todo: throws an exectpion */
	}
}
/* }}} */

/*
{{{ proto: Git2\Repsotiroy::getWorkdir()
*/
PHP_METHOD(git2_repository, getWorkdir)
{
	git_repository *repository;
	php_git2_repository *m_repository;
	const char *path = NULL;
	zval *m_path = NULL;

	m_repository = PHP_GIT2_GET_OBJECT(php_git2_repository, getThis());
	if (m_repository->repository != NULL) {
		path = git_repository_workdir(m_repository->repository);
		if (path != NULL) {
			MAKE_STD_ZVAL(m_path);
			ZVAL_STRING(m_path, path, 1);
			RETVAL_ZVAL(m_path,0,1);
		}
	} else {
		/* @todo: throws an exectpion */
	}
}
/* }}} */


/*
{{{ proto: Git2\Repsotiroy::init(string $path [, bool isBare])
*/
PHP_METHOD(git2_repository, init)
{
	char *path;
	int ret, path_len = 0;
	zend_bool is_bare = 0;
	git_repository *repository;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"s|b", &path, &path_len, &is_bare) == FAILURE) {
		return;
	}
	
	ret = git_repository_init(&repository, path, is_bare);
	if (ret == GIT_SUCCESS) {
		zval *object;
		php_git2_repository *m_repository;
		
		MAKE_STD_ZVAL(object);
		object_init_ex(object, git2_repository_class_entry);
		m_repository = PHP_GIT2_GET_OBJECT(php_git2_repository, object);
		m_repository->repository = repository;
		
		RETVAL_ZVAL(object,0,1);
	} else {
		/* @todo: throws an runtime exception */
		RETURN_FALSE;
	}
}
/* }}} */

/*
{{{ proto: Git2\Repsotiroy::headDetached()
	A repository's HEAD is detached when it points directly to a commit instead of a branch.
*/
PHP_METHOD(git2_repository, headDetached)
{
	git_repository *repository;
	php_git2_repository *m_repository;

	m_repository = PHP_GIT2_GET_OBJECT(php_git2_repository, getThis());
	if (m_repository->repository != NULL) {
		if (git_repository_head_detached(m_repository->repository) == 1) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
	} else {
		/* @todo: throws an exectpion */
	}
}
/* }}} */

/*
{{{ proto: Git2\Repsotiroy::headOrphan()
 An orphan branch is one named from HEAD but which doesn't exist in	
 the refs namespace, because it doesn't have any commit to point to.
*/                                                                                           
PHP_METHOD(git2_repository, headOrphan)
{
	git_repository *repository;
	php_git2_repository *m_repository;

	m_repository = PHP_GIT2_GET_OBJECT(php_git2_repository, getThis());
	if (m_repository->repository != NULL) {
		if (git_repository_head_orphan(m_repository->repository) == 1) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
	} else {
		/* @todo: throws an exectpion */
	}
}
/* }}} */

/*
{{{ proto: Git2\Repsotiroy::discover(string $path[, bool across_fs, string ceiling_dirs])
*/
PHP_METHOD(git2_repository, discover)
{
	char path_buffer[1024] = {0};
	size_t path_size = 1024;
	zend_bool across_fs = 1;
	char *start_path, *ceiling_dirs = NULL;
	int start_path_len, ceiling_dirs_len = 0;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
		"s|bs", &start_path, &start_path_len, &across_fs, &ceiling_dirs, & ceiling_dirs_len) == FAILURE) {
		return;
	}
	
	if (git_repository_discover(&path_buffer,path_size,start_path,(int)across_fs, ceiling_dirs) == GIT_SUCCESS) {
		RETVAL_STRING(path_buffer, 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



static zend_function_entry php_git2_repository_methods[] = {
	PHP_ME(git2_repository, __construct, arginfo_git2_repository___construct, ZEND_ACC_PUBLIC)
	PHP_ME(git2_repository, isEmpty,     NULL,                                ZEND_ACC_PUBLIC)
	PHP_ME(git2_repository, isBare,      NULL,                                ZEND_ACC_PUBLIC)
	PHP_ME(git2_repository, getPath,     NULL,                                ZEND_ACC_PUBLIC)
	PHP_ME(git2_repository, getWorkdir,  NULL,                                ZEND_ACC_PUBLIC)
	PHP_ME(git2_repository, headDetached,NULL,                                ZEND_ACC_PUBLIC)
	PHP_ME(git2_repository, headOrphan,  NULL,                                ZEND_ACC_PUBLIC)
	PHP_ME(git2_repository, init,        arginfo_git2_repository_init,        ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(git2_repository, discover,    arginfo_git2_repository_discover,    ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	{NULL,NULL,NULL}
};

void php_git2_repository_init(TSRMLS_D)
{
	zend_class_entry ce;
	
	INIT_NS_CLASS_ENTRY(ce, PHP_GIT2_NS, "Repository", php_git2_repository_methods);
	git2_repository_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	git2_repository_class_entry->create_object = php_git2_repository_new;
}