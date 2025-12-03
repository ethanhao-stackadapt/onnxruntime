#include <iostream>
#include <onnxruntime/core/session/onnxruntime_cxx_api.h>

// Declare the ACL-specific C API function (defined in libonnxruntime.so)
extern "C" OrtStatus* OrtSessionOptionsAppendExecutionProvider_ACL(OrtSessionOptions* options, int use_arena);

int main() {
    try {
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");

        // Get available providers
        std::vector<std::string> providers = Ort::GetAvailableProviders();

        std::cout << "Available Execution Providers:" << std::endl;
        for (const auto& provider : providers) {
            std::cout << "  - " << provider << std::endl;
        }

        // Check if ACL is available
        bool acl_found = false;
        for (const auto& provider : providers) {
            if (provider == "AclExecutionProvider" || provider == "ACLExecutionProvider") {
                acl_found = true;
                break;
            }
        }

        if (acl_found) {
            std::cout << "\n✅ ACL ExecutionProvider is AVAILABLE" << std::endl;

            // Use the ACL-specific C API to enable it
            Ort::SessionOptions session_options;

            // OrtSessionOptionsAppendExecutionProvider_ACL(session_options, use_arena)
            // use_arena = 1 means use memory arena for better performance
            OrtStatus* status = OrtSessionOptionsAppendExecutionProvider_ACL(session_options, 1);

            if (status == nullptr) {
                std::cout << "✅ Successfully configured session with ACL via C API" << std::endl;
                return 0;
            } else {
                const OrtApi* api = OrtGetApiBase()->GetApi(ORT_API_VERSION);
                const char* error_message = api->GetErrorMessage(status);
                std::cerr << "❌ Failed to enable ACL: " << error_message << std::endl;
                api->ReleaseStatus(status);
                return 1;
            }
        } else {
            std::cout << "\n❌ ACL ExecutionProvider is NOT AVAILABLE" << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
