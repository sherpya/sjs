/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sw=4 et tw=78:
 *
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Communicator client code, released
 * March 31, 1998.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

/*
 * JavaScript iterators.
 */
#include "jsstddef.h"
#include <string.h>     /* for memcpy */
#include "jstypes.h"
#include "jsutil.h"
#include "jsarena.h"
#include "jsapi.h"
#include "jsarray.h"
#include "jsatom.h"
#include "jsbool.h"
#include "jscntxt.h"
#include "jsconfig.h"
#include "jsexn.h"
#include "jsfun.h"
#include "jsgc.h"
#include "jsinterp.h"
#include "jsiter.h"
#include "jslock.h"
#include "jsobj.h"
#include "jsopcode.h"
#include "jsscope.h"
#include "jsscript.h"

#if JS_HAS_XML_SUPPORT
#include "jsxml.h"
#endif

extern const char js_throw_str[]; /* from jsscan.h */

#define JSSLOT_ITER_STATE       (JSSLOT_PRIVATE)
#define JSSLOT_ITER_FLAGS       (JSSLOT_PRIVATE + 1)

#if JSSLOT_ITER_FLAGS >= JS_INITIAL_NSLOTS
#error JS_INITIAL_NSLOTS must be greater than JSSLOT_ITER_FLAGS.
#endif

/*
 * Shared code to close iterator's state either through an explicit call or
 * when GC detects that the iterator is no longer reachable.
 */
void
js_CloseIteratorState(JSContext *cx, JSObject *iterobj)
{
    jsval *slots;
    jsval state, parent;
    JSObject *iterable;

    JS_ASSERT(JS_InstanceOf(cx, iterobj, &js_IteratorClass, NULL));
    slots = iterobj->slots;

    /* Avoid double work if js_CloseNativeIterator was called on obj. */
    state = slots[JSSLOT_ITER_STATE];
    if (JSVAL_IS_NULL(state))
        return;

    /* Protect against failure to fully initialize obj. */
    parent = slots[JSSLOT_PARENT];
    if (!JSVAL_IS_PRIMITIVE(parent)) {
        iterable = JSVAL_TO_OBJECT(parent);
#if JS_HAS_XML_SUPPORT
        if ((JSVAL_TO_INT(slots[JSSLOT_ITER_FLAGS]) & JSITER_FOREACH) &&
            OBJECT_IS_XML(cx, iterable)) {
            ((JSXMLObjectOps *) iterable->map->ops)->
                enumerateValues(cx, iterable, JSENUMERATE_DESTROY, &state,
                                NULL, NULL);
        } else
#endif
            OBJ_ENUMERATE(cx, iterable, JSENUMERATE_DESTROY, &state, NULL);
    }
    slots[JSSLOT_ITER_STATE] = JSVAL_NULL;
}

JSClass js_IteratorClass = {
    "Iterator",
    JSCLASS_HAS_RESERVED_SLOTS(2) | /* slots for state and flags */
    JSCLASS_HAS_CACHED_PROTO(JSProto_Iterator),
    JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub,   JS_ConvertStub,   JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

#if JS_HAS_GENERATORS

static JSBool
Iterator(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    jsval fval;
    const jsid id = ATOM_TO_JSID(cx->runtime->atomState.iteratorAtom);

    /* XXX work around old valueOf call hidden beneath js_ValueToObject */
    if (!JSVAL_IS_PRIMITIVE(argv[0])) {
        obj = JSVAL_TO_OBJECT(argv[0]);
    } else {
        obj = js_ValueToNonNullObject(cx, argv[0]);
        if (!obj)
            return JS_FALSE;
    }

    return JS_GetMethodById(cx, obj, id, &obj, &fval) &&
           js_InternalCall(cx, obj, fval, argc - 1, argv + 1, rval);
}

static JSBool
NewKeyValuePair(JSContext *cx, jsid key, jsval val, jsval *rval)
{
    jsval vec[2];
    JSObject *aobj;

    vec[0] = ID_TO_VALUE(key);
    vec[1] = val;
    aobj = js_NewArrayObject(cx, 2, vec);
    if (!aobj)
        return JS_FALSE;
    *rval = OBJECT_TO_JSVAL(aobj);
    return JS_TRUE;
}

static JSBool
SetupKeyValueReturn(JSContext *cx, jsid id, jsval val, jsval *rval)
{
#if JS_HAS_LVALUE_RETURN
    cx->rval2 = (jsval) id;
    cx->rval2set = JS_RVAL2_ITERKEY;
    *rval = val;
    return JS_TRUE;
#else
    return NewKeyValuePair(cx, id, val, rval);
#endif
}

static JSBool
CheckKeyValueReturn(JSContext *cx, uintN flags, jsid *idp, jsval *rval)
{
    jsval val, idval;
    JSBool arraylike;
    jsuint length;
    JSObject *obj;

    val = *rval;

#if JS_HAS_LVALUE_RETURN
    if (cx->rval2set == JS_RVAL2_ITERKEY) {
        cx->rval2set = JS_RVAL2_CLEAR;
        if (idp)
            *idp = (jsid) cx->rval2;
        if (!idp || (flags & JSITER_KEYVALUE))
            return NewKeyValuePair(cx, (jsid) cx->rval2, val, rval);
        return JS_TRUE;
    }
#endif

    if (!idp)
        return JS_TRUE;

    arraylike = JS_FALSE;
    length = 0;                         /* quell GCC overwarnings */
    obj = NULL;
    if (!JSVAL_IS_PRIMITIVE(val)) {
        obj = JSVAL_TO_OBJECT(val);
        if (!js_IsArrayLike(cx, obj, &arraylike, &length))
            return JS_FALSE;
    }

    if (arraylike && length == 2) {
        if (!OBJ_GET_PROPERTY(cx, obj, INT_TO_JSID(0), &idval))
            return JS_FALSE;
        if (!JS_ValueToId(cx, idval, idp))
            return JS_FALSE;
        if (flags & JSITER_KEYVALUE)
            return JS_TRUE;
        return OBJ_GET_PROPERTY(cx, obj, INT_TO_JSID(1), rval);
    }

    return JS_ValueToId(cx, val, idp);
}

static JSBool
iterator_next(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
              jsval *rval)
{
    JSObject *iterable;
    jsval state, val;
    uintN flags;
    JSBool foreach, ok;
    jsid id;

    if (!JS_InstanceOf(cx, obj, &js_IteratorClass, argv))
        return JS_FALSE;

    iterable = OBJ_GET_PARENT(cx, obj);
    state = OBJ_GET_SLOT(cx, obj, JSSLOT_ITER_STATE);
    if (JSVAL_IS_NULL(state))
        goto stop;

    flags = JSVAL_TO_INT(OBJ_GET_SLOT(cx, obj, JSSLOT_ITER_FLAGS));
    foreach = (flags & JSITER_FOREACH) != 0;
    ok =
#if JS_HAS_XML_SUPPORT
         (foreach && OBJECT_IS_XML(cx, iterable))
         ? ((JSXMLObjectOps *) iterable->map->ops)->
               enumerateValues(cx, iterable, JSENUMERATE_NEXT, &state,
                               &id, &val)
         :
#endif
           OBJ_ENUMERATE(cx, iterable, JSENUMERATE_NEXT, &state, &id);
    if (!ok)
        return JS_FALSE;

    OBJ_SET_SLOT(cx, obj, JSSLOT_ITER_STATE, state);
    if (JSVAL_IS_NULL(state))
        goto stop;

    if (foreach) {
#if JS_HAS_XML_SUPPORT
        if (!OBJECT_IS_XML(cx, iterable) &&
            !OBJ_GET_PROPERTY(cx, iterable, id, &val)) {
            return JS_FALSE;
        }
#endif
        if (!SetupKeyValueReturn(cx, id, val, rval))
            return JS_FALSE;
    } else {
        *rval = ID_TO_VALUE(id);
    }
    return JS_TRUE;

stop:
    js_ThrowStopIteration(cx, obj);
    return JS_FALSE;
}

static JSBool
iterator_self(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
              jsval *rval)
{
    *rval = OBJECT_TO_JSVAL(obj);
    return JS_TRUE;
}

static JSFunctionSpec iterator_methods[] = {
    {js_iterator_str, iterator_self, 0,0,0},
    {js_next_str,     iterator_next, 0,0,0},
    {0,0,0,0,0}
};

#endif /* JS_HAS_GENERATORS */

JSBool
js_NewNativeIterator(JSContext *cx, JSObject *obj, uintN flags, jsval *vp)
{
    JSObject *iterobj;
    jsval state;
    JSBool ok;

    /*
     * Create iterobj with a NULL parent to ensure that we use the correct
     * scope chain to lookup the iterator's constructor. Since we use the
     * parent slot to keep track of the iterable, we must fix it up later.
     */
    iterobj = js_NewObject(cx, &js_IteratorClass, NULL, NULL);
    if (!iterobj)
        return JS_FALSE;

    /* Store iterobj in *vp to protect it from GC (callers must root vp). */
    *vp = OBJECT_TO_JSVAL(iterobj);

    /* Initialize iterobj in case of enumerate hook failure. */
    iterobj->slots[JSSLOT_PARENT] = OBJECT_TO_JSVAL(obj);
    iterobj->slots[JSSLOT_ITER_STATE] = JSVAL_NULL;
    iterobj->slots[JSSLOT_ITER_FLAGS] = INT_TO_JSVAL(flags);
    if (!js_RegisterCloseableIterator(cx, iterobj))
        return JS_FALSE;

    ok =
#if JS_HAS_XML_SUPPORT
         ((flags & JSITER_FOREACH) && OBJECT_IS_XML(cx, obj))
         ? ((JSXMLObjectOps *) obj->map->ops)->
               enumerateValues(cx, obj, JSENUMERATE_INIT, &state, NULL, NULL)
         :
#endif
           OBJ_ENUMERATE(cx, obj, JSENUMERATE_INIT, &state, NULL);
    if (!ok)
        return JS_FALSE;

    iterobj->slots[JSSLOT_ITER_STATE] = state;
    return JS_TRUE;
}

uintN
js_GetNativeIteratorFlags(JSContext *cx, JSObject *iterobj)
{
    if (OBJ_GET_CLASS(cx, iterobj) != &js_IteratorClass)
        return 0;
    return JSVAL_TO_INT(OBJ_GET_SLOT(cx, iterobj, JSSLOT_ITER_FLAGS));
}

void
js_CloseNativeIterator(JSContext *cx, JSObject *iterobj)
{
    uintN flags;

    /*
     * If this iterator is not an instance of the native default iterator
     * class, leave it to be GC'ed.
     */
    if (!JS_InstanceOf(cx, iterobj, &js_IteratorClass, NULL))
        return;

    /*
     * If this iterator was not created by js_ValueToIterator called from the
     * for-in loop code in js_Interpret, leave it to be GC'ed.
     */
    flags = JSVAL_TO_INT(OBJ_GET_SLOT(cx, iterobj, JSSLOT_ITER_FLAGS));
    if (!(flags & JSITER_ENUMERATE))
        return;

    /*
     * Clear the cached iterator object member of cx.  Normally the GC clears
     * all contexts' cachedIterObj members, but JSOP_ENDITER calls us eagerly
     * to close iterobj.
     */
    if (iterobj == cx->cachedIterObj)
        cx->cachedIterObj = NULL;

    js_CloseIteratorState(cx, iterobj);
}

JSBool
js_DefaultIterator(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                   jsval *rval)
{
    JSBool keyonly;

    if (OBJ_GET_CLASS(cx, obj) == &js_IteratorClass) {
        *rval = OBJECT_TO_JSVAL(obj);
        return JS_TRUE;
    }

    keyonly = JS_FALSE;
    if (argc != 0 && !js_ValueToBoolean(cx, argv[0], &keyonly))
        return JS_FALSE;
    return js_NewNativeIterator(cx, obj, keyonly ? 0 : JSITER_FOREACH, rval);
}

/*
 * Inline expansion of Iterator, with extra logic to constrain the result of
 * ToObject(v).__iterator__.
 */
JSObject *
js_ValueToIterator(JSContext *cx, jsval v, uintN flags)
{
    JSObject *obj, *iterobj;
    JSTempValueRooter tvr;
    jsval arg, fval, rval;
    JSString *str;
    JSFunction *fun;
    const JSAtom *atom = cx->runtime->atomState.iteratorAtom;

    /* XXX work around old valueOf call hidden beneath js_ValueToObject */
    if (!JSVAL_IS_PRIMITIVE(v)) {
        obj = JSVAL_TO_OBJECT(v);
    } else {
        obj = js_ValueToNonNullObject(cx, v);
        if (!obj)
            return NULL;
    }

    arg = BOOLEAN_TO_JSVAL((flags & JSITER_FOREACH) == 0);

    JS_PUSH_SINGLE_TEMP_ROOT(cx, obj, &tvr);
    if (!JS_GetMethodById(cx, obj, ATOM_TO_JSID(atom), &obj, &fval))
        goto bad;
    if (JSVAL_IS_VOID(fval)) {
        /* Fail over to the default native iterator, called directly. */
        if (!js_DefaultIterator(cx, obj, 1, &arg, &rval))
            goto bad;
        if (JSVAL_IS_PRIMITIVE(rval))
            goto bad_iterator;
        iterobj = JSVAL_TO_OBJECT(rval);
        JS_ASSERT(OBJ_GET_CLASS(cx, iterobj) == &js_IteratorClass);
        iterobj->slots[JSSLOT_ITER_FLAGS] |= INT_TO_JSVAL(JSITER_ENUMERATE);
        goto out;
    }

    if (!js_InternalInvoke(cx, obj, fval, JSINVOKE_ITERATOR, 1, &arg, &rval))
        goto bad;

    if (JSVAL_IS_PRIMITIVE(rval))
        goto bad_iterator;

    iterobj = JSVAL_TO_OBJECT(rval);

    /*
     * If __iterator__ is the default native method, the native iterator it
     * returns can be flagged as hidden from script access.  This flagging is
     * predicated on js_ValueToIterator being called only by the for-in loop
     * code -- the js_CloseNativeIterator early-finalization optimization
     * based on it will break badly if script can reach iterobj.
     */
    if (OBJ_GET_CLASS(cx, iterobj) == &js_IteratorClass &&
        VALUE_IS_FUNCTION(cx, fval)) {
        fun = (JSFunction *) JS_GetPrivate(cx, JSVAL_TO_OBJECT(fval));
        if (!FUN_INTERPRETED(fun) && fun->u.n.native == js_DefaultIterator)
            iterobj->slots[JSSLOT_ITER_FLAGS] |= INT_TO_JSVAL(JSITER_ENUMERATE);
    }

out:
    JS_POP_TEMP_ROOT(cx, &tvr);
    return iterobj;

bad:
    iterobj = NULL;
    goto out;

bad_iterator:
    str = js_DecompileValueGenerator(cx, JSDVG_SEARCH_STACK, v, NULL);
    if (str) {
        JS_ReportErrorNumberUC(cx, js_GetErrorMessage, NULL,
                               JSMSG_BAD_ITERATOR_RETURN,
                               JSSTRING_CHARS(str),
                               JSSTRING_CHARS(ATOM_TO_STRING(atom)));
    }
    goto bad;
}

JSBool
js_CallIteratorNext(JSContext *cx, JSObject *iterobj, uintN flags,
                    jsid *idp, jsval *rval)
{
    JSBool unlock;
    JSObject *obj;
    JSScope *scope;
    JSScopeProperty *sprop;
    jsval fval;
    JSFunction *fun;
    const jsid id = ATOM_TO_JSID(cx->runtime->atomState.nextAtom);

    /* Fastest path for repeated call from for-in loop bytecode. */
    if (iterobj == cx->cachedIterObj) {
        JS_ASSERT(OBJ_GET_CLASS(cx, iterobj) == &js_IteratorClass);
        JS_ASSERT(flags & JSITER_ENUMERATE);
        if (!iterator_next(cx, iterobj, 0, NULL, rval) ||
            !CheckKeyValueReturn(cx, flags, idp, rval)) {
            cx->cachedIterObj = NULL;
            return JS_FALSE;
        }
        return JS_TRUE;
    }

    /* Fast path for native iterator with unoverridden .next() method. */
    unlock = JS_TRUE;
    obj = iterobj;
    JS_LOCK_OBJ(cx, obj);
    scope = OBJ_SCOPE(obj);
    sprop = NULL;

    while (LOCKED_OBJ_GET_CLASS(obj) == &js_IteratorClass) {
        obj = scope->object;
        sprop = SCOPE_GET_PROPERTY(scope, id);
        if (sprop)
            break;
        obj = LOCKED_OBJ_GET_PROTO(obj);
        if (!obj)
            break;
        JS_UNLOCK_SCOPE(cx, scope);
        scope = OBJ_SCOPE(obj);
        JS_LOCK_SCOPE(cx, scope);
    }

    if (sprop && SPROP_HAS_VALID_SLOT(sprop, scope)) {
        /*
         * Unlock scope as soon as we fetch fval, and clear the unlock flag in
         * case we do not return early after setting cx->cachedIterObj.
         */
        fval = LOCKED_OBJ_GET_SLOT(obj, sprop->slot);
        JS_UNLOCK_SCOPE(cx, scope);
        unlock = JS_FALSE;
        if (VALUE_IS_FUNCTION(cx, fval)) {
            fun = (JSFunction *) JS_GetPrivate(cx, JSVAL_TO_OBJECT(fval));
            if (!FUN_INTERPRETED(fun) && fun->u.n.native == iterator_next) {
                if (!iterator_next(cx, iterobj, 0, NULL, rval) ||
                    !CheckKeyValueReturn(cx, flags, idp, rval)) {
                    return JS_FALSE;
                }
                if (flags & JSITER_ENUMERATE)
                    cx->cachedIterObj = iterobj;
                return JS_TRUE;
            }
        }
    }

    if (unlock)
        JS_UNLOCK_SCOPE(cx, scope);

    return JS_GetMethodById(cx, iterobj, id, &iterobj, &fval) &&
           js_InternalCall(cx, iterobj, fval, 0, NULL, rval) &&
           CheckKeyValueReturn(cx, flags, idp, rval);
}

static JSBool
stopiter_hasInstance(JSContext *cx, JSObject *obj, jsval v, JSBool *bp)
{
    *bp = !JSVAL_IS_PRIMITIVE(v) &&
          OBJ_GET_CLASS(cx, JSVAL_TO_OBJECT(v)) == &js_StopIterationClass;
    return JS_TRUE;
}

JSClass js_StopIterationClass = {
    js_StopIteration_str,
    JSCLASS_HAS_CACHED_PROTO(JSProto_StopIteration),
    JS_PropertyStub,  JS_PropertyStub,
    JS_PropertyStub,  JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub,
    JS_ConvertStub,   JS_FinalizeStub,
    NULL,             NULL,
    NULL,             NULL,
    NULL,             stopiter_hasInstance,
    NULL,             NULL
};

JSBool
js_ThrowStopIteration(JSContext *cx, JSObject *obj)
{
    jsval v;

    JS_ASSERT(!JS_IsExceptionPending(cx));
    if (js_FindClassObject(cx, NULL, INT_TO_JSID(JSProto_StopIteration), &v))
        JS_SetPendingException(cx, v);
    return JS_FALSE;
}

#if JS_HAS_GENERATORS

static void
generator_finalize(JSContext *cx, JSObject *obj)
{
    JSGenerator *gen;

    gen = (JSGenerator *) JS_GetPrivate(cx, obj);
    if (gen) {
        /*
         * gen can be open on shutdown when close hooks are ignored or when
         * the embedding cancels scheduled close hooks.
         */
        JS_ASSERT(gen->state == JSGEN_NEWBORN || gen->state == JSGEN_CLOSED ||
                  gen->state == JSGEN_OPEN);
        JS_free(cx, gen);
    }
}

static uint32
generator_mark(JSContext *cx, JSObject *obj, void *arg)
{
    JSGenerator *gen;

    gen = (JSGenerator *) JS_GetPrivate(cx, obj);
    if (gen && gen->state != JSGEN_CLOSED) {
        /*
         * We must mark argv[-2], as js_MarkStackFrame will not.  Note that
         * js_MarkStackFrame will mark thisp (argv[-1]) and actual arguments,
         * plus any missing formals and local GC roots.
         */
        JS_ASSERT(!JSVAL_IS_PRIMITIVE(gen->frame.argv[-2]));
        GC_MARK(cx, JSVAL_TO_GCTHING(gen->frame.argv[-2]), "generator");
        js_MarkStackFrame(cx, &gen->frame);
    }
    return 0;
}

JSClass js_GeneratorClass = {
    js_Generator_str,
    JSCLASS_HAS_PRIVATE | JSCLASS_IS_ANONYMOUS |
    JSCLASS_HAS_CACHED_PROTO(JSProto_Generator),
    JS_PropertyStub,  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub,  JS_ConvertStub,  generator_finalize,
    NULL,             NULL,            NULL,            NULL,
    NULL,             NULL,            generator_mark,  NULL
};

JSObject *
js_NewGenerator(JSContext *cx, JSStackFrame *fp)
{
    JSObject *obj;
    uintN argc, nargs, nvars, depth, nslots;
    JSGenerator *gen;
    jsval *newsp;

    /* After the following return, failing control flow must goto bad. */
    obj = js_NewObject(cx, &js_GeneratorClass, NULL, NULL);
    if (!obj)
        return NULL;

    /* Load and compute stack slot counts. */
    argc = fp->argc;
    nargs = JS_MAX(argc, fp->fun->nargs);
    nvars = fp->nvars;
    depth = fp->script->depth;
    nslots = 2 + nargs + nvars + 2 * depth;

    /* Allocate obj's private data struct. */
    gen = (JSGenerator *)
          JS_malloc(cx, sizeof(JSGenerator) + (nslots - 1) * sizeof(jsval));
    if (!gen)
        goto bad;

    gen->obj = obj;

    /* Copy call-invariant object and function references. */
    gen->frame.callobj = fp->callobj;
    gen->frame.argsobj = fp->argsobj;
    gen->frame.varobj = fp->varobj;
    gen->frame.script = fp->script;
    gen->frame.fun = fp->fun;
    gen->frame.thisp = fp->thisp;
    gen->arena.next = NULL;

    /* Use newsp to carve space out of gen->stack. */
    newsp = gen->stack;
    gen->arena.base = (jsuword) newsp;
    gen->arena.limit = gen->arena.avail = (jsuword) (newsp + nslots);

#define COPY_STACK_ARRAY(vec,cnt,num)                                         \
    JS_BEGIN_MACRO                                                            \
        gen->frame.cnt = cnt;                                                 \
        gen->frame.vec = newsp;                                               \
        newsp += (num);                                                       \
        memcpy(gen->frame.vec, fp->vec, (num) * sizeof(jsval));               \
    JS_END_MACRO

    /* Copy argv, rval, and vars. */
    *newsp++ = fp->argv[-2];
    *newsp++ = fp->argv[-1];
    COPY_STACK_ARRAY(argv, argc, nargs);
    gen->frame.rval = fp->rval;
    COPY_STACK_ARRAY(vars, nvars, nvars);

#undef COPY_STACK_ARRAY

    /* Initialize or copy virtual machine state. */
    gen->frame.down = NULL;
    gen->frame.annotation = NULL;
    gen->frame.scopeChain = fp->scopeChain;
    gen->frame.pc = fp->pc;

    /* Allocate generating pc and operand stack space. */
    gen->frame.spbase = gen->frame.sp = newsp + depth;

    /* Copy remaining state (XXX sharp* and xml* should be local vars). */
    gen->frame.sharpDepth = 0;
    gen->frame.sharpArray = NULL;
    gen->frame.flags = fp->flags;
    gen->frame.dormantNext = NULL;
    gen->frame.xmlNamespace = NULL;
    gen->frame.blockChain = NULL;

    /* Note that gen is newborn. */
    gen->state = JSGEN_NEWBORN;

    if (!JS_SetPrivate(cx, obj, gen)) {
        JS_free(cx, gen);
        goto bad;
    }

    return obj;

  bad:
    cx->newborn[GCX_OBJECT] = NULL;
    return NULL;
}

typedef enum JSGeneratorOp {
    JSGENOP_NEXT,
    JSGENOP_SEND,
    JSGENOP_THROW,
    JSGENOP_CLOSE
} JSGeneratorOp;

/*
 * Start newborn or restart yielding generator and perform the requested
 * operation inside its frame.
 */
static JSBool
SendToGenerator(JSContext *cx, JSGeneratorOp op, JSObject *obj,
                JSGenerator *gen, jsval arg, jsval *rval)
{
    JSStackFrame *fp;
    jsval junk;
    JSArena *arena;
    JSBool ok;

    JS_ASSERT(gen->state ==  JSGEN_NEWBORN || gen->state == JSGEN_OPEN);
    switch (op) {
      case JSGENOP_NEXT:
      case JSGENOP_SEND:
        if (gen->state == JSGEN_OPEN) {
            /*
             * Store the argument to send as the result of the yield
             * expression.
             */
            gen->frame.sp[-1] = arg;
        }
        gen->state = JSGEN_RUNNING;
        break;

      case JSGENOP_THROW:
        JS_SetPendingException(cx, arg);
        gen->state = JSGEN_RUNNING;
        break;

      default:
        JS_ASSERT(op == JSGENOP_CLOSE);
        JS_SetPendingException(cx, JSVAL_ARETURN);
        gen->state = JSGEN_CLOSING;
        break;
    }

    /* Extend the current stack pool with gen->arena. */
    arena = cx->stackPool.current;
    JS_ASSERT(!arena->next);
    JS_ASSERT(!gen->arena.next);
    JS_ASSERT(cx->stackPool.current != &gen->arena);
    cx->stackPool.current = arena->next = &gen->arena;

    /* Push gen->frame around the interpreter activation. */
    fp = cx->fp;
    cx->fp = &gen->frame;
    gen->frame.down = fp;
    ok = js_Interpret(cx, gen->frame.pc, &junk);
    cx->fp = fp;
    gen->frame.down = NULL;

    /* Retract the stack pool and sanitize gen->arena. */
    JS_ASSERT(!gen->arena.next);
    JS_ASSERT(arena->next == &gen->arena);
    JS_ASSERT(cx->stackPool.current == &gen->arena);
    cx->stackPool.current = arena;
    arena->next = NULL;

    if (gen->frame.flags & JSFRAME_YIELDING) {
        /* Yield cannot fail, throw or be called on closing. */
        JS_ASSERT(ok);
        JS_ASSERT(!cx->throwing);
        JS_ASSERT(gen->state == JSGEN_RUNNING);
        JS_ASSERT(op != JSGENOP_CLOSE);
        gen->frame.flags &= ~JSFRAME_YIELDING;
        gen->state = JSGEN_OPEN;
        *rval = gen->frame.rval;
        return JS_TRUE;
    }

    gen->state = JSGEN_CLOSED;

    if (ok) {
        /* Returned, explicitly or by falling off the end. */
        if (op == JSGENOP_CLOSE)
            return JS_TRUE;
        return js_ThrowStopIteration(cx, obj);
    }

    /*
     * An error, silent termination by branch callback or an exception.
     * Propagate the condition to the caller.
     */
    return JS_FALSE;
}

/*
 * Execute gen's close hook after the GC detects that the object has become
 * unreachable.
 */
JSBool
js_CloseGeneratorObject(JSContext *cx, JSGenerator *gen)
{
    /* JSGenerator.closeLink must be already unlinked from all lists. */
    JS_ASSERT(!gen->next);
    JS_ASSERT(gen != cx->runtime->gcCloseState.reachableList);
    JS_ASSERT(gen != cx->runtime->gcCloseState.todoHead);

    /* We pass null as rval since SendToGenerator never uses it with CLOSE. */
    return SendToGenerator(cx, JSGENOP_CLOSE, gen->obj, gen, JSVAL_VOID, NULL);
}

/*
 * Common subroutine of generator_(next|send|throw|close) methods.
 */
static JSBool
generator_op(JSContext *cx, JSGeneratorOp op,
             JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSGenerator *gen;
    JSString *str;
    jsval arg;
    JSBool wasNewborn;

    if (!JS_InstanceOf(cx, obj, &js_GeneratorClass, argv))
        return JS_FALSE;

    gen = (JSGenerator *) JS_GetPrivate(cx, obj);
    switch (gen->state) {
      case JSGEN_NEWBORN:
        switch (op) {
          case JSGENOP_NEXT:
          case JSGENOP_THROW:
            break;

          case JSGENOP_SEND:
            if (!JSVAL_IS_VOID(argv[0])) {
                str = js_DecompileValueGenerator(cx, JSDVG_SEARCH_STACK,
                                                 argv[0], NULL);
                if (str) {
                    JS_ReportErrorNumberUC(cx, js_GetErrorMessage, NULL,
                                           JSMSG_BAD_GENERATOR_SEND,
                                           JSSTRING_CHARS(str));
                }
                return JS_FALSE;
            }
            break;

          default:
            JS_ASSERT(op == JSGENOP_CLOSE);
            gen->state = JSGEN_CLOSED;
            return JS_TRUE;
        }
        wasNewborn = JS_TRUE;
        break;

      case JSGEN_OPEN:
        wasNewborn = JS_FALSE;
        break;

      case JSGEN_RUNNING:
      case JSGEN_CLOSING:
        str = js_DecompileValueGenerator(cx, JSDVG_SEARCH_STACK, argv[-1],
                                         JS_GetFunctionId(gen->frame.fun));
        if (str) {
            JS_ReportErrorNumberUC(cx, js_GetErrorMessage, NULL,
                                   JSMSG_NESTING_GENERATOR,
                                   JSSTRING_CHARS(str));
        }
        return JS_FALSE;

      default:
        JS_ASSERT(gen->state == JSGEN_CLOSED);
        switch (op) {
          case JSGENOP_NEXT:
          case JSGENOP_SEND:
            return js_ThrowStopIteration(cx, obj);
          case JSGENOP_THROW:
            JS_SetPendingException(cx, argv[0]);
            return JS_FALSE;
          default:
            JS_ASSERT(op == JSGENOP_CLOSE);
            return JS_TRUE;
        }
    }

    arg = (op == JSGENOP_SEND || op == JSGENOP_THROW)
          ? argv[0]
          : JSVAL_VOID;
    if (!SendToGenerator(cx, op, obj, gen, arg, rval))
        return JS_FALSE;
    if (wasNewborn && gen->state == JSGEN_OPEN) {
        /*
         * The generator yielded the first time. Register it with GC to ensure
         * that suspended finally blocks will be executed.
         */
        js_RegisterOpenGenerator(cx, gen);
    }
    return JS_TRUE;
}

static JSBool
generator_send(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
               jsval *rval)
{
    return generator_op(cx, JSGENOP_SEND, obj, argc, argv, rval);
}

static JSBool
generator_next(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
               jsval *rval)
{
    return generator_op(cx, JSGENOP_NEXT, obj, argc, argv, rval);
}

static JSBool
generator_throw(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                jsval *rval)
{
    return generator_op(cx, JSGENOP_THROW, obj, argc, argv, rval);
}

static JSBool
generator_close(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                jsval *rval)
{
    return generator_op(cx, JSGENOP_CLOSE, obj, argc, argv, rval);
}

static JSFunctionSpec generator_methods[] = {
    {js_iterator_str, iterator_self,     0,0,0},
    {js_next_str,     generator_next,    0,0,0},
    {js_send_str,     generator_send,    1,0,0},
    {js_throw_str,    generator_throw,   1,0,0},
    {js_close_str,    generator_close,   0,JSPROP_READONLY|JSPROP_PERMANENT,0},
    {0,0,0,0,0}
};

#endif /* JS_HAS_GENERATORS */

JSObject *
js_InitIteratorClasses(JSContext *cx, JSObject *obj)
{
    JSObject *proto, *stop;

    /* Idempotency required: we initialize several things, possibly lazily. */
    if (!js_GetClassObject(cx, obj, JSProto_StopIteration, &stop))
        return NULL;
    if (stop)
        return stop;

#if JS_HAS_GENERATORS
    /* Expose Iterator and initialize the generator internals if configured. */
    proto = JS_InitClass(cx, obj, NULL, &js_IteratorClass, Iterator, 2,
                         NULL, iterator_methods, NULL, NULL);
    if (!proto)
        return NULL;
    proto->slots[JSSLOT_ITER_STATE] = JSVAL_NULL;

    if (!JS_InitClass(cx, obj, NULL, &js_GeneratorClass, NULL, 0,
                      NULL, generator_methods, NULL, NULL)) {
        return NULL;
    }
#endif

    return JS_InitClass(cx, obj, NULL, &js_StopIterationClass, NULL, 0,
                        NULL, NULL, NULL, NULL);
}
