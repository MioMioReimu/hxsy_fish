
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

template<
    typename Signature,
    typename Fn, typename StackAllocator, typename Allocator,
    typename Caller,
    int arity
>
class coroutine_object< Signature, Fn, StackAllocator, Allocator, Caller, void, arity > :
    private stack_tuple< StackAllocator >,
    public coroutine_base< Signature >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Signature, Fn, StackAllocator, Allocator, Caller, void, arity
        >
    >::other                                            allocator_t;
    typedef typename arg< Signature >::type             arg_type;

private:
    typedef stack_tuple< StackAllocator >                pbase_type;
    typedef coroutine_base< Signature >                 base_type;

    Fn                      fn_;
    allocator_t             alloc_;

    static void destroy_( allocator_t & alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    coroutine_object( coroutine_object &);
    coroutine_object & operator=( coroutine_object const&);

    void enter_()
    {
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                this->caller_.jump(
                    this->callee_,
                    reinterpret_cast< intptr_t >( this),
                    this->preserve_fpu() ) ) );
        this->callee_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void enter_( typename detail::param< arg_type >::type arg)
    {
        tuple< coroutine_object *,
            typename detail::param< arg_type >::type
        > tpl( this, arg);
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                this->caller_.jump(
                    this->callee_,
                    reinterpret_cast< intptr_t >( & tpl),
                    this->preserve_fpu() ) ) );
        this->callee_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void unwind_stack_() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( ! this->is_complete() );

        this->flags_ |= flag_unwind_stack;
        holder< arg_type > hldr_to( & this->caller_, true);
        this->caller_.jump(
            this->callee_,
            reinterpret_cast< intptr_t >( & hldr_to),
            this->preserve_fpu() );
        this->flags_ &= ~flag_unwind_stack;

        BOOST_ASSERT( this->is_complete() );
    }

public:
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    coroutine_object( BOOST_RV_REF( Fn) fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline1< coroutine_object >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( forward< Fn >( fn) ),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( BOOST_RV_REF( Fn) fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( forward< Fn >( fn) ),
        alloc_( alloc)
    { enter_( arg); }
#else
    coroutine_object( Fn fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline1< coroutine_object >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( Fn fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_( arg); }

    coroutine_object( BOOST_RV_REF( Fn) fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline1< coroutine_object >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( BOOST_RV_REF( Fn) fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_( arg); }
#endif

    ~coroutine_object()
    {
        if ( ! this->is_complete() && this->force_unwind() )
            unwind_stack_();
    }

    void run()
    {
        coroutine_context callee;
        coroutine_context caller;

        {
            Caller c( this->caller_, false, this->preserve_fpu(), alloc_);
            try
            { fn_( c); }
            catch ( forced_unwind const&)
            {}
            catch (...)
            { this->except_ = current_exception(); }
            callee = c.impl_->callee_;
        }

        this->flags_ |= flag_complete;
        holder< void > hldr_to( & caller);
        caller.jump(
            callee,
            reinterpret_cast< intptr_t >( & hldr_to),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

    void run( typename detail::param< arg_type >::type arg)
    {
        coroutine_context callee;
        coroutine_context caller;

        {
            Caller c( this->caller_, false, this->preserve_fpu(), alloc_);
            c.impl_->result_ = arg;
            try
            { fn_( c); }
            catch ( forced_unwind const&)
            {}
            catch (...)
            { this->except_ = current_exception(); }
            callee = c.impl_->callee_;
        }

        this->flags_ |= flag_complete;
        holder< void > hldr_to( & caller);
        caller.jump(
            callee,
            reinterpret_cast< intptr_t >( & hldr_to),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

    void deallocate_object()
    { destroy_( alloc_, this); }
};

template<
    typename Signature,
    typename Fn, typename StackAllocator, typename Allocator,
    typename Caller,
    int arity
>
class coroutine_object< Signature, reference_wrapper< Fn >, StackAllocator, Allocator, Caller, void, arity > :
    private stack_tuple< StackAllocator >,
    public coroutine_base< Signature >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Signature, Fn, StackAllocator, Allocator, Caller, void, arity
        >
    >::other                                            allocator_t;
    typedef typename arg< Signature >::type             arg_type;

private:
    typedef stack_tuple< StackAllocator >                pbase_type;
    typedef coroutine_base< Signature >                 base_type;

    Fn                      fn_;
    allocator_t             alloc_;

    static void destroy_( allocator_t & alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    coroutine_object( coroutine_object &);
    coroutine_object & operator=( coroutine_object const&);

    void enter_()
    {
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                this->caller_.jump(
                    this->callee_,
                    reinterpret_cast< intptr_t >( this),
                    this->preserve_fpu() ) ) );
        this->callee_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void enter_( typename detail::param< arg_type >::type arg)
    {
        tuple< coroutine_object *,
            typename detail::param< arg_type >::type
        > tpl( this, arg);
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                this->caller_.jump(
                    this->callee_,
                    reinterpret_cast< intptr_t >( & tpl),
                    this->preserve_fpu() ) ) );
        this->callee_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void unwind_stack_() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( ! this->is_complete() );

        this->flags_ |= flag_unwind_stack;
        holder< arg_type > hldr_to( & this->caller_, true);
        this->caller_.jump(
            this->callee_,
            reinterpret_cast< intptr_t >( & hldr_to),
            this->preserve_fpu() );
        this->flags_ &= ~flag_unwind_stack;

        BOOST_ASSERT( this->is_complete() );
    }

public:
    coroutine_object( reference_wrapper< Fn > fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline1< coroutine_object >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( reference_wrapper< Fn > fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_( arg); }

    ~coroutine_object()
    {
        if ( ! this->is_complete() && this->force_unwind() )
            unwind_stack_();
    }

    void run()
    {
        coroutine_context callee;
        coroutine_context caller;

        {
            Caller c( this->caller_, false, this->preserve_fpu(), alloc_);
            try
            { fn_( c); }
            catch ( forced_unwind const&)
            {}
            catch (...)
            { this->except_ = current_exception(); }
            callee = c.impl_->callee_;
        }

        this->flags_ |= flag_complete;
        holder< void > hldr_to( & caller);
        caller.jump(
            callee,
            reinterpret_cast< intptr_t >( & hldr_to),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

    void run( typename detail::param< arg_type >::type arg)
    {
        coroutine_context callee;
        coroutine_context caller;

        {
            Caller c( this->caller_, false, this->preserve_fpu(), alloc_);
            c.impl_->result_ = arg;
            try
            { fn_( c); }
            catch ( forced_unwind const&)
            {}
            catch (...)
            { this->except_ = current_exception(); }
            callee = c.impl_->callee_;
        }

        this->flags_ |= flag_complete;
        holder< void > hldr_to( & caller);
        caller.jump(
            callee,
            reinterpret_cast< intptr_t >( & hldr_to),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

    void deallocate_object()
    { destroy_( alloc_, this); }
};

template<
    typename Signature,
    typename Fn, typename StackAllocator, typename Allocator,
    typename Caller,
    int arity
>
class coroutine_object< Signature, const reference_wrapper< Fn >, StackAllocator, Allocator, Caller, void, arity > :
    private stack_tuple< StackAllocator >,
    public coroutine_base< Signature >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Signature, Fn, StackAllocator, Allocator, Caller, void, arity
        >
    >::other                                            allocator_t;
    typedef typename arg< Signature >::type             arg_type;

private:
    typedef stack_tuple< StackAllocator >                pbase_type;
    typedef coroutine_base< Signature >                 base_type;

    Fn                      fn_;
    allocator_t             alloc_;

    static void destroy_( allocator_t & alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    coroutine_object( coroutine_object &);
    coroutine_object & operator=( coroutine_object const&);

    void enter_()
    {
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                this->caller_.jump(
                    this->callee_,
                    reinterpret_cast< intptr_t >( this),
                    this->preserve_fpu() ) ) );
        this->callee_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void enter_( typename detail::param< arg_type >::type arg)
    {
        tuple< coroutine_object *,
            typename detail::param< arg_type >::type
        > tpl( this, arg);
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                this->caller_.jump(
                    this->callee_,
                    reinterpret_cast< intptr_t >( & tpl),
                    this->preserve_fpu() ) ) );
        this->callee_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void unwind_stack_() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( ! this->is_complete() );

        this->flags_ |= flag_unwind_stack;
        holder< arg_type > hldr_to( & this->caller_, true);
        this->caller_.jump(
            this->callee_,
            reinterpret_cast< intptr_t >( & hldr_to),
            this->preserve_fpu() );
        this->flags_ &= ~flag_unwind_stack;

        BOOST_ASSERT( this->is_complete() );
    }

public:
    coroutine_object( const reference_wrapper< Fn > fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline1< coroutine_object >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( const reference_wrapper< Fn > fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_( arg); }

    ~coroutine_object()
    {
        if ( ! this->is_complete() && this->force_unwind() )
            unwind_stack_();
    }

    void run()
    {
        coroutine_context callee;
        coroutine_context caller;

        {
            Caller c( this->caller_, false, this->preserve_fpu(), alloc_);
            try
            { fn_( c); }
            catch ( forced_unwind const&)
            {}
            catch (...)
            { this->except_ = current_exception(); }
            callee = c.impl_->callee_;
        }

        this->flags_ |= flag_complete;
        holder< void > hldr_to( & caller);
        caller.jump(
            callee,
            reinterpret_cast< intptr_t >( & hldr_to),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

    void run( typename detail::param< arg_type >::type arg)
    {
        coroutine_context callee;
        coroutine_context caller;

        {
            Caller c( this->caller_, false, this->preserve_fpu(), alloc_);
            c.impl_->result_ = arg;
            try
            { fn_( c); }
            catch ( forced_unwind const&)
            {}
            catch (...)
            { this->except_ = current_exception(); }
            callee = c.impl_->callee_;
        }

        this->flags_ |= flag_complete;
        holder< void > hldr_to( & caller);
        caller.jump(
            callee,
            reinterpret_cast< intptr_t >( & hldr_to),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

    void deallocate_object()
    { destroy_( alloc_, this); }
};
