# Building ONNX Runtime 1.22.0 with XNNPACK for AWS Graviton (ARM64)

## Quick Start

```bash
# 1. Initialize submodules
git submodule update --init --recursive

# 2. Build Docker image
docker buildx build --platform linux/arm64 \
  -t onnxruntime-graviton-xnnpack:1.22.0 \
  -f dockerfiles/Dockerfile.graviton-xnnpack \
  --load .

# 3. Extract library
mkdir -p onnxruntime-graviton-arm64
docker run --rm -v $(pwd)/onnxruntime-graviton-arm64:/output \
  onnxruntime-graviton-xnnpack:1.22.0 \
  sh -c "cp /usr/local/lib/libonnxruntime.so* /output/ && cp -r /usr/local/include/onnxruntime /output/"
```

## Output

- `libonnxruntime.so.1.22.0` - Shared library with XNNPACK enabled
- `libonnxruntime.so` - Symlink
- `onnxruntime/` - C/C++ headers

## Changes Made to Build Successfully

| File | Change | Reason |
|------|--------|--------|
| `cmake/deps.txt` | Updated Eigen hash from `5ea4d05e...` to `51982be8...` | GitLab regenerated the archive, changing its SHA1 |
| `Dockerfile.graviton-xnnpack` | Use Amazon Linux 2023 base | Provides glibc 2.34 (compatible with Graviton instances) |
| `Dockerfile.graviton-xnnpack` | Install Python 3.11 | ONNX Runtime 1.22.0 uses `match/case` syntax (Python 3.10+) |
| `Dockerfile.graviton-xnnpack` | Install CMake 3.28.3 | ONNX Runtime 1.22.0 requires CMake 3.28+ |
| `Dockerfile.graviton-xnnpack` | Add `patch` utility | Required by XNNPACK to apply patches during build |

## Verify XNNPACK is Included

```bash
# Check for XNNPACK symbols
nm onnxruntime-graviton-arm64/libonnxruntime.so.1.22.0 | grep -i xnnpack
```

Should show `XnnpackExecutionProvider`, `XnnpackProviderFactory`, etc.
