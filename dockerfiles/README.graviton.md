# Building ONNX Runtime 1.22.0 for AWS Graviton2 (ARM64)

This directory contains Dockerfiles for building ONNX Runtime with different execution providers optimized for **AWS Graviton2** processors (c6g, m6g, r6g instances).

## Target Platform

- **Instance types**: c6g, m6g, r6g (Graviton2)
- **Architecture**: ARMv8.2-A with NEON and dot product instructions
- **OS**: Amazon Linux 2023

> **Note**: To support other Graviton generations, see the `ACL_ARCH` build arg in the Dockerfiles.
> - Graviton1: use `--build-arg ACL_ARCH=arm64-v8a`
> - Graviton3/4: default `arm64-v8.2-a` works, or enable SVE for more optimization

## Available Builds

| Dockerfile | Backend | Best For |
|------------|---------|----------|
| `Dockerfile.graviton-xnnpack` | XNNPACK | General inference, well-maintained |
| `Dockerfile.graviton-acl` | ACL (Arm Compute Library) | ARM-optimized ops, NEON acceleration |

---

## Option 1: XNNPACK Build

### Quick Start

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

### Verify XNNPACK is Included

```bash
nm onnxruntime-graviton-arm64/libonnxruntime.so.1.22.0 | grep -i xnnpack
```

Should show `XnnpackExecutionProvider`, `XnnpackProviderFactory`, etc.

---

## Option 2: ACL Build (Arm Compute Library)

Target: **Graviton2** (ARMv8.2-A) with ACL v24.04

### Quick Start

```bash
# 1. Initialize submodules
git submodule update --init --recursive

# 2. Build Docker image (includes building ACL v24.04 from source)
docker buildx build --platform linux/arm64 \
  -t onnxruntime-graviton-acl:1.22.0 \
  -f dockerfiles/Dockerfile.graviton-acl \
  --load .

# 3. Extract libraries (includes ACL runtime libs)
mkdir -p onnxruntime-graviton-acl-arm64
docker run --rm -v $(pwd)/onnxruntime-graviton-acl-arm64:/output \
  onnxruntime-graviton-acl:1.22.0 \
  sh -c "cp /usr/local/lib/libonnxruntime.so* /output/ && \
         cp /usr/local/lib/libarm_compute*.so /output/ && \
         cp -r /usr/local/include/onnxruntime /output/"
```

### Verify ACL is Included

```bash
nm onnxruntime-graviton-acl-arm64/libonnxruntime.so.1.22.0 | grep -i acl
```

Should show `AclExecutionProvider`, `AclProviderFactory`, etc.

### ACL Runtime Dependencies

The ACL build requires these libraries at runtime:
- `libarm_compute.so`
- `libarm_compute_graph.so`

Make sure to deploy them alongside `libonnxruntime.so`.

---

## Output Files

| Build | Files |
|-------|-------|
| XNNPACK | `libonnxruntime.so.1.22.0`, `libonnxruntime.so`, `onnxruntime/` headers |
| ACL | Same as above + `libarm_compute.so`, `libarm_compute_graph.so` |

---

## Changes Made to Build Successfully

| File | Change | Reason |
|------|--------|--------|
| `cmake/deps.txt` | Updated Eigen hash from `5ea4d05e...` to `51982be8...` | GitLab regenerated the archive, changing its SHA1 |
| `Dockerfile.graviton-*` | Use Amazon Linux 2023 base | Provides glibc 2.34 (compatible with Graviton instances) |
| `Dockerfile.graviton-*` | Install Python 3.11 | ONNX Runtime 1.22.0 uses `match/case` syntax (Python 3.10+) |
| `Dockerfile.graviton-*` | Install CMake 3.28.3 | ONNX Runtime 1.22.0 requires CMake 3.28+ |
| `Dockerfile.graviton-xnnpack` | Add `patch` utility | Required by XNNPACK to apply patches during build |
| `Dockerfile.graviton-acl` | Add `scons` build tool | Required to build ACL from source |

---

## XNNPACK vs ACL: Which to Choose?

| Aspect | XNNPACK | ACL |
|--------|---------|-----|
| Maintainer | Google | Arm |
| Optimization target | Mobile/edge, general ARM | ARM-specific (NEON, SVE) |
| Build complexity | Simple | Requires building ACL first |
| Runtime deps | None extra | ACL shared libraries |
| Graviton support | Good | Good (Neoverse optimized) |

**Recommendation**: Start with **XNNPACK** for simplicity. Try **ACL** if you need specific operator optimizations or want to benchmark both.
