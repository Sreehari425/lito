export module lito.frontend.parser:header_dependency;

import rstd;
import lito.frontend.result;
import lito.frontend.preprocessor;

using namespace rstd::prelude;

export namespace lito::frontend::parser
{

class HeaderDependencyConsumer {
public:
    static auto make() -> HeaderDependencyConsumer { return HeaderDependencyConsumer {}; }

    auto consume(slice<lexical::Token> tokens) -> lexical::Result<empty> {
        static_cast<void>(tokens);
        return Ok(empty {});
    }

    auto finish(const preprocessor::PreprocessedTranslationUnit& translation)
        -> lexical::Result<FrontendResult> {
        auto headers = translation.header_inputs.iter()
                           .map([](auto path) {
                               return path->clone();
                           })
                           .collect<Vec<rstd::path::PathBuf>>();
        return Ok(FrontendResult {
            .source = rstd::path::PathBuf::from(translation.sources.path(translation.main_source)),
            .header_inputs            = rstd::move(headers),
            .embedded_inputs          = as<Clone>(translation.embedded_inputs).clone(),
            .external_macros          = as<Clone>(translation.external_macros).clone(),
            .preprocessor_environment = translation.environment_identity.clone(),
            .input_bytes              = translation.input_bytes,
        });
    }
};

} // namespace lito::frontend::parser
