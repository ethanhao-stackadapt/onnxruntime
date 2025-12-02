#include <iostream>
#include <onnxruntime/core/session/onnxruntime_cxx_api.h>

int main() {
    try {
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");

        // Get available providers
        std::vector<std::string> providers = Ort::GetAvailableProviders();

        std::cout << "Available Execution Providers:" << std::endl;
        for (const auto& provider : providers) {
            std::cout << "  - " << provider << std::endl;
        }

        // Check if XNNPACK is available
        bool xnnpack_found = false;
        for (const auto& provider : providers) {
            if (provider == "XnnpackExecutionProvider") {
                xnnpack_found = true;
                break;
            }
        }

        if (xnnpack_found) {
            std::cout << "\n✅ XNNPACK ExecutionProvider is AVAILABLE" << std::endl;

            // Try to create a session with XNNPACK
            Ort::SessionOptions session_options;
            session_options.AppendExecutionProvider("XNNPACK");
            std::cout << "✅ Successfully configured session with XNNPACK" << std::endl;

            return 0;
        } else {
            std::cout << "\n❌ XNNPACK ExecutionProvider is NOT AVAILABLE" << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
