#ifndef CYANIDE_HOOK_IMPL_POLYHOOK_HPP_
#define CYANIDE_HOOK_IMPL_POLYHOOK_HPP_

#include <cyanide/function_traits.hpp>
#include <cyanide/hook_wrapper.hpp>

#include <polyhook2/Detour/ADetour.hpp>
#include <polyhook2/Detour/x86Detour.hpp>

#include <concepts>
#include <cstdint>
#include <format>
#include <optional>
#include <stdexcept>
#include <utility> // std::forward

namespace cyanide {

template <std::derived_from<PLH::Detour> HookT>
class polyhook_implementation {
public:
    void install(void *source, const void *destination)
    {
        detour_.emplace(
            reinterpret_cast<std::uint64_t>(source),
            reinterpret_cast<std::uint64_t>(destination),
            &trampoline_);

        if (!detour_->hook())
            throw std::runtime_error{
                std::format("Failed to install the hook at {}", source)};
    }

    void uninstall()
    {
        const bool result = detour_->unHook();
        detour_.reset();

        if (!result)
            throw std::runtime_error{"Failed to uninstall the hook"};
    }

    void *get_trampoline()
    {
        return reinterpret_cast<void *>(trampoline_);
    }

protected:
    std::optional<HookT> detour_;
    std::uint64_t        trampoline_ = 0;
};

template <
    cyanide::types::FunctionPtr SourceT,
    typename CallbackT,
    typename... HookArgs>
auto make_polyhook_x86(
    SourceT   &&source,
    CallbackT &&callback,
    HookArgs &&...hook_args)
{
    return hook_wrapper<
        polyhook_implementation<PLH::x86Detour>,
        SourceT,
        CallbackT,
        HookArgs...>{
        std::forward<SourceT>(source),
        std::forward<CallbackT>(callback),
        std::forward<HookArgs>(hook_args)...};
}

} // namespace cyanide

#endif // !CYANIDE_HOOK_IMPL_POLYHOOK_HPP_