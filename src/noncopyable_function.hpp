#ifndef TOP100_NONCOPYABLE_FUNCTION_HPP_
#define TOP100_NONCOPYABLE_FUNCTION_HPP_

#include <cstdint>
#include <utility>
#include <functional>
#include <type_traits>

namespace top100 {

template <typename>
class NoncopyableFunction;

template <typename R, typename... Args>
class NoncopyableFunction<R(Args...)> {
private:
    // NOTE: sizeof(std::function<void()>) == 32
    static constexpr std::size_t DIRECT_CALL_SIZE = 32;

    using Self = NoncopyableFunction;
    using Call = R (*)(const Self* f, Args...);
    using Move = void (*)(Self* from, Self* to);
    using Dsty = void (*)(Self* f);

    union [[gnu::may_alias]] Storage {
        char direct[DIRECT_CALL_SIZE];
        void* indirect;
    };

    struct Vtable {
        const Call call;
        const Move move;
        const Dsty destroy;
    };

    static R emptyCall(const Self*, Args...) {
        throw std::bad_function_call();
    }
    static void emptyMove(Self*, Self*) {}
    static void emptyDestroy(Self*) {}

    static constexpr Vtable emptyVtable = {emptyCall, emptyMove, emptyDestroy};

    template <typename F>
    struct DirectVtable {
        static F* access(Self* f) noexcept {
            return reinterpret_cast<F*>(f->sto_.direct);
        }

        static const F* access(const Self* f) noexcept {
            return reinterpret_cast<const F*>(f->sto_.direct);
        }

        static R call(const Self* f, Args... args) {
            return  (*access(const_cast<Self*>(f)))(std::forward<Args>(args)...);
        }

        static void move(Self* from, Self* to) noexcept {
            new (access(to)) F(std::move(*access(from)));
            destroy(from);
        }

        static void destroy(Self* f) noexcept {
            access(f)->~F();
        }

        static void init(F&& f, Self* to) {
            new (access(to)) F(std::move(f));
        }

        static constexpr Vtable makeVtable() noexcept {
            return {call, move, destroy};
        }
        static const Vtable vtable;
    };

    template <typename F>
    struct IndirectVtable {
        static F* access(Self* f) noexcept {
            return reinterpret_cast<F*>(f->sto_.indirect);
        }

        static const F* access(const Self* f) noexcept {
            return reinterpret_cast<const F*>(f->sto_.indirect);
        }

        static R call(const Self* f, Args... args) {
            return (*access(const_cast<Self*>(f)))(std::forward<Args>(args)...);
        }

        static void move(Self* from, Self* to) noexcept {
            new (&to->sto_.indirect) (void*)(from->sto_.indirect);
        }

        static void destroy(Self* f) noexcept {
            delete access(f);
        }

        static void init(F&& f, Self* to) {
            to->sto_.indirect = new F(std::move(f));
        }

        static constexpr Vtable makeVtable() noexcept {
            return {call, move, destroy};
        }
        static const Vtable vtable;
    };

    template <typename F>
    class VtableFor
        : public std::conditional_t<sizeof(F) <= DIRECT_CALL_SIZE &&
                                        alignof(F) <= alignof(Storage) &&
                                        std::is_nothrow_move_constructible_v<F>,
                                    DirectVtable<F>, IndirectVtable<F>> {};

public:
    NoncopyableFunction() noexcept : vtable_(&emptyVtable) {
        sto_.indirect = nullptr;
    }

    template <typename F>
    NoncopyableFunction(F f) {
        VtableFor<F>::init(std::move(f), this);
        vtable_ = &VtableFor<F>::vtable;
    }

    template <typename T, typename... As>
    NoncopyableFunction(R (T::*member)(As...)) : NoncopyableFunction(std::mem_fn(member)) {}

    template <typename T, typename... As>
    NoncopyableFunction(const R (T::*member)(As...)) : NoncopyableFunction(std::mem_fn(member)) {}

    // Grab the vtable & data from `rhs`
    // `rhs` is like the initial NoncopyableFunction
    NoncopyableFunction(NoncopyableFunction&& rhs) noexcept
        : vtable_(std::exchange(rhs.vtable_, &emptyVtable)) {
        vtable_->move(&rhs, this);
    }

    NoncopyableFunction& operator=(NoncopyableFunction&& rhs) noexcept {
        if (this != &rhs) {
            this->~NoncopyableFunction();
            new (this) NoncopyableFunction(std::move(rhs));
        }
        return *this;
    }

    NoncopyableFunction(const NoncopyableFunction&) = delete;
    NoncopyableFunction& operator=(const NoncopyableFunction&) = delete;

    ~NoncopyableFunction() {
        vtable_->destroy(this);
    }

    R operator()(Args... args) const {
        return vtable_->call(this, std::forward<Args>(args)...);
    }

    explicit operator bool() const {
        return vtable_ != &emptyVtable;
    }

private:
    Storage sto_;

    const Vtable* vtable_;
};


template <typename R, typename... Args>
constexpr typename NoncopyableFunction<R(Args...)>::Vtable NoncopyableFunction<R(Args...)>::emptyVtable;

template <typename R, typename... Args>
template <typename F>
const typename NoncopyableFunction<R(Args...)>::Vtable NoncopyableFunction<R(Args...)>::DirectVtable<F>::vtable
        = NoncopyableFunction<R(Args...)>::DirectVtable<F>::makeVtable();

template <typename R, typename... Args>
template <typename F>
const typename NoncopyableFunction<R(Args...)>::Vtable NoncopyableFunction<R(Args...)>::IndirectVtable<F>::vtable
        = NoncopyableFunction<R(Args...)>::IndirectVtable<F>::makeVtable();
}

#endif
