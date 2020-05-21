#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <turbojpeg.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    tjhandle jpegDecompressor = tjInitDecompress();

    int width, height, subsamp, colorspace;
    int res = tjDecompressHeader3(
        jpegDecompressor, data, size, &width, &height, &subsamp, &colorspace);
    if (res != 0 || width == 0 || height == 0 || ((size_t)width * height > (1024 * 1024))) {
        tjDestroy(jpegDecompressor);
        return 0;
    }
    std::unique_ptr<unsigned char[]> buf(new unsigned char[width * height * 3]);
    tjDecompress2(jpegDecompressor, data, size, buf.get(), width, 0, height, TJPF_RGB, 0);
    tjDestroy(jpegDecompressor);
    return 0;
}
