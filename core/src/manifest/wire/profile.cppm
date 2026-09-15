module;
#include <rstd/macro.hpp>
export module lito.core:manifest.wire.profile;
import rstd;
import rstd.serde;
import :manifest.wire;
using namespace rstd::prelude;
using namespace rstd::literals;

export namespace lito::manifest::wire
{
struct ProfileValue {
    Option<bool>   boolean;
    Option<i64>    integer;
    Option<String> text;
};
struct BuildProfile {
    Option<String>       inherits;
    Option<ProfileValue> optimization, debug, strip, lto;
    Option<bool>         exceptions, rtti;
};
struct Profiles {
    Option<BaseProfile>                               base;
    Option<bool>                                      exceptions, rtti;
    rstd::collections::BTreeMap<String, BuildProfile> named;
};
} // namespace lito::manifest::wire
export namespace rstd
{
template<>
struct Impl<serde::Deserialize, lito::manifest::wire::ProfileValue> {
    template<typename D>
    static auto deserialize(D& d)
        -> Result<lito::manifest::wire::ProfileValue, typename D::error_type> {
        using lito::manifest::wire::ProfileValue;
        if (d.kind() == serde::ValueKind::Boolean)
            return Ok(ProfileValue { .boolean = Some(rstd_try(d.deserialize_bool())) });
        if (d.kind() == serde::ValueKind::SignedInteger)
            return Ok(ProfileValue { .integer = Some(rstd_try(d.deserialize_i64())) });
        return Ok(ProfileValue { .text = Some(rstd_try(d.deserialize_string())) });
    }
};
template<>
struct Impl<serde::Deserialize, lito::manifest::wire::BuildProfile> {
    template<typename D>
    static auto deserialize(D& d)
        -> Result<lito::manifest::wire::BuildProfile, typename D::error_type> {
        using namespace lito::manifest::wire;
        auto inherits     = serde::OptionalField<String>("inherits"_str);
        auto optimization = serde::OptionalField<ProfileValue>("opt-level"_str);
        auto debug        = serde::OptionalField<ProfileValue>("debug"_str);
        auto strip        = serde::OptionalField<ProfileValue>("strip"_str);
        auto lto          = serde::OptionalField<ProfileValue>("lto"_str);
        auto exceptions   = serde::OptionalField<bool>("exceptions"_str);
        auto rtti         = serde::OptionalField<bool>("rtti"_str);
        rstd_try(serde::deserialize_record(d,
                                           serde::UnknownFieldPolicy::Reject,
                                           inherits,
                                           optimization,
                                           debug,
                                           strip,
                                           lto,
                                           exceptions,
                                           rtti));
        return Ok(BuildProfile { .inherits     = inherits.take(),
                                 .optimization = optimization.take(),
                                 .debug        = debug.take(),
                                 .strip        = strip.take(),
                                 .lto          = lto.take(),
                                 .exceptions   = exceptions.take(),
                                 .rtti         = rtti.take() });
    }
};
template<>
struct Impl<serde::Deserialize, lito::manifest::wire::Profiles> {
    template<typename D>
    static auto deserialize(D& d)
        -> Result<lito::manifest::wire::Profiles, typename D::error_type> {
        using namespace lito::manifest::wire;
        auto base       = serde::OptionalField<BaseProfile>("base"_str);
        auto exceptions = serde::OptionalField<bool>("exceptions"_str);
        auto rtti       = serde::OptionalField<bool>("rtti"_str);
        auto result     = Profiles {};
        rstd_try(serde::deserialize_record(
            d, [&](ref<str> name, auto& map) -> Result<empty, typename D::error_type> {
                if (rstd_try(serde::assign_record_field(d, map, name, base, exceptions, rtti)))
                    return Ok(empty {});
                if (result.named.contains_key(name)) return Err(d.duplicate_field(name));
                result.named.insert(String::make(name),
                                    rstd_try(map.template next_value<BuildProfile>()));
                return Ok(empty {});
            }));
        result.base       = base.take();
        result.exceptions = exceptions.take();
        result.rtti       = rtti.take();
        return Ok(rstd::move(result));
    }
};
} // namespace rstd
