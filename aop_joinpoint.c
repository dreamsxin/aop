/*
/+----------------------------------------------------------------------+
 | AOP                                                                  |
 +----------------------------------------------------------------------+
 | Copyright (c) 2012 Julien Salleyron, Gérald Croës                    |
 +----------------------------------------------------------------------+
 | This source file is subject to version 3.01 of the PHP license,      |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.php.net/license/3_01.txt.                                 |
 | If you did not receive a copy of the PHP license and are unable to   |
 | obtain it through the world-wide-web, please send a note to          |
 | license@php.net so we can mail you a copy immediately.               |
 +----------------------------------------------------------------------+
 | Author: Julien Salleyron <julien.salleyron@gmail.com>                |
 +----------------------------------------------------------------------+
*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/php_string.h"
#include "ext/pcre/php_pcre.h"
#include "aop.h"
#include "aop_joinpoint.h"
#include "Zend/zend_interfaces.h"

#define GET_THIS() (aop_joinpoint_object *)((char *)Z_OBJ_P(getThis()) - XtOffsetOf(aop_joinpoint_object, std));

PHP_METHOD(AopJoinpoint, getArguments){
    aop_joinpoint_object* this = GET_THIS();
    /*
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->args == NULL) {
        obj->args = get_current_args(obj->ex TSRMLS_CC);
    }
    if (obj->args != NULL) {
        RETURN_ZVAL(obj->args, 1, 0);
    }   
    RETURN_NULL();
    */
}

PHP_METHOD(AopJoinpoint, setArguments){
    aop_joinpoint_object* this = GET_THIS();
/*
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    zval *params;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &params) == FAILURE) {
        zend_error(E_ERROR, "setArguments expects an array as its first argument");
        return;
    }
    if (obj->args != NULL) {
        zval_ptr_dtor(&obj->args);
    }
    obj->args = params;
    obj->args_overloaded = 1;
    Z_ADDREF_P(params);
    RETURN_NULL();
    */
}

PHP_METHOD(AopJoinpoint, getKindOfAdvice){
    aop_joinpoint_object* this = GET_THIS();
    RETURN_LONG(this->current_pointcut->kind_of_advice);
}

PHP_METHOD(AopJoinpoint, getPointcut){
    aop_joinpoint_object* this = GET_THIS();
    /*
    AopJoinpoint_object *obj = (AopJoinpoint_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    RETURN_STRING(obj->current_pointcut->selector, 1);
    */

}

PHP_METHOD(AopJoinpoint, getReturnedValue){
    aop_joinpoint_object* this = GET_THIS();
    if (this->current_pointcut->kind_of_advice & AOP_KIND_BEFORE) {
        zend_error(E_ERROR, "getReturnedValue is not available when the advice was added with aop_add_before");
    }
    if (this->retval_ptr!=NULL) {
        RETURN_ZVAL(this->retval_ptr, 0, 1);
    }
}


PHP_METHOD(AopJoinpoint, setReturnedValue){
    aop_joinpoint_object* this = GET_THIS();
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", this->retval_ptr) == FAILURE) {
        zend_error(E_ERROR, "Error");
        return;
    }
}

PHP_METHOD(AopJoinpoint, getObject) {
    aop_joinpoint_object* this = GET_THIS();
    RETURN_ZVAL(&this->execute_data->This, 1, 0);
}

PHP_METHOD(AopJoinpoint, getClassName){
    aop_joinpoint_object* this = GET_THIS();

    zend_class_entry *ce = this->std.ce;
    if (ce != NULL) {
        RETURN_STR(zend_string_dup(ce->name, 0));
    }
    RETURN_NULL();
}

PHP_METHOD(AopJoinpoint, getFunctionName){
    aop_joinpoint_object* this = GET_THIS();
    if (this->current_pointcut->kind_of_advice & AOP_KIND_METHOD) {
        zend_error(E_ERROR, "getFunctionName is only available when the JoinPoint is a function call"); 
    }
    if (this->funcname!=NULL) {
        RETURN_STR(this->funcname);
    }
    RETURN_NULL();
}

PHP_METHOD(AopJoinpoint, getException){
    aop_joinpoint_object* this = GET_THIS();
    
    if (!(this->current_pointcut->kind_of_advice & AOP_KIND_CATCH)){
        zend_error(E_ERROR, "getException is only available when the advice was added with aop_add_after or aop_add_after_throwing"); 
    }
    if (Z_TYPE(this->exception) == IS_OBJECT) {
        RETURN_ZVAL(&this->exception, 1, 0);
    }
    RETURN_NULL();
}



PHP_METHOD(AopJoinpoint, getMethodName){
    aop_joinpoint_object* this = GET_THIS();
    if (this->current_pointcut->kind_of_advice & AOP_KIND_FUNCTION) {
        zend_error(E_ERROR, "getMethodName is only available when the JoinPoint is a method call"); 
    }
    if (this->funcname!=NULL) {
        RETURN_STR(this->funcname);
    }
    RETURN_NULL();
}

PHP_METHOD(AopJoinpoint, process){

    aop_joinpoint_object* this = GET_THIS();
    if (!(this->current_pointcut->kind_of_advice & AOP_KIND_AROUND)) {
        zend_error(E_ERROR, "process is only available when the advice was added with aop_add_around"); 
    }
    zval arg;
    aop_g(overloaded) = 1; 
    test_pointcut_and_execute(this->current_pointcut_zval_ptr+1, this->execute_data, this->original, this->retval_ptr TSRMLS_CC);
    aop_g(overloaded) = 0; 
    if (this->retval_ptr!=NULL) {
        RETURN_ZVAL(this->retval_ptr, 0, 1);
    }
}
